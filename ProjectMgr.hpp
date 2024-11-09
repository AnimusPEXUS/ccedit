#ifndef WAYROUND_I2P_20241106_133730_883847
#define WAYROUND_I2P_20241106_133730_883847

#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "utils.hpp"

namespace wayround_i2p::ccedit
{

class ProjectMgr
{
  public:
    static ProjectMgr_shared create(Controller_shared controller);

    void show();
    void destroy();

  protected:
    ProjectMgr(Controller_shared controller);

  private:
    ProjectMgr_shared own_ptr;

    RunOnce destroyer;

    // controller should not possibly be destroyed before any other window
    // of ccedit, so it is always shared inside of ccedit windows
    Controller_shared controller;

    Gtk::Window win;

    Gtk::ScrolledWindow project_list_sw;
    Gtk::ColumnView     project_list_view;

    Glib::RefPtr<Gtk::MultiSelection>
        project_list_view_selection;

    Gtk::Box main_box;
    Gtk::Box button_box;
    Gtk::Box button_box_sec;

    Gtk::Button    add_proj;
    Gtk::Button    rm_proj;
    Gtk::Button    edit_proj;
    Gtk::Separator sep0;
    Gtk::Button    open_proj;
    Gtk::Separator sep1;
    Gtk::Button    open_global;

    Gtk::Button modules_info_print;
    Gtk::Button save_cfg;

    void add_columns();

    void table_cell_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item,
        Gtk::Align                         halign
    );
    void table_name_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void table_name_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void table_path_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void table_path_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void on_add_click();
    void on_rm_click();
    void on_edit_click();
    void on_open_click();
    void on_open_global_click();

    void on_modules_info_print();
    void on_save_cfg();

    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
