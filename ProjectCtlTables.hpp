#ifndef WAYROUND_I2P_20241204_051018_596394
#define WAYROUND_I2P_20241204_051018_596394

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "types.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubjectTableRowWidget : public Gtk::Box
{
  public:
    WorkSubjectTableRowWidget();
    ~WorkSubjectTableRowWidget();

    void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  private:
    WorkSubject_shared bond_ws;

    Gtk::Box button_box;

    Gtk::Button btn_close;
    Gtk::Button btn_open_editor;

    Gtk::Label edited_indicator;
    Gtk::Label path;

    void update_labels();

    void on_btn_close();
    void on_btn_open_editor();

    void on_ws_changed();
};

class CodeEditorTableRowWidget : public Gtk::Box
{
  public:
    CodeEditorTableRowWidget();
    ~CodeEditorTableRowWidget();

    void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  private:
    CodeEditorAbstract_shared bond_ed;

    Gtk::Grid   btn_grid;
    Gtk::Button btn_close;
    Gtk::Button btn_show;
    Gtk::Button btn_up;
    Gtk::Button btn_down;
    Gtk::Label  path;

    void on_btn_show();
    void on_btn_close();
    void on_btn_up();
    void on_btn_down();
};

} // namespace wayround_i2p::ccedit

#endif
