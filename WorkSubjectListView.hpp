#ifndef WAYROUND_I2P_20241106_133730_901154
#define WAYROUND_I2P_20241106_133730_901154

#include <format>
#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubjectListView : public Gtk::Window
{
  public:
    WorkSubjectListView(ProjectCtl_shared project_ctl);
    ~WorkSubjectListView();

  private:
    ProjectCtl_shared project_ctl;

    Gtk::Box main_box;

    Gtk::Box tools_box;

    Gtk::ScrolledWindow               ws_view_sw;
    Gtk::ColumnView                   ws_view;
    Glib::RefPtr<Gtk::MultiSelection> ws_view_sel;

    void add_columns();

    void table_cell_setup(
        const Glib::RefPtr<Gtk::ListItem> &list_item,
        Gtk::Align                         halign
    );
    void table_subject_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void updateTitle();

    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
