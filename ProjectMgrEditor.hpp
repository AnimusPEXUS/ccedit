#ifndef PROJECT_MGR_EDITOR_HPP
#define PROJECT_MGR_EDITOR_HPP

#include <gtkmm.h>

#include "Controller.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class ProjectMgrEditor : public Gtk::Window
    {

      public:
        ProjectMgrEditor(
            std::shared_ptr<Controller> controller,
            std::string                 proj_name,
            std::string                 proj_path
        );

      private:
        std::shared_ptr<Controller> controller;

        std::string proj_name_orig;
        std::string proj_path_orig;

        Gtk::Box main_box;

        Gtk::Grid main_grid;

        Gtk::Label name_label;
        Gtk::Label path_label;

        Gtk::Entry project_name;
        Gtk::Entry project_path;

        Gtk::Button btn_browse;

        Gtk::Button btn_ok;
        Gtk::Button btn_cancel;

        Gtk::Box button_box;

        void on_ok_click();
        void on_cancel_click();
        void on_browse_click();

        void on_browse_click_finish(std::shared_ptr<Gio::AsyncResult> res);

        Glib::RefPtr<Gtk::FileDialog> select_dir_dialog;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
