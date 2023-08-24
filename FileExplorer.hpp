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

    class FileExplorer : public Gtk::Window
    {
      public:
        FileExplorer(
            std::shared_ptr<ProjectCtl> proj_ctl
        );

        ~FileExplorer();

      private:
        std::shared_ptr<ProjectCtl> proj_ctl;

        // few have to be pointer, because FileExplorerWidget
        // incomplete at this point.
        // few can't be simple pointer, because othervise
        // it's deletion causes segfault
        std::shared_ptr<FileExplorerWidget> few;

        void cleanup_few();

        void on_destroy_sig();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
