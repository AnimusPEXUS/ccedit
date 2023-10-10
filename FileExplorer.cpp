#include <algorithm>
#include <filesystem>
#include <format>
#include <typeinfo>

#include <experimental/scope>

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

    file_list_view.set_homogeneous(false);

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
                std::cout << "setup bool(list_item): " << bool(list_item) << " typeid(list_item).name(): " << typeid(list_item).name() << std::endl;
                std::cout << "setup bool(list_item->get_child()): " << bool(list_item->get_child()) << " typeid(list_item->get_child()).name(): " << typeid(list_item->get_child()).name() << std::endl;
                std::cout << "setup bool(list_item->get_item()): " << bool(list_item->get_item()) << " typeid(list_item->get_item()).name(): " << typeid(list_item->get_item()).name() << std::endl;

                /*
                        auto list_item_item = list_item->get_item();
                        auto tlr            = std::dynamic_pointer_cast<Gtk::TreeListRow>(list_item_item);
                        if (!tlr)
                        {
                            std::cout << "!Gtk::TreeListRow " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                            return;
                        }
                        */

                auto exp = Gtk::make_managed<Gtk::TreeExpander>();
                auto box = Gtk::make_managed<Gtk::Box>();
                box->set_spacing(5);
                auto pic = Gtk::make_managed<Gtk::Image>();
                auto lbl = Gtk::make_managed<Gtk::Label>("", Gtk::Align::START);
                box->append(*pic);
                box->append(*lbl);
                exp->set_child(*box);
                list_item->set_child(*exp);
                // exp->set_list_row(tlr);
            }
        )
    );

    factory->signal_bind().connect(
        sigc::bind(
            [this](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                std::cout << "bind bool(list_item): " << bool(list_item) << " typeid(list_item).name(): " << typeid(list_item).name() << std::endl;
                std::cout << "bind bool(list_item->get_child()): " << bool(list_item->get_child()) << " typeid(list_item->get_child()).name(): " << typeid(list_item->get_child()).name() << std::endl;
                std::cout << "bind bool(list_item->get_item()): " << bool(list_item->get_item()) << " typeid(list_item->get_item()).name(): " << typeid(list_item->get_item()).name() << std::endl;

                auto list_item_item = list_item->get_item();
                auto tlr            = std::dynamic_pointer_cast<Gtk::TreeListRow>(list_item_item);
                if (!tlr)
                {
                    std::cout << "!Gtk::TreeListRow " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                    return;
                }
                std::cout << "is Gtk::TreeListRow" << std::endl;

                auto exp = dynamic_cast<Gtk::TreeExpander *>(list_item->get_child());
                if (!exp)
                {
                    std::cout << "!Gtk::TreeExpander " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                    return;
                }
                exp->set_list_row(tlr);

                auto box = dynamic_cast<Gtk::Box *>(exp->get_child());
                if (!box)
                {
                    std::cout << "!Gtk::Box " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                    return;
                }

                auto pic = dynamic_cast<Gtk::Image *>(box->get_first_child());
                if (!pic)
                {
                    std::cout << "!Gtk::Image " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                    return;
                }

                auto label = dynamic_cast<Gtk::Label *>(box->get_last_child());
                if (!label)
                {
                    std::cout << "!Gtk::Label " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
                    return;
                }

                auto f_path = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(tlr->get_item())->pth;

                pic->set_from_icon_name("folder");

                std::string new_text = f_path.filename();
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
    auto err = navigateToRoot();
    if (err != 0)
    {
        // todo: display error?
        return;
    }
}

std::filesystem::path calcSubpathByDirTreeRow(Glib::RefPtr<Gtk::TreeListRow> row)
{
    std::filesystem::path ret("");

    Glib::RefPtr<Gtk::TreeListRow> cur_row  = row;
    Glib::RefPtr<Gtk::TreeListRow> next_row = row;

    for (;;)
    {
        if (!next_row)
        {
            break;
        }
        cur_row  = next_row;
        ret      = std::dynamic_pointer_cast<FileExplorerDirTreeRow>((cur_row->get_item()))->pth.filename() / ret;
        next_row = cur_row->get_parent();
    }

    return ret;
}

int FileExplorer::navigateToRoot()
{
    auto [list_store, err] = dirTreeGenDirListStore("/");
    if (err != 0)
    {
        return err;
    }

    auto dir_tree_view_selection = Gtk::SingleSelection::create();

    auto x = Gtk::TreeListModel::create(
        list_store,
        [this](const Glib::RefPtr<Glib::ObjectBase> &item)
        {
            // auto ret_null     = Glib::RefPtr<Gio::ListStore<FileExplorerDirTreeRow>>();
            Glib::RefPtr<Gio::ListModel> ret_null = nullptr;
            // auto                                                 ret_initiated = Gio::ListStore<FileExplorerDirTreeRow>::create();

            auto tree_row = item.get();

            int err = 0;

            std::filesystem::path        projPath;
            Glib::RefPtr<Gio::ListModel> res_good;

            auto cast_res = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(item);

            std::cout << "asked to create children for row with: " << cast_res->pth << std::endl;

            std::tie(projPath, err) = getProjectPath();
            if (err != 0)
            {
                // todo: report error
                return ret_null;
            }

            std::tie(res_good, err) = dirTreeGenDirListStore(std::filesystem::relative(cast_res->pth, projPath));
            if (err != 0)
            {
                // todo: report error
                return ret_null;
            }

            std::cout << "Gtk::TreeListModel::create cb" << std::endl;
            return res_good;
        }
    );

    dir_tree_view_selection->set_model(x);
    dir_tree_view.set_model(dir_tree_view_selection);

    fileListNavigateTo("/");

    return 0;
}

std::tuple<std::filesystem::path, int> FileExplorer::getProjectPath()
{
    return proj_ctl->getProjectPath();
}

int FileExplorer::fileListNavigateTo(std::filesystem::path subpath)
{
    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return err;
    }

    std::filesystem::path path_to_list = proj_path / subpath;

    // todo: Im on fedora 38 with gtk 4.10. I don't have gtk_flow_box_remove_all() function
    while (true)
    {
        auto c = file_list_view.get_first_child();
        if (c == nullptr)
        {
            break;
        }
        file_list_view.remove(*c);
    }

    auto                                dir_itr = std::filesystem::directory_iterator(path_to_list);
    std::filesystem::directory_iterator end_itr;

    for (;;)
    {
        if (dir_itr == end_itr)
        {
            break;
        }
        auto entry = *dir_itr;

        if (!entry.is_directory())
        {
            // todo: I think something wrong here. testing and maybe fixing needed (mem leak?)
            auto new_item = FileExplorerFileListItem(entry.path().filename());
            new_item.set_vexpand(false);
            new_item.set_hexpand(false);
            file_list_view.append(new_item);
        }
        dir_itr++;
    }

    return 0;
}

/*
// c++ version of this function have exception handling problems
std::tuple<
    Glib::RefPtr<Gio::ListModel>,
    int>
    FileExplorer::dirTreeGenDirListStore(std::filesystem::path subpath)
{

    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
    }

    auto                  liststore    = Gio::ListStore<FileExplorerDirTreeRow>::create();
    std::filesystem::path path_to_list = proj_path / subpath;

    std::filesystem::directory_iterator dir_itr;
    try
    {
        dir_itr = std::filesystem::directory_iterator(path_to_list);
    }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cout << "std::filesystem::filesystem_error: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (const std::exception &e)
        {
            std::cout << "std::exception: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (std::exception &e)
        {
            std::cout << "std::exception: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (...)
        {
            std::cout << "unknown exception: " << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }

    std::filesystem::directory_iterator end_itr;

    auto ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    while (true)
    {
        if (dir_itr == end_itr)
        {
            break;
        }
        std::filesystem::directory_entry entry;

        try
        {
            entry = *dir_itr;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cout << "std::filesystem::filesystem_error: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (const std::exception &e)
        {
            std::cout << "std::exception: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (std::exception &e)
        {
            std::cout << "std::exception: " << e.what() << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }
        catch (...)
        {
            std::cout << "unknown exception: " << std::endl;
            // todo: maybe here should be not return, but loop have to continue to iterate
            return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
        }

        if (entry.is_directory())
        {
            auto new_item = FileExplorerDirTreeRow::create();
            new_item->pth = entry.path();
            ret->append(new_item);
        }
        // std::cout << "  path: " << entry.path().string() << std::endl;
        dir_itr++;
    }

    return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(ret, 0);
} */

std::tuple<
    Glib::RefPtr<Gio::ListModel>,
    int>
    FileExplorer::dirTreeGenDirListStore(std::filesystem::path subpath)
{
    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
    }

    auto                  liststore    = Gio::ListStore<FileExplorerDirTreeRow>::create();
    std::filesystem::path path_to_list = proj_path / subpath;

    auto dir = Gio::File::create_for_path(path_to_list.string());

    auto enumerator = dir->enumerate_children();

    auto sg02 = std::experimental::fundamentals_v3::scope_exit(
        [&enumerator]()
        {
            enumerator->close();
        }
    );

    auto                                              ret = Gio::ListStore<FileExplorerDirTreeRow>::create();
    std::vector<Glib::RefPtr<FileExplorerDirTreeRow>> items;

    for (;;)
    {
        auto fi = enumerator->next_file();
        if (fi == nullptr)
        {
            break;
        }
        auto name = fi->get_name();
        auto type = fi->get_file_type();
        if (type == Gio::FileType::DIRECTORY)
        {
            auto new_item = FileExplorerDirTreeRow::create();
            new_item->pth = path_to_list / name;
            items.push_back(new_item);
        }
    }

    std::sort(
        items.begin(),
        items.end(),
        [](
            const Glib::RefPtr<FileExplorerDirTreeRow> &a,
            const Glib::RefPtr<FileExplorerDirTreeRow> &b
        )
        {
            return a->pth.filename() < b->pth.filename();
        }
    );

    for (auto i : items)
    {
        ret->append(i);
    }

    return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(ret, 0);
}

/*int FileExplorer::dirTreeNavigateSubpath(std::filesystem::path subpath)
{

}*/
