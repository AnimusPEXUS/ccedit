#ifndef PROJECT_CTL_HPP
#define PROJECT_CTL_HPP

#include <iostream>

#include <gtkmm.h>

#include "Controller.hpp"
#include "FileExplorer.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class Controller;

    class FileExplorer;

    class ProjectCtl : public Gtk::Window
    {
      public:
        ProjectCtl(std::shared_ptr<Controller> controller);

        ~ProjectCtl()
        {
            std::cout << "~ProjectCtl()" << std::endl;
        }

        std::shared_ptr<ProjectCtl> own_ptr;

        std::shared_ptr<Controller> getController();

        void registerFileExplorer(std::shared_ptr<FileExplorer> fe);
        void unregisterFileExplorer(FileExplorer *fe);
        void unregisterFileExplorer(std::shared_ptr<FileExplorer> fe);

      private:
        std::shared_ptr<Controller> controller;

        std::vector<std::shared_ptr<FileExplorer>> explorer_windows;

        Gtk::Button show_file_explorer_btn;

        void on_show_file_explorer_btn();

        void on_hide_sig();
        void on_destroy_sig();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
