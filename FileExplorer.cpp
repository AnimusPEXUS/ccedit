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

    main_box.set_margin(5);

    selected_file_lbl.set_text("<filename>");
    selected_file_lbl.set_hexpand(true);

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);

    go_home_btn.set_label("home");
    exit_folder_btn.set_label("..");

    path_box.set_spacing(5);
    path_box.append(go_home_btn);
    path_box.append(exit_folder_btn);
    path_box.append(refresh_btn);
    path_box.append(path_entry);

    setupDirTreeView();
    setupFileListView();

    file_list_view.set_orientation(Gtk::Orientation::HORIZONTAL);
    file_list_view.set_max_columns(1000);

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

    dir_tree_view.signal_activate().connect(
        sigc::mem_fun(*this, &FileExplorer::on_dir_tree_view_activate)
    );

    file_list_view.signal_activate().connect(
        sigc::mem_fun(*this, &FileExplorer::on_file_list_view_activate)
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
                auto exp = Gtk::make_managed<Gtk::TreeExpander>();
                auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);
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
                auto list_item_item = list_item->get_item();

                auto tlr = std::dynamic_pointer_cast<Gtk::TreeListRow>(list_item_item);
                if (!tlr)
                {
                    return;
                }

                auto exp = dynamic_cast<Gtk::TreeExpander *>(list_item->get_child());
                if (!exp)
                {
                    return;
                }
                exp->set_list_row(tlr);

                auto box = dynamic_cast<Gtk::Box *>(exp->get_child());
                if (!box)
                {
                    return;
                }

                auto pic = dynamic_cast<Gtk::Image *>(box->get_first_child());
                if (!pic)
                {
                    return;
                }

                auto label = dynamic_cast<Gtk::Label *>(box->get_last_child());
                if (!label)
                {
                    return;
                }

                auto f_path = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(
                                  tlr->get_item()
                )
                                  ->pth;

                pic->set_from_icon_name("folder");

                std::string new_text = f_path.filename();
                label->set_text(new_text);
            }
        )
    );

    dir_tree_view.set_factory(factory);
}

void FileExplorer::setupFileListView()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            [](
                const Glib::RefPtr<Gtk::ListItem> &list_item
            )
            {
                auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);
                auto pic = Gtk::make_managed<Gtk::Image>();
                auto lbl = Gtk::make_managed<Gtk::Label>("", Gtk::Align::START);
                box->append(*pic);
                box->append(*lbl);
                box->set_margin(0);
                box->set_hexpand(false);
                box->set_vexpand(false);
                list_item->set_child(*box);
            }
        )
    );

    factory->signal_bind().connect(
        sigc::bind(
            [this](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto list_item_item = list_item->get_item();

                auto tlr = std::dynamic_pointer_cast<FileExplorerFileListRow>(list_item_item);
                if (!tlr)
                {
                    return;
                }

                auto box = dynamic_cast<Gtk::Box *>(list_item->get_child());
                if (!box)
                {
                    return;
                }

                auto pic = dynamic_cast<Gtk::Image *>(box->get_first_child());
                if (!pic)
                {
                    return;
                }

                auto label = dynamic_cast<Gtk::Label *>(box->get_last_child());
                if (!label)
                {
                    return;
                }

                auto f_path = tlr->pth;

                auto fi   = Gio::File::create_for_path(f_path);
                auto fii  = fi->query_info();
                auto icon = fii->get_icon();

                pic->set(icon);

                std::string new_text = f_path.filename();
                label->set_text(new_text);
            }
        )
    );

    file_list_view.set_factory(factory);
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

void FileExplorer::on_dir_tree_view_activate(guint pos)
{
    int  err = 0;
    auto sel = std::dynamic_pointer_cast<Gtk::SingleSelection>(dir_tree_view.get_model());
    if (!sel)
    {
        std::cout << "!sel" << __FILE__ << " : " << __LINE__ << std::endl;
        return;
    }

    auto model = std::dynamic_pointer_cast<Gtk::TreeListModel>(sel->get_model());
    if (!model)
    {
        std::cout << "!model" << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(sel->get_model()).name() << std::endl;
        return;
    }

    auto row  = model->get_row(pos);
    auto item = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(row->get_item());
    if (!item)
    {

        std::cout << "!item: " << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(row->get_item()).name() << std::endl;
        return;
    }

    // std::cout << item->pth << std::endl;

    auto                  pth = item->pth;
    std::filesystem::path base;
    std::tie(base, err) = getProjectPath();
    if (err != 0)
    {
        return;
    }
    // todo: add correctness check
    auto pth_rel = std::filesystem::relative(pth, base);
    fileListNavigateTo(pth_rel);
}

void FileExplorer::on_file_list_view_activate(guint pos)
{
    int  err = 0;
    auto sel = std::dynamic_pointer_cast<Gtk::MultiSelection>(file_list_view.get_model());
    if (!sel)
    {
        std::cout << "!sel" << __FILE__ << " : " << __LINE__ << std::endl;
        return;
    }

    auto model = std::dynamic_pointer_cast<Gio::ListStore<FileExplorerFileListRow>>(sel->get_model());
    if (!model)
    {
        std::cout << "!model" << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(sel->get_model()).name() << std::endl;
        return;
    }

    auto item = std::dynamic_pointer_cast<FileExplorerFileListRow>(model->get_item(pos));
    if (!item)
    {

        std::cout << "!item: " << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(model->get_item(pos)).name() << std::endl;
        return;
    }

    // std::cout << item->pth << std::endl;

    auto pth = item->pth;

    // todo: add correctness checks

    selected_file_lbl.set_text(pth.string());
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
        [this](std::shared_ptr<Gio::AsyncResult> res)
        {
            if (res == NULL)
            {
                return;
            }

            auto result = select_file_dialog->open_finish(res);

            if (result == NULL)
            {
                return;
            }

            selected_file_lbl.set_text(result->get_path());

            select_file_dialog.reset();
        }
    );
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
        cur_row = next_row;
        ret     = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(
                  (cur_row->get_item())
              )
                  ->pth.filename()
            / ret;
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
            Glib::RefPtr<Gio::ListModel> ret_null = nullptr;

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

            std::tie(res_good, err) = dirTreeGenDirListStore(
                std::filesystem::relative(cast_res->pth, projPath)
            );
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

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return err;
    }

    std::filesystem::path path_to_list = proj_path / subpath;

    auto dir = Gio::File::create_for_path(path_to_list.string());

    auto enumerator = dir->enumerate_children();

    auto sg02 = std::experimental::fundamentals_v3::scope_exit(
        [&enumerator]()
        {
            enumerator->close();
        }
    );

    auto ret = Gio::ListStore<FileExplorerFileListRow>::create();

    std::vector<Glib::RefPtr<FileExplorerFileListRow>> items;

    for (;;)
    {
        auto fi = enumerator->next_file();
        if (fi == nullptr)
        {
            break;
        }
        auto name = fi->get_name();
        auto type = fi->get_file_type();
        if (type != Gio::FileType::DIRECTORY)
        {
            auto new_item = FileExplorerFileListRow::create();
            new_item->pth = path_to_list / name;
            items.push_back(new_item);
        }
    }

    std::sort(
        items.begin(),
        items.end(),
        [](
            const Glib::RefPtr<FileExplorerFileListRow> &a,
            const Glib::RefPtr<FileExplorerFileListRow> &b
        )
        {
            return a->pth.filename() < b->pth.filename();
        }
    );

    for (auto i : items)
    {
        ret->append(i);
    }

    auto sel = Gtk::MultiSelection::create();
    sel->set_model(ret);
    file_list_view.set_model(sel);

    return 0;
}

std::tuple<Glib::RefPtr<Gio::ListModel>, int>
    FileExplorer::dirTreeGenDirListStore(std::filesystem::path subpath)
{
    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto [proj_path, err] = getProjectPath();
    if (err != 0)
    {
        return std::tuple<Glib::RefPtr<Gio::ListModel>, int>(err_ret, err);
    }

    std::filesystem::path path_to_list = proj_path / subpath;

    auto dir = Gio::File::create_for_path(path_to_list.string());

    auto enumerator = dir->enumerate_children();

    auto sg02 = std::experimental::fundamentals_v3::scope_exit(
        [&enumerator]()
        {
            enumerator->close();
        }
    );

    auto ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

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
