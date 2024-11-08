#ifndef WAYROUND_I2P_20241106_133730_895599
#define WAYROUND_I2P_20241106_133730_895599

#include <iostream>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class FileExplorer : public Gtk::Window
{
  public:
    static FileExplorer_shared create(
        ProjectCtl_shared proj_ctl
    );

    ~FileExplorer();

  protected:
    FileExplorer(
        ProjectCtl_shared proj_ctl
    );

  public:
    int touchFileOrMkDirRelToProject(
        std::filesystem::path subpath,
        bool                  file
    );

    int touchFileOrMkDirRelToCurrent(
        std::filesystem::path subpath,
        bool                  file
    );

    int touchFileOrMkDir(
        std::filesystem::path subpath,
        bool                  file,
        bool                  rel_to_current
    );

    int dirTreeNavigateTo(std::filesystem::path subpath);

    int fileListNavigateTo(std::filesystem::path subpath);
    int fileListRefresh();

  private:
    ProjectCtl_shared proj_ctl;

    FileExplorer_shared own_ptr;

    // widgets
    Gtk::Box main_box;

    Gtk::Box path_box;

    // Gtk::FlowBox   fb1;
    Gtk::Button    reset_view_btn;
    Gtk::Button    go_root_btn;
    Gtk::Button    refresh_btn;
    Gtk::Separator sep1;
    // Gtk::FlowBox   fb2;
    Gtk::Button    filelauncher_dir_btn;
    Gtk::Button    find_file_btn;
    Gtk::Separator sep2;
    // Gtk::FlowBox   fb3;
    Gtk::Button    make_file_or_directory_btn;
    Gtk::Button    rename_file_or_directory_btn;
    Gtk::Button    remove_file_or_directory_btn;
    Gtk::Separator sep3;
    Gtk::Entry     path_entry;

    Gtk::Paned lists_box;

    Gtk::ScrolledWindow dir_tree_sw;
    Gtk::ListView       dir_tree_view;

    Gtk::ScrolledWindow file_list_sw;
    Gtk::GridView       file_list_view;

    // main functions

    void setupDirTreeView();
    void setupFileListView();

    void updateTitle();

    int navigateToRoot();

    std::tuple<Glib::RefPtr<Gio::ListModel>, int>
        dirTreeGenDirListStore(std::filesystem::path subpath);

    // signal handlers

    void on_project_rename();

    void on_dir_tree_view_activate(guint);
    void on_file_list_view_activate(guint);

    void on_reset_view_btn();
    void on_go_root_btn();
    void on_refresh_btn();

    void on_filelauncher_dir_btn();
    void on_find_file_btn();

    void on_make_file_or_directory_btn();
    void on_rename_file_or_directory_btn();

    void on_destroy_sig();

    Glib::RefPtr<Gtk::FileDialog> select_file_dialog;

    // working variables

    std::filesystem::path opened_subdir;
};

class FileExplorerMakeFileDir : public Gtk::Window
{
  public:
    static FileExplorerMakeFileDir_shared create(
        FileExplorer_shared   expl,
        std::filesystem::path subdir
    );

    ~FileExplorerMakeFileDir();

  protected:
    FileExplorerMakeFileDir(
        FileExplorer_shared   expl,
        std::filesystem::path subdir
    );

  private:
    FileExplorer_shared   expl;
    std::filesystem::path subdir;

    FileExplorerMakeFileDir_shared own_ptr;

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

class FileExplorerRenameFileDir : public Gtk::Window
{
  public:
};

} // namespace wayround_i2p::ccedit

#endif
