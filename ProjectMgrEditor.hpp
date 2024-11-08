#ifndef WAYROUND_I2P_20241106_133730_890322
#define WAYROUND_I2P_20241106_133730_890322

#include <iostream>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class ProjectMgrEditor
{
  public:
    static ProjectMgrEditor_shared create(
        Controller_shared     controller,
        std::string           proj_name,
        std::filesystem::path proj_path
    );

  protected:
    ProjectMgrEditor(
        Controller_shared     controller,
        std::string           proj_name,
        std::filesystem::path proj_path
    );

  public:
    ~ProjectMgrEditor();

  private:
    Controller_weak controller;

    ProjectMgrEditor_weak own_ptr;

    std::string           proj_name_orig;
    std::filesystem::path proj_path_orig;

    Gtk::Window win;

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

    // todo: rework this function
    // void on_browse_click_finish(std::shared_ptr<Gio::AsyncResult> res);

    void on_destroy_sig();

    Glib::RefPtr<Gtk::FileDialog> select_dir_dialog;
};

} // namespace wayround_i2p::ccedit

#endif
