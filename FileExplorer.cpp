#include <algorithm>
#include <filesystem>
#include <format>
#include <typeinfo>

#include <experimental/scope>

#include "FileExplorer.hpp"
#include "FindFile.hpp"
#include "utils.hpp"

using namespace wayround_i2p::ccedit;

FileExplorer_shared FileExplorer::create(
    ProjectCtl_shared proj_ctl
)
{
    auto ret = FileExplorer_shared(
        new FileExplorer(proj_ctl)
    );
    ret->own_ptr = ret;
    return ret;
}

FileExplorer::FileExplorer(ProjectCtl_shared proj_ctl) :
    main_box(Gtk::Orientation::VERTICAL, 5)
{
    this->proj_ctl = proj_ctl;

    maximize();
    updateTitle();

    main_box.set_margin(5);

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);
    // абв
    reset_view_btn.set_label("⏹️");
    reset_view_btn.set_tooltip_text("Reset View");
    go_root_btn.set_label("🏠");
    go_root_btn.set_tooltip_text("Nav to Project Root");
    refresh_btn.set_label("⟲");
    refresh_btn.set_tooltip_text("Refresh");

    filelauncher_dir_btn.set_label("🗁");
    filelauncher_dir_btn.set_tooltip_text("Open current Folder in System's App");
    find_file_btn.set_label("🔍");
    find_file_btn.set_tooltip_text("Find File..");

    make_file_or_directory_btn.set_label("🆕");
    make_file_or_directory_btn.set_tooltip_text("mk dir/file..");
    rename_file_or_directory_btn.set_label("✒️");
    rename_file_or_directory_btn.set_tooltip_text("Rename..");
    remove_file_or_directory_btn.set_label("␡");
    remove_file_or_directory_btn.set_tooltip_text("Delete File or Dir..");

    path_box.set_spacing(5);
    path_box.append(reset_view_btn);
    path_box.append(go_root_btn);
    path_box.append(refresh_btn);
    path_box.append(sep1);
    path_box.append(filelauncher_dir_btn);
    path_box.append(find_file_btn);
    path_box.append(reset_view_btn);
    path_box.append(sep2);
    path_box.append(make_file_or_directory_btn);
    path_box.append(rename_file_or_directory_btn);
    path_box.append(remove_file_or_directory_btn);
    path_box.append(sep3);
    path_box.append(path_entry);

    setupDirTreeView();
    setupFileListView();

    file_list_view.set_orientation(Gtk::Orientation::HORIZONTAL);
    file_list_view.set_max_columns(1000);

    lists_box.set_start_child(dir_tree_sw);
    lists_box.set_end_child(file_list_sw);
    lists_box.set_resize_start_child(false);

    lists_box.set_wide_handle(true);

    lists_box.set_vexpand(true);
    lists_box.set_hexpand(true);

    dir_tree_sw.set_has_frame(true);
    file_list_sw.set_has_frame(true);

    dir_tree_sw.set_child(dir_tree_view);
    file_list_sw.set_child(file_list_view);

    main_box.append(path_box);
    main_box.append(lists_box);

    set_child(main_box);

    reset_view_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_reset_view_btn)
    );

    go_root_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_go_root_btn)
    );

    refresh_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_refresh_btn)
    );

    filelauncher_dir_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_filelauncher_dir_btn)
    );

    find_file_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_find_file_btn)
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

    make_file_or_directory_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_make_file_or_directory_btn)
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
        sigc::bind( // todo: sigc::bind needed?
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
        sigc::bind( // todo: sigc::bind needed?
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

                auto f_path = proj_ctl->getProjectPath() / tlr->pth;

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
        new_title = std::format(
            "{} - File Explorer - Code Editor",
            proj_ctl->getProjectName()
        );
    }

    set_title(new_title);
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

    std::filesystem::path proj_path = proj_ctl->getProjectPath();
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
        } catch (std::exception &e)
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

    std::cout << "on_file_list_view_activate: " << item->pth << std::endl;

    auto pth = proj_ctl->getProjectPath() / item->pth;
    std::cout << "on_file_list_view_activate trying to open " << pth;

    // todo: add correctness checks

    auto f = Gio::File::create_for_path(pth.string());
    if (f->query_info()->get_file_type() == Gio::FileType::DIRECTORY)
    {
        dirTreeNavigateTo(pth);
    }
    else
    {
        proj_ctl->workSubjectEnsureExistance(pth);
        proj_ctl->workSubjectNewEditor(pth);
        // proj_ctl->workSubjectExistingOrNewEditor(pth);
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
    auto proj_path = proj_ctl->getProjectPath();

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
    auto x = FindFile::create(proj_ctl);
    x->show();
    proj_ctl->getController()->registerWindow(x);
}

void FileExplorer::on_make_file_or_directory_btn()
{
    auto x = FileExplorerMakeFileDir::create(own_ptr, opened_subdir);
    x->show();
    proj_ctl->getController()->registerWindow(x);
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    own_ptr.reset();
    std::cout << "FileExplorer sig destroy exit" << std::endl;
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

            auto projPath = proj_ctl->getProjectPath();

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

    auto proj_path = proj_ctl->getProjectPath();

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

    auto proj_path = proj_ctl->getProjectPath();

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

FileExplorerMakeFileDir_shared FileExplorerMakeFileDir::create(
    FileExplorer_shared expl,
    std::filesystem::path         subdir
)
{
    auto ret = FileExplorerMakeFileDir_shared(
        new FileExplorerMakeFileDir(
            expl,
            subdir
        )
    );
    ret->own_ptr = ret;
    return ret;
}

FileExplorerMakeFileDir::FileExplorerMakeFileDir(
    FileExplorer_shared expl,
    std::filesystem::path         subdir
)
{
    this->expl   = expl;
    this->subdir = subdir; // todo: rename to subpath

    set_child(main_box);

    main_box.set_margin(5);
    main_box.set_spacing(5);

    btn_box.set_spacing(5);

    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    btn_box.set_orientation(Gtk::Orientation::HORIZONTAL);

    main_grid.set_column_homogeneous(false);
    main_grid.set_vexpand(true);

    name_ent.set_hexpand(true);
    name_ent.set_valign(Gtk::Align::CENTER);

    main_box.append(main_grid);
    main_box.append(btn_box);

    placement_lbl2.set_text(subdir.string());

    main_grid.attach(placement_lbl, 0, 0);
    main_grid.attach(placement_lbl2, 1, 0);

    main_grid.attach(type_name_lbl, 0, 1);
    main_grid.attach(name_ent, 1, 1);

    btn_box.append(mk_dir_btn);
    btn_box.append(mk_file_btn);
    btn_box.append(cancel_btn);

    placement_lbl.set_text("create file or dir inside");
    type_name_lbl.set_text("select new name");

    mk_dir_btn.set_label("Make Dir");
    mk_file_btn.set_label("Make File");
    cancel_btn.set_label("Cancel");

    mk_dir_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorerMakeFileDir::on_mk_dir_btn)
    );

    mk_file_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorerMakeFileDir::on_mk_file_btn)
    );

    cancel_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorerMakeFileDir::on_cancel_btn)
    );

    signal_destroy().connect(
        sigc::mem_fun(
            *this,
            &FileExplorerMakeFileDir::on_destroy_sig
        )
    );
}

FileExplorerMakeFileDir::~FileExplorerMakeFileDir()
{
    std::cout << "~FileExplorerMakeFileDir()" << std::endl;
}

void FileExplorerMakeFileDir::on_destroy_sig()
{
    std::cout << "FileExplorerMakeFileDir sig destroy" << std::endl;
    own_ptr.reset();
    std::cout << "FileExplorerMakeFileDir sig destroy exit" << std::endl;
}

int FileExplorerMakeFileDir::common_func(bool file)
{
    // todo: maybe additional name_from_user cleanups needed
    // (space strippings/trimmings?)

    // todo: show messages if error?

    auto new_name = subdir / std::filesystem::path(name_ent.get_text());

    auto ret = expl->touchFileOrMkDirRelToProject(new_name, file);

    return ret;
}

void FileExplorerMakeFileDir::on_mk_dir_btn()
{
    common_func(false);
    expl->fileListRefresh();
    close();
}

void FileExplorerMakeFileDir::on_mk_file_btn()
{
    common_func(true);
    expl->fileListRefresh();
    close();
}

void FileExplorerMakeFileDir::on_cancel_btn()
{
    close();
}
