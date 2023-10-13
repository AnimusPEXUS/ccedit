#ifndef FILE_EXPLORER_HPP
#define FILE_EXPLORER_HPP

#include <iostream>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class FileExplorerWidget;

    class ProjectCtl;

    class FileExplorer : public Gtk::Window
    {
      public:
        FileExplorer(
            std::shared_ptr<ProjectCtl> proj_ctl
        );

        ~FileExplorer();

      private:
        std::shared_ptr<ProjectCtl> proj_ctl;

        Gtk::Box main_box;

        Gtk::Box path_box;

        Gtk::Button go_home_btn;
        Gtk::Button exit_folder_btn;
        Gtk::Button refresh_btn;
        Gtk::Entry  path_entry;

        Gtk::Paned lists_box;

        Gtk::ScrolledWindow dir_tree_sw;
        Gtk::ListView       dir_tree_view;

        Gtk::ScrolledWindow file_list_sw;
        Gtk::GridView       file_list_view;

        Gtk::Box    temp_file_selector_box;
        Gtk::Button temp_file_selector_btn;
        Gtk::Label  selected_file_lbl;
        Gtk::Button temp_file_open_btn;

        void setupDirTreeView();
        void setupFileListView();

        void updateTitle();

        std::tuple<std::filesystem::path, int> getProjectPath();

        int navigateToRoot();

        int fileListNavigateTo(std::filesystem::path subpath);

        std::tuple<Glib::RefPtr<Gio::ListModel>, int>
            dirTreeGenDirListStore(std::filesystem::path subpath);

        void on_project_rename();

        void on_destroy_sig();

        void on_temp_file_selector_btn();
        void on_temp_file_open_btn();

        void on_dir_tree_view_activate(guint);

        void on_refresh_btn();

        Glib::RefPtr<Gtk::FileDialog> select_file_dialog;

        std::filesystem::path opened_subdir;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
