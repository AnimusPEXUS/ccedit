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
    static ProjectCtlWin_shared create(ProjectCtl_shared project_ctl);
    ~ProjectCtlWin();

  protected:
    ProjectCtlWin(ProjectCtl_shared project_ctl);

  public:
    void updateTitle();

    void show();
    void destroy();

  private:
    RunOnce destroyer;

    ProjectCtlWin_shared own_ptr;

    ProjectCtl_shared project_ctl;
    Controller_shared controller;

    Gtk::Window win;

    Gtk::Box main_box;

    Gtk::Box b_box;

    Gtk::Button show_file_explorer_btn;
    Gtk::Button quit_project_btn;

    Gtk::Paned ws_ed_paned;

    // ------- work subjects -------

    Gtk::Box ws_main_box;

    Gtk::Box ws_tools_box;

    Gtk::ScrolledWindow               ws_view_sw;
    Gtk::ColumnView                   ws_view;
    Glib::RefPtr<Gtk::MultiSelection> ws_view_sel;

    void ws_add_columns();

    void ws_table_cell_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item,
        Gtk::Align                         halign
    );
    void ws_table_subject_cell_bind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    );

    // ------- editors -------

    Gtk::Box eds_main_box;

    Gtk::Box eds_tools_box;

    Gtk::ScrolledWindow               eds_view_sw;
    Gtk::ColumnView                   eds_view;
    Glib::RefPtr<Gtk::MultiSelection> eds_view_sel;

    void eds_add_columns();

    void eds_table_cell_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item,
        Gtk::Align                         halign
    );
    void eds_table_subject_cell_bind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    );

    // -------------------------

    void on_show_file_explorer_btn();
    void on_show_new_worksubject_list_btn();
    void on_show_new_editor_list_btn();

    void on_quit_project_btn();

    void on_hide_sig();
    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
