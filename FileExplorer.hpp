#ifndef FILE_EXPLORER_HPP
#define FILE_EXPLORER_HPP

#include <iostream>

#include <gtkmm.h>

#include "FileExplorerWidget.hpp"
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

        // can't allocate FileExplorerWidget on stack
        // because it has circular dependency on ProjectCtl
        // and incomplete at this point
        FileExplorerWidget *few;

        void on_destroy_sig();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
