#ifndef WAYROUND_I2P_20241223_224139_496973
#define WAYROUND_I2P_20241223_224139_496973

#include <iostream>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "ProjectMenu.hpp"

#include "FileExplorerTables.hpp"

#include "forward_declarations.hpp"
#include "signal_sigc_compat.hpp"
#include "utils.hpp"

namespace wayround_i2p::ccedit
{

class FileExplorer
{
  public:
    static FileExplorer_shared create(
        ProjectCtl_shared project_ctl
    );

    ~FileExplorer();

  protected:
    FileExplorer(
        ProjectCtl_shared project_ctl
    );

  public:
    void show();
    void destroy();

    Gtk::Window &getWindowRef();
    Gtk::Window *getWindowPtr();

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
    FileExplorer_shared own_ptr;

    RunOnce destroyer;

    ProjectCtl_shared project_ctl;

    ProjectMenuGenerator wmg;

    Gtk::ApplicationWindow win;

    Gtk::Box main_box;

    Gtk::Box path_box;

    Gtk::Box    fb1;
    Gtk::Button reset_view_btn;
    Gtk::Button go_root_btn;
    Gtk::Button refresh_btn;
    // Gtk::Separator sep1;
    Gtk::Box    fb2;
    Gtk::Button filelauncher_dir_btn;
    Gtk::Button find_file_btn;
    // Gtk::Separator sep2;
    Gtk::Box    fb3;
    Gtk::Button make_file_or_directory_btn;
    // Gtk::Button rename_file_or_directory_btn;
    // Gtk::Button remove_file_or_directory_btn;
    // Gtk::Separator sep3;

    Gtk::MenuButton show_windows_btn;

    Gtk::Entry path_entry;

    Gtk::Paned lists_box;

    Gtk::ScrolledWindow dir_tree_sw;
    Gtk::ListView       dir_tree_view;

    Gtk::ScrolledWindow file_list_sw;
    Gtk::GridView       file_list_view;

    // main functions

    void setup_actions();
    void setup_main_menu();
    void setup_hotkeys();
    void setupDirTreeView();
    void setupFileListView();

    void updateTitle();

    int navigateToRoot();

    std::tuple<Glib::RefPtr<Gio::ListModel>, int>
        dirTreeGenDirListStore(std::filesystem::path subpath);

    // signal handlers

    SlotSigCCompat<void()> on_project_rename_slot;

    void on_project_rename();

    void on_dir_tree_view_activate(guint);
    void on_file_list_view_activate(guint);

    void on_reset_view_btn();
    void on_go_root_btn();
    void on_refresh_btn();

    void on_filelauncher_dir_btn();
    void on_find_file_btn();

    void on_make_file_or_directory_btn();

    // void on_rename_file_or_directory_btn();

    void on_destroy_sig();
    bool on_signal_close_request();

    // todo: is it really have to be as class member?
    Glib::RefPtr<Gtk::FileDialog> select_file_dialog;

    // working variables

    std::filesystem::path opened_subdir;
};

} // namespace wayround_i2p::ccedit

#endif
