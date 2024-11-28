#ifndef WAYROUND_I2P_20241125_061830_595300
#define WAYROUND_I2P_20241125_061830_595300

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "types.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubjectTableRowWidget : public Gtk::Box
{
  public:
    WorkSubjectTableRowWidget(
        const Glib::RefPtr<Gtk::ListItem>                          &list_item,
        std::function<void(TableItemTplP<WorkSubject_shared> item)> go_action
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
        const Glib::RefPtr<Gtk::ListItem>                                 &list_item,
        std::function<void(TableItemTplP<CodeEditorAbstract_shared> item)> go_action
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
