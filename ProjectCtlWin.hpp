#ifndef WAYROUND_I2P_20241106_133730_892839
#define WAYROUND_I2P_20241106_133730_892839

#include <filesystem>
#include <iostream>
#include <memory>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class ProjectCtlWin
{
  public:
    static ProjectCtlWin_shared create(ProjectCtl_shared proj_ctl);
    ~ProjectCtlWin();

  protected:
    ProjectCtlWin(ProjectCtl_shared proj_ctl);

  public:
    void updateTitle();

    void show();
    void destroy();

  private:
    runOnce destroyer;

    ProjectCtlWin_shared own_ptr;

    ProjectCtl_shared proj_ctl;
    Controller_shared controller;

    Gtk::Window win;

    Gtk::Box main_box;

    Gtk::Button show_file_explorer_btn;
    Gtk::Button show_new_worksubject_list_btn;
    Gtk::Button show_new_editor_list_btn;

    void on_show_file_explorer_btn();
    void on_show_new_worksubject_list_btn();
    void on_show_new_editor_list_btn();

    void on_hide_sig();
    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
