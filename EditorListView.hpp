#ifndef WAYROUND_I2P_20241106_133730_893487
#define WAYROUND_I2P_20241106_133730_893487

#include <format>
#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "utils.hpp"

namespace wayround_i2p::ccedit
{

class EditorListView : public Gtk::Window
{
  public:
    static EditorListView_shared create(ProjectCtl_shared project_ctl);

    ~EditorListView();

  protected:
    EditorListView(ProjectCtl_shared project_ctl);

  private:
    EditorListView_shared own_ptr;
    RunOnce               destroyer;

    ProjectCtl_shared project_ctl;

    Gtk::Box main_box;

    Gtk::Box tools_box;

    Gtk::ScrolledWindow               eds_view_sw;
    Gtk::ColumnView                   eds_view;
    Glib::RefPtr<Gtk::MultiSelection> eds_view_sel;

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
