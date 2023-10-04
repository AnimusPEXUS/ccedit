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
        // Glib::RefPtr<Gtk::SingleSelection> dir_tree_view_selection;

        Gtk::ScrolledWindow file_list_sw;
        Gtk::ListView       file_list_view;

        Gtk::Box    temp_file_selector_box;
        Gtk::Button temp_file_selector_btn;
        Gtk::Label  selected_file_lbl;
        Gtk::Button temp_file_open_btn;

        void on_project_rename();

        void updateTitle();

        void on_destroy_sig();

        void on_temp_file_selector_btn();
        void on_temp_file_open_btn();

        void on_refresh_btn();

        void on_temp_file_selector_btn_finish(std::shared_ptr<Gio::AsyncResult> res);

        Glib::RefPtr<Gtk::FileDialog> select_file_dialog;

        std::tuple<std::filesystem::path, int> getProjectPath();
        std::filesystem::path                  opened_subdir;

        std::tuple<Glib::RefPtr<Gio::ListModel>, int>
            dirTreeGenRootListStore(std::filesystem::path subpath);

        void setupDirTreeView();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
