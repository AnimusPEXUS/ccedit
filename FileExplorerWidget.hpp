#ifndef FILE_EXPLORER_WIDGET_HPP
#define FILE_EXPLORER_WIDGET_HPP

#include <iostream>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

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
        Gtk::Entry  path_entry;

        Gtk::ScrolledWindow file_list_sw;
        Gtk::ColumnView     file_list_view;

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
