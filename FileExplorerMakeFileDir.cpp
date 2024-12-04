#include "FileExplorerMakeFileDir.hpp"

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

    win.set_child(main_box);

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

    win.signal_destroy().connect(
        sigc::mem_fun(
            *this,
            &FileExplorerMakeFileDir::on_destroy_sig
        )
    );
}

FileExplorerMakeFileDir::~FileExplorerMakeFileDir()
{
    std::cout << "~FileExplorerMakeFileDir()" << std::endl;
    destroyer.run();
}

void FileExplorerMakeFileDir::show()
{
    win.show();
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
    destroy();
}

void FileExplorerMakeFileDir::on_mk_file_btn()
{
    common_func(true);
    expl->fileListRefresh();
    destroy();
}

void FileExplorerMakeFileDir::on_cancel_btn()
{
    destroy();
}

} // namespace wayround_i2p::ccedit
