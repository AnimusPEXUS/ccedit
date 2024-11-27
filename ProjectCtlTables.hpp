#ifndef WAYROUND_I2P_20241125_061830_595300
#define WAYROUND_I2P_20241125_061830_595300

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "types.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubjectTableRow;
using WorkSubjectTableRowP = Glib::RefPtr<WorkSubjectTableRow>;

class CodeEditorTableRow;
using CodeEditorTableRowP = Glib::RefPtr<CodeEditorTableRow>;

// todo: maybe! here and in other *Table.[hc]pp files
//       make a template to geterate those List Item object classes.
//       or, maybe, even better: use single real class and
//       use std::any for storing payload
class WorkSubjectTableRow : public Glib::Object
{
  public:
    WorkSubject_shared work_subj;

    static WorkSubjectTableRowP create();

    ~WorkSubjectTableRow();

  protected:
    WorkSubjectTableRow();
};

class CodeEditorTableRow : public Glib::Object
{
  public:
    CodeEditorAbstract_shared editor;

    static CodeEditorTableRowP create();

    ~CodeEditorTableRow();

  protected:
    CodeEditorTableRow();
};

class WorkSubjectTableRowWidget : public Gtk::Box
{
  public:
    WorkSubjectTableRowWidget(
        const Glib::RefPtr<Gtk::ListItem>             &list_item,
        std::function<void(WorkSubjectTableRowP item)> go_action
    );
    ~WorkSubjectTableRowWidget();

    void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  private:
    Gtk::Button btn_new_win;
    Gtk::Button btn_existing_or_new_win;
    Gtk::Button btn_close;
    Gtk::Label  path;

    void on_btn_new_win();
    void on_btn_existing_or_new_win();
    void on_btn_close();
};

class CodeEditorTableRowWidget : public Gtk::Box
{
  public:
    CodeEditorTableRowWidget(
        const Glib::RefPtr<Gtk::ListItem>             &list_item,
        std::function<void(WorkSubjectTableRowP item)> go_action
    );
    ~CodeEditorTableRowWidget();

    void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

  private:
    Gtk::Button btn_present;
    Gtk::Button btn_close;
    Gtk::Label  path;

    void on_btn_present();
    void on_btn_close();
};

} // namespace wayround_i2p::ccedit

#endif
