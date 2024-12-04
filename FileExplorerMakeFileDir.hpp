#ifndef WAYROUND_I2P_20241204_051018_574830
#define WAYROUND_I2P_20241204_051018_574830

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class FileExplorerMakeFileDir
{
  public:
    static FileExplorerMakeFileDir_shared create(
        FileExplorer_shared   expl,
        std::filesystem::path subdir
    );

    ~FileExplorerMakeFileDir();

    void show();
    void destroy();

  protected:
    FileExplorerMakeFileDir(
        FileExplorer_shared   expl,
        std::filesystem::path subdir
    );

  private:
    FileExplorerMakeFileDir_shared own_ptr;

    RunOnce destroyer;

    FileExplorer_shared   expl;
    std::filesystem::path subdir;

    Gtk::Window win;

    Gtk::Box  main_box;
    Gtk::Grid main_grid;

    Gtk::Label placement_lbl;
    Gtk::Label placement_lbl2;

    Gtk::Label type_name_lbl;
    Gtk::Entry name_ent;

    Gtk::Box    btn_box;
    Gtk::Button mk_dir_btn;
    Gtk::Button mk_file_btn;
    Gtk::Button cancel_btn;

    void on_mk_dir_btn();
    void on_mk_file_btn();
    void on_cancel_btn();

    void on_destroy_sig();

    int common_func(bool file);
};

} // namespace wayround_i2p::ccedit

#endif
