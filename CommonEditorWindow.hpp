#ifndef WAYROUND_I2P_20241204_051018_589125
#define WAYROUND_I2P_20241204_051018_589125

#include <memory>

#include <gtkmm.h>

#include "ProjectMenu.hpp"

#include "forward_declarations.hpp"

#include "modularity.hpp"
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

struct CommonEditorWindowParams
{
    std::function<void(CommonEditorWindow *ed_win)> menu_maker_cb;
    std::function<void(CommonEditorWindow *ed_win)> actions_maker_cb;
    std::function<void(CommonEditorWindow *ed_win)> hotkeys_maker_cb;
};

class CommonEditorWindow : public CodeEditorAbstract
{
  public:
    static CommonEditorWindow_shared create(
        ProjectCtl_shared               project_ctl,
        WorkSubject_shared              subject,
        const CommonEditorWindowParams &params
    );

  protected:
    CommonEditorWindow(
        ProjectCtl_shared               project_ctl,
        WorkSubject_shared              subject,
        const CommonEditorWindowParams &params
    );

  public:
    ~CommonEditorWindow();

    CodeEditorAbstract_shared getOwnPtr() const;

    ProjectCtl_shared getProjectCtl() const;

    WorkSubject_shared getWorkSubject() const;
    bool               workSubjectIs(WorkSubject_shared) const;

    // todo: this 'todo:' should be ok
    // ⬇️ {CodeEditorAbstract overrides} ⬇️
    // todo: looks like unicode causes incorrect character counting in file
    //       search. check: use FindFile to search for 'todo:' and try to 'Go'
    //       to this 'todo:' - if first 'todo:' not highlited - bug present

    void show();
    void destroy();

    // void setTransientWindow(Gtk::Window &win) override;

    Gtk::Window *getWindowPtr();
    Gtk::Window &getWindowRef();

    Glib::RefPtr<Gio::Menu> getMenuModel() const;

    std::size_t getCursorOffsetPosition() const;
    void        setCursorOffsetPosition(std::size_t new_pos, bool scroll = true);
    std::size_t getCurrentLine() const;
    void        setCurrentLine(std::size_t line, bool scroll = true);
    void        selectSlice(std::size_t start, std::size_t end);
    void        unselect();
    std::string getText() const;

    // ⬆️ {CodeEditorAbstract overrides} ⬆️

    void updateTitle(); // todo: make it private?

  private:
    CommonEditorWindow_shared       own_ptr;
    const CommonEditorWindowParams &params;

    // std::function<void()> callback_on_destroy;

    ProjectMenuGenerator wmg;

    ProjectCtl_shared  project_ctl;
    WorkSubject_shared subject;
    CodeEditorMod     *mod;

    RunOnce destroyer;

    Gtk::ApplicationWindow win;

    Gtk::Box            main_box;
    Gtk::Box            text_view_box_upper;
    Gtk::Box            text_view_box;
    Gtk::DrawingArea    linum_area;
    Gtk::ScrolledWindow text_view_sw;
    Gtk::TextView       text_view;

    void make_menubar();
    void make_actions();
    void make_hotkeys();

    Gtk::PopoverMenuBar menu_bar;

    Glib::RefPtr<Gio::Menu> menu_model;

    void action_work_subject_reload();
    void action_work_subject_save();
    void action_work_subject_save_as();
    void action_work_subject_close();

    void action_search_show_window();

    void action_windows_prev_window();
    void action_windows_next_window();
    void action_windows_duplicate_window();
    void action_windows_close_window();
    void action_windows_show_project_mgr();
    void action_windows_show_project_ctl();
    void action_windows_create_new_explorer();

    CommonEditorWindowStateStorage saved_editor_state;
    void                           saveState();
    void                           restoreState();

    void redraw_linum(
        const Cairo::RefPtr<Cairo::Context> &cont,
        int                                  width,
        int                                  height
    );

    void force_redraw_linum();

    void on_destroy_sig();
    bool on_signal_close_request();
};

} // namespace wayround_i2p::ccedit

#endif
