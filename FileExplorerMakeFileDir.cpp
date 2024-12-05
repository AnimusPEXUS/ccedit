#include "FileExplorerMakeFileDir.hpp"

#include "FileExplorer.hpp"

namespace wayround_i2p::ccedit
{

FileExplorerMakeFileDir_shared FileExplorerMakeFileDir::create(
    FileExplorer_shared   expl,
    std::filesystem::path subdir
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
    FileExplorer_shared   expl,
    std::filesystem::path subdir
) :
    destroyer(
        [this]()
        {
            win.destroy();
            own_ptr.reset();
        }
    )
{
    this->expl   = expl;
    this->subdir = subdir; // todo: rename to subpath

    win.set_title("Touch File or Make Directory");

    win.set_child(main_box);

    main_box.set_margin(5);
    main_box.set_spacing(5);

    // btn_box.set_spacing();

    btn_box.add_css_class("linked");
    btn_box.set_homogeneous(true);

    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    btn_box.set_orientation(Gtk::Orientation::HORIZONTAL);

    main_grid.set_column_homogeneous(false);
    main_grid.set_vexpand(true);

    name_ent.set_hexpand(true);
    name_ent.set_valign(Gtk::Align::CENTER);

    main_box.append(main_grid);
    main_box.append(btn_box);

    placement_ent.set_text(subdir.string());
    placement_ent.set_editable(false);

    main_grid.set_vexpand(true);

    main_grid.set_row_homogeneous(true);

    main_grid.set_column_spacing(5);
    main_grid.set_row_spacing(5);

    main_grid.attach(placement_lbl, 0, 0);
    main_grid.attach(placement_ent, 1, 0);

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

    win.set_focus(name_ent);

    win.set_transient_for(expl->getWindowRef());
    win.set_destroy_with_parent(true);
    win.set_modal(true);

    mk_dir_btn.signal_clicked().connect(
        [this]()
        { on_mk_dir_btn(); }
    );

    mk_file_btn.signal_clicked().connect(
        [this]()
        { on_mk_file_btn(); }
    );

    cancel_btn.signal_clicked().connect(
        [this]()
        { on_cancel_btn(); }
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

FileExplorerMakeFileDir::~FileExplorerMakeFileDir()
{
    std::cout << "~FileExplorerMakeFileDir()" << std::endl;
    destroyer.run();
}

void FileExplorerMakeFileDir::show()
{
    win.present();
}

void FileExplorerMakeFileDir::destroy()
{
    destroyer.run();
}

void FileExplorerMakeFileDir::on_destroy_sig()
{
    std::cout << "FileExplorerMakeFileDir sig destroy" << std::endl;
    destroyer.run();
    std::cout << "FileExplorerMakeFileDir sig destroy exit" << std::endl;
}

bool FileExplorerMakeFileDir::on_signal_close_request()
{
    std::cout << "FileExplorerMakeFileDir::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

int FileExplorerMakeFileDir::common_func(bool file)
{
    // todo: maybe additional name_from_user cleanups needed
    // (space strippings/trimmings?)

    // todo: show messages if error?

    auto x = expl.lock();
    if (!x)
    {
        return 10;
    }

    auto new_name = subdir / std::filesystem::path(name_ent.get_text());

    auto ret = x->touchFileOrMkDirRelToProject(new_name, file);

    return ret;
}

void FileExplorerMakeFileDir::on_mk_dir_btn()
{
    auto x = expl.lock();
    if (!x)
    {
        return;
    }
    common_func(false);
    x->fileListRefresh();
    destroy();
}

void FileExplorerMakeFileDir::on_mk_file_btn()
{
    auto x = expl.lock();
    if (!x)
    {
        return;
    }
    common_func(true);
    x->fileListRefresh();
    destroy();
}

void FileExplorerMakeFileDir::on_cancel_btn()
{
    destroy();
}

} // namespace wayround_i2p::ccedit
