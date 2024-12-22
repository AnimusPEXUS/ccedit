#include <algorithm>
#include <filesystem>
#include <format>
#include <typeinfo>

#include <experimental/scope>

#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "FileExplorerMakeFileDir.hpp"
#include "FindFile.hpp"
#include "ProjectCtl.hpp"

#include "utils.hpp"

namespace wayround_i2p::ccedit
{

FileExplorer_shared FileExplorer::create(
    ProjectCtl_shared project_ctl
)
{
    auto ret     = FileExplorer_shared(new FileExplorer(project_ctl));
    ret->own_ptr = ret;
    project_ctl->registerFileExplorer(ret);
    return ret;
}

FileExplorer::FileExplorer(ProjectCtl_shared project_ctl) :
    main_box(Gtk::Orientation::VERTICAL, 0),
    wmg(project_ctl),
    destroyer(
        [this]()
        {
            std::cout << "FileExplorer::destroyer.run()" << std::endl;
            this->project_ctl->unregisterFileExplorer(own_ptr);
            win.destroy();
            own_ptr.reset();
        }
    )
{
    this->project_ctl = project_ctl;

    // maximize();
    updateTitle();

    // main_box.set_margin(5);

    // main_box.append(menu_bar);

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);
    // абв

    path_box.add_css_class("toolbar");

    fb1.add_css_class("linked");
    fb2.add_css_class("linked");
    fb3.add_css_class("linked");

    reset_view_btn.set_image_from_icon_name("edit-clear-all");
    reset_view_btn.set_tooltip_text("Reset View");
    go_root_btn.set_image_from_icon_name("go-home");
    go_root_btn.set_tooltip_text("Nav to Project Root");
    refresh_btn.set_image_from_icon_name("view-refresh");
    refresh_btn.set_tooltip_text("Refresh");

    filelauncher_dir_btn.set_image_from_icon_name("document-open");
    filelauncher_dir_btn.set_tooltip_text("Open current Folder in System's App");
    find_file_btn.set_image_from_icon_name("system-search");
    find_file_btn.set_tooltip_text("Find File..");

    make_file_or_directory_btn.set_image_from_icon_name("document-new");
    make_file_or_directory_btn.set_tooltip_text("mk dir/file..");

    show_windows_btn.set_menu_model(
        wmg.createProjectMenu("file_explorer_window")
    );
    show_windows_btn.add_css_class("circular");
    show_windows_btn.set_has_frame(true);
    show_windows_btn.set_icon_name("applications-utilities");
    show_windows_btn.set_tooltip_text("Project Menu");

    path_entry.set_hexpand(true);

    // path_box.set_spacing(5);

    fb1.append(reset_view_btn);
    fb1.append(go_root_btn);
    fb1.append(refresh_btn);

    path_box.append(fb1);
    // path_box.append(sep1);

    fb2.append(filelauncher_dir_btn);
    fb2.append(find_file_btn);
    fb2.append(reset_view_btn);

    path_box.append(fb2);
    // path_box.append(sep2);

    fb3.append(make_file_or_directory_btn);
    // fb3.append(rename_file_or_directory_btn);
    // fb3.append(remove_file_or_directory_btn);

    path_box.append(fb3);
    // path_box.append(sep3);

    path_box.append(show_windows_btn);

    path_box.append(path_entry);

    setup_actions();
    setup_main_menu();
    setup_hotkeys();

    setupDirTreeView();
    setupFileListView();

    file_list_view.set_orientation(Gtk::Orientation::HORIZONTAL);
    file_list_view.set_max_columns(1000);

    lists_box.set_start_child(dir_tree_sw);
    lists_box.set_end_child(file_list_sw);
    lists_box.set_resize_start_child(false);

    lists_box.set_wide_handle(false);

    lists_box.set_vexpand(true);
    lists_box.set_hexpand(true);

    // dir_tree_sw.set_has_frame(true);
    // file_list_sw.set_has_frame(true);

    dir_tree_sw.set_child(dir_tree_view);
    file_list_sw.set_child(file_list_view);

    main_box.append(path_box);
    main_box.append(lists_box);

    win.set_child(main_box);

    on_project_rename_slot->setFun(
        [this]()
        { updateTitle(); }
    );

    project_ctl->signal_updated_name().connect(on_project_rename_slot);

    reset_view_btn.signal_clicked().connect(
        [this]()
        { on_reset_view_btn(); }
    );

    go_root_btn.signal_clicked().connect(
        [this]()
        { on_go_root_btn(); }
    );

    refresh_btn.signal_clicked().connect(
        [this]()
        { on_refresh_btn(); }
    );

    filelauncher_dir_btn.signal_clicked().connect(
        [this]()
        { on_filelauncher_dir_btn(); }
    );

    find_file_btn.signal_clicked().connect(
        [this]()
        { on_find_file_btn(); }
    );

    dir_tree_view.signal_activate().connect(
        [this](guint pos)
        { on_dir_tree_view_activate(pos); }
    );

    file_list_view.signal_activate().connect(
        [this](guint pos)
        { on_file_list_view_activate(pos); }
    );

    make_file_or_directory_btn.signal_clicked().connect(
        [this]()
        { on_make_file_or_directory_btn(); }
    );

    win.signal_destroy().connect(
        [this]()
        { on_destroy_sig(); }
    );

    win.signal_close_request().connect(
        [this]() -> bool
        { return on_signal_close_request(); },
        true
    );
}

FileExplorer::~FileExplorer()
{
    std::cout << "~FileExplorer()" << std::endl;
    destroyer.run();
}

void FileExplorer::show()
{
    win.present();
}

void FileExplorer::destroy()
{
    destroyer.run();
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    destroyer.run();
    std::cout << "FileExplorer sig destroy exit" << std::endl;
}

bool FileExplorer::on_signal_close_request()
{
    std::cout << "FileExplorer::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

Gtk::Window &FileExplorer::getWindowRef()
{
    return win;
}

Gtk::Window *FileExplorer::getWindowPtr()
{
    return &win;
}

void FileExplorer::setupDirTreeView()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
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
    );

    factory->signal_bind().connect(
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
    );

    dir_tree_view.set_factory(factory);
}

void FileExplorer::setupFileListView()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
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
    );

    factory->signal_bind().connect(
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

            auto f_path = project_ctl->getProjectPath() / tlr->pth;

            auto fi   = Gio::File::create_for_path(f_path);
            auto fii  = fi->query_info();
            auto icon = fii->get_icon();

            pic->set(icon);

            std::string new_text = f_path.filename();
            label->set_text(new_text);
        }
    );

    file_list_view.set_factory(factory);
}

void FileExplorer::setup_main_menu()
{
}

void FileExplorer::setup_actions()
{
    auto action_group = Gio::SimpleActionGroup::create();

    wmg.addActionsToActionGroup(action_group);

    win.insert_action_group("file_explorer_window", action_group);
}

void FileExplorer::setup_hotkeys()
{
    auto controller = Gtk::ShortcutController::create();
    controller->set_scope(Gtk::ShortcutScope::LOCAL);

    /*
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_r,
            Gdk::ModifierType::CONTROL_MASK
        ),
        Gtk::NamedAction::create("file_explorer_window.work_subject_reload")
    ));
*/
    win.add_controller(controller);
}

void FileExplorer::updateTitle()
{
    std::string new_title("");

    if (project_ctl->isGlobalProject())
    {
        new_title = "global - File Explorer - Code Editor";
    }
    else
    {
        new_title = std::format(
            "{} - File Explorer - Code Editor",
            project_ctl->getProjectName()
        );
    }

    win.set_title(new_title);
}

int FileExplorer::touchFileOrMkDirRelToProject(
    std::filesystem::path subpath,
    bool                  file
)
{
    return touchFileOrMkDir(subpath, file, false);
}

int FileExplorer::touchFileOrMkDirRelToCurrent(
    std::filesystem::path subpath,
    bool                  file
)
{
    return touchFileOrMkDir(subpath, file, true);
}

int FileExplorer::touchFileOrMkDir(
    std::filesystem::path subpath,
    bool                  file,
    bool                  rel_to_current
)
{
    int err = 0;

    subpath = subpath.relative_path();

    err = check_relpath_is_relative_and_sane(subpath);
    if (err != 0)
    {
        return err;
    }

    std::filesystem::path proj_path = project_ctl->getProjectPath();
    std::filesystem::path base_path = proj_path;

    if (rel_to_current)
    {
        base_path /= opened_subdir;
    }

    auto full_path = base_path / subpath;

    full_path = full_path.lexically_normal();

    {
        auto tmp_rel_path = std::filesystem::relative(full_path, proj_path);
        err               = check_relpath_is_relative_and_sane(tmp_rel_path);
        if (err != 0)
        {
            return 4;
        }
    }

    if (file)
    {
        auto f = fopen(full_path.string().c_str(), "w");
        if (f == nullptr)
        {
            return 2;
        }
        fclose(f);
    }
    else
    {
        try
        {
            create_directories(full_path);
        }
        catch (std::exception &e)
        {
            return 3;
        }
    }

    return 0;
}

void FileExplorer::on_dir_tree_view_activate(guint pos)
{
    int err = 0;
    std::cout << "on_dir_tree_view_activate pos " << pos;
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

    auto pth = item->pth;

    std::cout << "on_dir_tree_view_activate trying to navigate to " << pth;
    fileListNavigateTo(pth);
}

void FileExplorer::on_file_list_view_activate(guint pos)
{
    int  err = 0;
    auto sel = std::dynamic_pointer_cast<Gtk::MultiSelection>(
        file_list_view.get_model()
    );
    if (!sel)
    {
        std::cout << "!sel" << __FILE__ << " : " << __LINE__ << std::endl;
        return;
    }

    auto model
        = std::dynamic_pointer_cast<Gio::ListStore<FileExplorerFileListRow>>(
            sel->get_model()
        );
    if (!model)
    {
        std::cout << "!model" << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(sel->get_model()).name() << std::endl;
        return;
    }

    auto item
        = std::dynamic_pointer_cast<FileExplorerFileListRow>(
            model->get_item(pos)
        );
    if (!item)
    {

        std::cout << "!item: " << __FILE__ << " : " << __LINE__ << std::endl;
        std::cout << typeid(model->get_item(pos)).name() << std::endl;
        return;
    }

    std::cout << "on_file_list_view_activate: " << item->pth << std::endl;

    auto pth      = item->pth;
    auto pth_full = project_ctl->getProjectPath() / pth;

    std::cout << "on_file_list_view_activate trying to open " << pth_full << std::endl;

    // todo: add correctness checks

    auto f = Gio::File::create_for_path(pth_full.string());

    /*
    auto sg01 = std::experimental::fundamentals_v3::scope_exit(
        [&f]()
        {
            f->close();
        }
    );
    */

    if (f->query_info()->get_file_type() == Gio::FileType::DIRECTORY)
    {
        dirTreeNavigateTo(pth);
    }
    else
    {
        auto res = project_ctl->workSubjectEnsureExistance(pth);
        if (!res)
        {
            // todo: report
            return;
        }
        project_ctl->workSubjectNewEditor(pth);
        // project_ctl->workSubjectExistingOrNewEditor(pth);
    }
}

void FileExplorer::on_reset_view_btn()
{
    auto err = navigateToRoot();
    if (err != 0)
    {
        // todo: display error?
        return;
    }
}

void FileExplorer::on_go_root_btn()
{
    auto err = fileListNavigateTo("/");
    if (err != 0)
    {
        // todo: display error?
        return;
    }
}

void FileExplorer::on_refresh_btn()
{
    // todo: refresh dir tree
    auto err = fileListNavigateTo(opened_subdir);
    if (err != 0)
    {
        // todo: display error?
        return;
    }
}

void FileExplorer::on_filelauncher_dir_btn()
{
    auto proj_path = project_ctl->getProjectPath();

    auto lun = Gtk::FileLauncher::create(
        Gio::File::create_for_path((proj_path / opened_subdir).string())
    );
    lun->launch(
        [&lun](Glib::RefPtr<Gio::AsyncResult> &res)
        {
            std::cout << "launch launched" << std::endl;
        }
    );
}

void FileExplorer::on_find_file_btn()
{
    auto x = FindFile::create(project_ctl);
    x->show();
}

void FileExplorer::on_make_file_or_directory_btn()
{
    auto x = FileExplorerMakeFileDir::create(own_ptr, opened_subdir);
    x->show();
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

            Glib::RefPtr<Gio::ListModel> res_good;

            auto cast_res = std::dynamic_pointer_cast<FileExplorerDirTreeRow>(item);

            std::cout << "asked to create children for row with: " << cast_res->pth << std::endl;

            auto projPath = project_ctl->getProjectPath();

            // std::tie(res_good, err) = dirTreeGenDirListStore(
            //  std::filesystem::relative(cast_res->pth, projPath)
            // );
            std::tie(res_good, err) = dirTreeGenDirListStore(
                cast_res->pth
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

int FileExplorer::dirTreeNavigateTo(std::filesystem::path subpath)
{
    /*
    subpath = subpath.relative_path();

    auto proj_path = proj_ctl->getProjectPath();
    */

    // todo: todo

    return 0;
}

int FileExplorer::fileListRefresh()
{
    return fileListNavigateTo(opened_subdir);
}

int FileExplorer::fileListNavigateTo(std::filesystem::path subpath)
{

    // todo: subpath sanity checks erquired here, there and everythere

    subpath = subpath.relative_path();

    auto proj_path = project_ctl->getProjectPath();

    std::filesystem::path path_to_list = proj_path / subpath;

    std::cout << "fileListNavigateTo path_to_list: " << path_to_list << std::endl;

    auto dir = Gio::File::create_for_path(path_to_list.string());

    // todo: check dir is directory

    auto enumerator = dir->enumerate_children();

    auto sg02 = std::experimental::fundamentals_v3::scope_exit(
        [&enumerator]()
        {
            enumerator->close();
        }
    );

    auto ret = Gio::ListStore<FileExplorerFileListRow>::create();

    std::vector<Glib::RefPtr<FileExplorerFileListRow>> dir_items;
    std::vector<Glib::RefPtr<FileExplorerFileListRow>> file_items;

    for (;;)
    {
        auto fi = enumerator->next_file();
        if (fi == nullptr)
        {
            break;
        }
        auto name = fi->get_name();
        auto type = fi->get_file_type();

        std::cout << "fileListNavigateTo    fi->name: " << name << std::endl;

        auto new_item          = FileExplorerFileListRow::create();
        auto complete_filename = path_to_list / name;
        std::cout << "fileListNavigateTo    complete_filename: " << complete_filename << std::endl;
        new_item->pth = std::filesystem::relative(
            complete_filename,
            proj_path
        );
        std::cout << "fileListNavigateTo    new_item->pth: " << new_item->pth << std::endl;
        if (type == Gio::FileType::DIRECTORY)
        {
            dir_items.push_back(new_item);
        }
        else
        {
            file_items.push_back(new_item);
        }
    }

    std::sort(
        dir_items.begin(),
        dir_items.end(),
        [](
            const Glib::RefPtr<FileExplorerFileListRow> &a,
            const Glib::RefPtr<FileExplorerFileListRow> &b
        )
        {
            return a->pth.filename() < b->pth.filename();
        }
    );

    std::sort(
        file_items.begin(),
        file_items.end(),
        [](
            const Glib::RefPtr<FileExplorerFileListRow> &a,
            const Glib::RefPtr<FileExplorerFileListRow> &b
        )
        {
            return a->pth.filename() < b->pth.filename();
        }
    );

    for (auto i : dir_items)
    {
        ret->append(i);
    }

    for (auto i : file_items)
    {
        ret->append(i);
    }

    auto sel = Gtk::MultiSelection::create();
    sel->set_model(ret);
    file_list_view.set_model(sel);

    // todo: rename 'opened_subdir' variable to something better
    opened_subdir = subpath;

    return 0;
}

std::tuple<Glib::RefPtr<Gio::ListModel>, int>
    FileExplorer::dirTreeGenDirListStore(std::filesystem::path subpath)
{
    // todo: more subpath checks and sanitations
    subpath = subpath.relative_path();

    auto err_ret = Gio::ListStore<FileExplorerDirTreeRow>::create();

    auto proj_path = project_ctl->getProjectPath();

    std::filesystem::path path_to_list = proj_path / subpath;

    std::cout << "dirTreeGenDirListStore path_to_list: " << path_to_list << std::endl;

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
            auto new_item          = FileExplorerDirTreeRow::create();
            auto complete_filename = path_to_list / name;
            new_item->pth          = std::filesystem::relative(
                complete_filename,
                proj_path
            );
            items.push_back(new_item);
        }
    }

    // todo: use ListStore sorting mechanism
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

} // namespace wayround_i2p::ccedit
