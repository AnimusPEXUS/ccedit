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
        static std::shared_ptr<FileExplorer> create(
            std::shared_ptr<ProjectCtl> proj_ctl
        );

        ~FileExplorer();

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

      protected:
        FileExplorer(
            std::shared_ptr<ProjectCtl> proj_ctl
        );

      private:
        std::shared_ptr<ProjectCtl> proj_ctl;

        std::shared_ptr<FileExplorer> own_ptr;

        // widgets
        Gtk::Box main_box;

        Gtk::Box path_box;

        Gtk::Button go_home_btn;
        Gtk::Button exit_folder_btn;
        Gtk::Button make_file_or_directory_btn;
        Gtk::Button rename_file_or_directory_btn;
        Gtk::Button refresh_btn;
        Gtk::Entry  path_entry;
        Gtk::Button filelauncher_dir_btn;

        Gtk::Paned lists_box;

        Gtk::ScrolledWindow dir_tree_sw;
        Gtk::ListView       dir_tree_view;

        Gtk::ScrolledWindow file_list_sw;
        Gtk::GridView       file_list_view;

        // main functions

        void setupDirTreeView();
        void setupFileListView();

        void updateTitle();

        std::tuple<std::filesystem::path, int> getProjectPath();

        int navigateToRoot();

        std::tuple<Glib::RefPtr<Gio::ListModel>, int>
            dirTreeGenDirListStore(std::filesystem::path subpath);

        // signal handlers

        void on_project_rename();

        void on_temp_file_selector_btn();
        void on_temp_file_open_btn();

        void on_dir_tree_view_activate(guint);
        void on_file_list_view_activate(guint);

        void on_refresh_btn();

        void on_filelauncher_dir_btn();

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
        static std::shared_ptr<FileExplorerMakeFileDir> create(
            std::shared_ptr<FileExplorer> expl
        );

        ~FileExplorerMakeFileDir();

      protected:
        FileExplorerMakeFileDir(std::shared_ptr<FileExplorer> expl);

      private:
        std::shared_ptr<FileExplorer> expl;

        std::shared_ptr<FileExplorerMakeFileDir> own_ptr;

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

        void on_destroy_sig();

        int common_func(bool file);
    };

    class FileExplorerRenameFileDir : public Gtk::Window
    {
      public:
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
