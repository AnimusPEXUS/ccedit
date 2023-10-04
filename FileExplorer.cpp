#include <filesystem>
#include <format>

#include "FileExplorer.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorer::FileExplorer(std::shared_ptr<ProjectCtl> proj_ctl) :
    main_box(Gtk::Orientation::VERTICAL, 5)
{
    this->proj_ctl = proj_ctl;

    updateTitle();

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    selected_file_lbl.set_text("<filename>");
    selected_file_lbl.set_hexpand(true);

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);

    go_home_btn.set_label("home");
    exit_folder_btn.set_label("..");

    path_box.append(go_home_btn);
    path_box.append(exit_folder_btn);
    path_box.append(refresh_btn);
    path_entry.set_margin_start(5);
    path_box.append(path_entry);

    setupDirTreeView();

    lists_box.set_start_child(dir_tree_sw);
    lists_box.set_end_child(file_list_sw);

    lists_box.set_wide_handle(true);

    lists_box.set_vexpand(true);
    lists_box.set_hexpand(true);

    dir_tree_sw.set_has_frame(true);
    file_list_sw.set_has_frame(true);

    dir_tree_sw.set_child(dir_tree_view);
    file_list_sw.set_child(file_list_view);

    temp_file_selector_btn.set_label("Browse..");
    temp_file_open_btn.set_label("Open");

    temp_file_selector_box.append(temp_file_selector_btn);
    temp_file_selector_box.append(selected_file_lbl);
    temp_file_selector_box.append(temp_file_open_btn);

    main_box.append(path_box);
    main_box.append(lists_box);
    main_box.append(temp_file_selector_box);

    set_child(main_box);

    temp_file_selector_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_temp_file_selector_btn)
    );

    temp_file_open_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_temp_file_open_btn)
    );

    refresh_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_refresh_btn)
    );

    proj_ctl->signal_updated_name()->connect(
        sigc::mem_fun(*this, &FileExplorer::updateTitle)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &FileExplorer::on_destroy_sig)
    );
}

FileExplorer::~FileExplorer()
{
    std::cout << "~FileExplorer()" << std::endl;
}

void FileExplorer::setupDirTreeView()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            [](
                const Glib::RefPtr<Gtk::ListItem> &list_item
            )
            {
                list_item->set_child(*Gtk::make_managed<Gtk::Label>(
                    "",
                    Gtk::Align::START
                )
                );
            }
        )
    );

    factory->signal_bind().connect(
        sigc::bind(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto col = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(
                    list_item->get_item()
                );
                if (!col)
                {
                    return;
                }

                auto label = dynamic_cast<Gtk::Label *>(
                    list_item->get_child()
                );
                if (!label)
                {
                    return;
                }

                std::string new_text = col->filename;
                label->set_text(new_text);
            }
        )
    );

    dir_tree_view.set_factory(factory);
}

void FileExplorer::updateTitle()
{
    std::string new_title("");

    if (proj_ctl->isGlobalProject())
    {
        new_title = "global - File Explorer - Code Editor";
    }
    else
    {
        int         err = 0;
        std::string name;
        std::tie(name, err) = proj_ctl->getProjectName();
        if (err != 0)
        {
            name = "can't determine project name";
        }
        new_title = std::format(
            "{} - File Explorer - Code Editor",
            name
        );
    }

    set_title(new_title);
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    delete this;
    std::cout << "FileExplorer sig destroy exit" << std::endl;
}

void FileExplorer::on_temp_file_selector_btn()
{
    select_file_dialog = Gtk::FileDialog::create();

    auto lbl_txt = selected_file_lbl.get_text();

    if (lbl_txt.length() != 0)
    {
        auto lbl_gio_file = Gio::File::create_for_path(lbl_txt);

        std::cout << "applying file to dialog: " << lbl_gio_file->get_path() << std::endl;

        select_file_dialog->set_initial_file(lbl_gio_file);
    }

    select_file_dialog->set_title("select a file to edit");
    select_file_dialog->open(
        *this,
        sigc::mem_fun(
            *this,
            &FileExplorer::on_temp_file_selector_btn_finish
        )
    );
}

void FileExplorer::on_temp_file_selector_btn_finish(
    std::shared_ptr<Gio::AsyncResult> res
)
{
    if (res == NULL)
    {
        // std::cout << "res == NULL\n";
        return;
    }

    // std::cout << "on_browse_click_finish\n";
    auto result = select_file_dialog->open_finish(res);

    if (result == NULL)
    {
        // std::cout << "result == NULL\n";
        return;
    }

    //  std::cout << format("path: {}\n", result->get_path());
    //  // cout << format("path: {}",result->get_path());
    //  std::cout.flush();
    //  //
    //

    selected_file_lbl.set_text(result->get_path());

    select_file_dialog.reset();
}

void FileExplorer::on_temp_file_open_btn()
{
    std::filesystem::path x(selected_file_lbl.get_text());
    proj_ctl->workSubjectEnsureExistance(x);
    proj_ctl->workSubjectNewEditor(x);
}

void FileExplorer::on_refresh_btn()
{
    auto [list_store, err] = dirTreeGenRootListStore("/");
    if (err != 0)
    {
        // todo: display error?
        return;
    }

    auto dir_tree_view_selection = Gtk::SingleSelection::create();
    dir_tree_view_selection->set_model(list_store);
    dir_tree_view.set_model(dir_tree_view_selection);
}

std::tuple<std::filesystem::path, int> FileExplorer::getProjectPath()
{
    return proj_ctl->getProjectPath();
}

std::tuple<
    Glib::RefPtr<Gio::ListModel>,
    int>
    FileExplorer::dirTreeGenRootListStore(std::filesystem::path subpath)
{
    // todo: check subpath sanyty. force to be relative?

    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
    }

    auto                  liststore    = Gio::ListStore<FileExplorerDirTreeRow>::create();
    std::filesystem::path path_to_list = proj_path / subpath;

    std::cout << "      proj_path: " << proj_path.string() << std::endl;
    std::cout << "   path_to_list: " << path_to_list.string() << std::endl;

    auto                                dir_itr = std::filesystem::directory_iterator(path_to_list);
    std::filesystem::directory_iterator end_itr;

    auto ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    while (true)
    {
        if (dir_itr == end_itr)
        {
            break;
        }
        auto entry = *dir_itr;

        if (entry.is_directory())
        {
            auto new_item      = FileExplorerDirTreeRow::create();
            new_item->filename = entry.path().filename();
            ret->append(new_item);
        }
        // std::cout << "  path: " << entry.path().string() << std::endl;
        dir_itr++;
    }

    return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(ret, 0);
}

/*int FileExplorer::dirTreeNavigateSubpath(std::filesystem::path subpath)
{

}*/
