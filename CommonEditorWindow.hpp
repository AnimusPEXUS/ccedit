#ifndef WAYROUND_I2P_20241106_133730_894827
#define WAYROUND_I2P_20241106_133730_894827

#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "modularity.hpp"
#include "tables.hpp"
#include "utils.hpp"

#include "CodeEditorAbstract.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p::ccedit
{

struct CommonEditorWindowStateStorage
{
    unsigned int cur_pos_iter_offset = 0;
    double       scroll_adj          = 0;
};

class CommonEditorWindow : public CodeEditorAbstract
{
  public:
    CommonEditorWindow(
        ProjectCtl_shared     project_ctl,
        WorkSubject_shared    subject,
        std::function<void()> callback_on_destroy
    );
    ~CommonEditorWindow();

    // todo: this 'todo:' should be ok
    // ⬇️ {CodeEditorAbstract overrides} ⬇️
    // todo: looks like unicode causes incorrect character counting in file
    //       search. check: use FindFile to search for 'todo:' and try to 'Go'
    //       to this 'todo:' - if first 'todo:' not highlited - bug present

    void show() override;
    void present() override;

    void destroy() override;

    // void setTransientWindow(Gtk::Window &win) override;

    Gtk::Window *getWindowPtr() override;
    Gtk::Window &getWindowRef() override;

    std::size_t getCursorOffsetPosition() override;
    void        setCursorOffsetPosition(std::size_t new_pos, bool scroll = true) override;
    std::size_t getCurrentLine() override;
    void        setCurrentLine(std::size_t line, bool scroll = true) override;
    void        selectSlice(std::size_t start, std::size_t end) override;
    void        unselect() override;
    std::string getText() override;

    // ⬆️ {CodeEditorAbstract overrides} ⬆️

    void updateTitle(); // todo: make it private?

    void setOutlineContents(
        std::vector<std::tuple<std::size_t, std::string>> val
    );
    void setOutlineCurrentLine(std::size_t val);

    virtual std::vector<std::tuple<std::size_t, std::string>>
        genOutlineContents() = 0;

  private:
    std::function<void()> callback_on_destroy;

    ProjectCtl_shared  project_ctl;
    WorkSubject_shared subject;
    CodeEditorMod     *mod;

    RunOnce destroyer;

    Gtk::ApplicationWindow win;

    Gtk::Box                                      main_box;
    Gtk::Paned                                    paned;
    Gtk::Box                                      text_view_box_upper;
    Gtk::Box                                      text_view_box;
    Gtk::DrawingArea                              linum_area;
    Gtk::ScrolledWindow                           text_view_sw;
    Gtk::TextView                                 text_view;
    Gtk::Box                                      outline_box;
    Gtk::ScrolledWindow                           outline_view_sw;
    Gtk::ColumnView                               outline_view;
    Gtk::Button                                   outline_view_refresh_btn;
    Glib::RefPtr<Gtk::SingleSelection>            outline_view_selection;
    Glib::RefPtr<Gio::ListStore<OutlineTableRow>> outline_list_store;

    void setup_outline_columns();

    void make_menubar();
    void make_actions();
    void make_hotkeys();

    Gtk::PopoverMenuBar menu_bar;

    Glib::RefPtr<Gio::Menu> menu_model;

    Glib::RefPtr<Gio::Menu>     mm_buffer;
    Glib::RefPtr<Gio::MenuItem> mm_buffer_reload;
    Glib::RefPtr<Gio::MenuItem> mm_buffer_save;
    Glib::RefPtr<Gio::MenuItem> mm_buffer_save_as;

    Glib::RefPtr<Gio::Menu>     mm_search;
    Glib::RefPtr<Gio::MenuItem> mm_search_search_window;

    void action_buffer_reload();
    void action_buffer_save();
    void action_buffer_save_as();

    void action_search_show_window();

    CommonEditorWindowStateStorage saved_editor_state;
    void                           saveState();
    void                           restoreState();

    void redraw_linum(
        const Cairo::RefPtr<Cairo::Context> &cont,
        int                                  width,
        int                                  height
    );

    void force_redraw_linum();

    void on_outline_refresh_btn();
    void on_outline_activate(guint val);

    void on_destroy_sig();
    bool on_signal_close_request();

  protected:
    Glib::RefPtr<Gio::Menu> getMenuModel();

  private:
    virtual void make_special_menu()    = 0;
    virtual void make_special_actions() = 0;
    virtual void make_special_hotkeys() = 0;
};

} // namespace wayround_i2p::ccedit

#endif
