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

    Controller_shared getController();

    void show();
    void destroy();

    Gtk::Window *getWindowPtr();
    Gtk::Window &getWindowRef();

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

    // todo: better names for buttons

    Gtk::Button    btn_add_proj;
    Gtk::Button    btn_rm_proj;
    Gtk::Button    btn_edit_proj;
    Gtk::Separator sep0;
    Gtk::Button    btn_open_proj;
    Gtk::Separator sep1;
    Gtk::Button    btn_open_global;

    Gtk::Separator sep2;
    Gtk::Button    btn_modules_info_print;
    Gtk::Button    btn_save_cfg;

    Gtk::Separator sep3;
    Gtk::Button    btn_quit;

    void add_columns();

    void table_cell_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item,
        Gtk::Align                         halign
    );
    void table_name_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void table_name_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void table_path_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void table_path_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void on_btn_add_click();
    void on_btn_rm_click();
    void on_btn_edit_click();
    void on_btn_open_click();
    void on_btn_open_global_click();

    void on_btn_modules_info_print();
    void on_btn_save_cfg_click();

    void on_btn_quit_click();

    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
