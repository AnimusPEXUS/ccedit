#ifndef FILE_EXPLORER_WIDGET_HPP
#define FILE_EXPLORER_WIDGET_HPP

#include <iostream>

#include <gtkmm.h>

// #include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class ProjectCtl;

    class FileExplorerWidget : public Gtk::Box
    {
      public:
        Gtk::Box path_box;

        Gtk::Button go_home_btn;
        Gtk::Button exit_folder_btn;
        Gtk::Button refresh_btn;
        Gtk::Entry  path_entry;

        Gtk::Paned lists_box;

        Gtk::ScrolledWindow dir_tree_sw;
        Gtk::ColumnView     dir_tree_view;

        Gtk::ScrolledWindow file_list_sw;
        Gtk::ColumnView     file_list_view;

        Gtk::Label selected_file_lbl;

        Gtk::Widget getRootWidget();

        FileExplorerWidget(
            std::shared_ptr<ProjectCtl> proj_ctl
        );

        ~FileExplorerWidget();

      private:
        std::shared_ptr<ProjectCtl> proj_ctl;

        void on_destroy_sig();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
