#ifndef WAYROUND_I2P_20241204_051018_587907
#define WAYROUND_I2P_20241204_051018_587907

#include <filesystem>
#include <iostream>
#include <memory>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "ProjectMenu.hpp"

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

    ProjectMenuGenerator wmg;

    Gtk::Window win;

    Gtk::Box main_box;

    Gtk::Box b_box;

    Gtk::MenuButton show_windows_btn;
    Gtk::Button     rotate_paned_btn;

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

    void on_rotate_paned_btn();

    void on_hide_sig();
    void on_destroy_sig();
    bool on_signal_close_request();
};

} // namespace wayround_i2p::ccedit

#endif
