
#include <format>
#include <limits>

#include <gtkmm.h>

#include "CommonEditorWindow.hpp"

#include "Controller.hpp"
#include "FindText.hpp"

namespace wayround_i2p::ccedit
{

CommonEditorWindow_shared CommonEditorWindow::create(
    ProjectCtl_shared               project_ctl,
    WorkSubject_shared              subject,
    const CommonEditorWindowParams &params
)
{
    auto ret = CommonEditorWindow_shared(
        new CommonEditorWindow(
            project_ctl,
            subject,
            params
        )
    );
    ret->own_ptr = ret;

    project_ctl->registerEditor(ret);
    project_ctl->getController()->registerWindow(ret->getWindowPtr());

    return ret;
}

CommonEditorWindow::CommonEditorWindow(
    ProjectCtl_shared               project_ctl,
    WorkSubject_shared              subject,
    const CommonEditorWindowParams &params
) :
    destroyer(
        [this]()
        {
            std::cout << "CommonEditorWindow::destroyer.run()" << std::endl;
            this->project_ctl->unregisterEditor(
                this->own_ptr
            );
            destroy();
            win.destroy();
            own_ptr.reset();
        }
    ),
    wmg(project_ctl),
    // callback_on_destroy(callback_on_destroy),
    main_box(Gtk::Orientation::VERTICAL, 0),
    text_view_box_upper(Gtk::Orientation::VERTICAL, 0),
    text_view_box(Gtk::Orientation::HORIZONTAL, 0),
    params(params)
{
    this->project_ctl = project_ctl;
    this->subject     = subject;

    win.set_child(main_box);

    main_box.append(menu_bar);
    main_box.append(text_view_box_upper);

    text_view_box.set_vexpand(true);

    text_view_box_upper.append(text_view_box);

    text_view_box.append(linum_area);
    text_view_box.append(text_view_sw);

    text_view_sw.set_policy(
        Gtk::PolicyType::ALWAYS,
        Gtk::PolicyType::ALWAYS
    );

    linum_area.set_content_width(50);

    text_view_box.set_vexpand(true);
    text_view_box.set_hexpand(true);
    text_view_box.set_valign(Gtk::Align::FILL);
    text_view_box.set_halign(Gtk::Align::FILL);

    text_view_sw.set_vexpand(true);
    text_view_sw.set_hexpand(true);
    text_view_sw.set_valign(Gtk::Align::FILL);
    text_view_sw.set_halign(Gtk::Align::FILL);

    text_view_sw.set_overlay_scrolling(false);

    text_view_sw.set_kinetic_scrolling(false);

    text_view_sw.set_child(text_view);

    text_view.set_monospace(true);
    text_view.set_buffer(subject->getTextBuffer());

    make_menubar();
    make_actions();
    make_hotkeys();

    linum_area.set_draw_func(
        [this](
            const Cairo::RefPtr<Cairo::Context> &cont,
            int                                  width,
            int                                  height
        )
        {
            redraw_linum(
                cont,
                width,
                height
            );
        }
    );

    subject->signal_modified_changed().connect(
        [this]()
        { updateTitle(); }
    );

    subject->signal_editors_save_state().connect(
        [this]()
        { saveState(); }
    );

    subject->signal_editors_restore_state().connect(
        [this]()
        { restoreState(); }
    );

    project_ctl->signal_updated_name().connect(
        [this]()
        { updateTitle(); }
    );

    on_destroy_sig_slot = [this]()
    { on_destroy_sig_slot(); };

    win.signal_destroy().connect(on_destroy_sig_slot);

    win.signal_close_request().connect(
        [this]() -> bool
        { return on_signal_close_request(); },
        true
    );

    text_view_sw.get_vscrollbar()->get_adjustment()->signal_changed().connect(
        [this]()
        { force_redraw_linum(); }
    );

    text_view_sw.get_vscrollbar()->get_adjustment()->signal_value_changed().connect(
        [this]()
        { force_redraw_linum(); }
    );

    updateTitle();
}

CommonEditorWindow::~CommonEditorWindow()
{
    std::cout << "~CommonEditorWindow()" << std::endl;
    destroyer.run();
}

CodeEditorAbstract_shared CommonEditorWindow::getOwnPtr() const
{
    return own_ptr;
}

ProjectCtl_shared CommonEditorWindow::getProjectCtl() const
{
    return project_ctl;
}

void CommonEditorWindow::show()
{
    std::cout << "CommonEditorWindow::show()" << std::endl;
    win.present();
}

void CommonEditorWindow::destroy()
{
    std::cout << "CommonEditorWindow::destroy()" << std::endl;
    destroyer.run();
}

void CommonEditorWindow::on_destroy_sig()
{
    std::cout << "CommonEditorWindow::on_destroy_sig()" << std::endl;
    destroyer.run();
}

bool CommonEditorWindow::on_signal_close_request()
{
    std::cout << "CommonEditorWindow::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

void CommonEditorWindow::make_menubar()
{
    Glib::RefPtr<Gio::Menu> sect;

    auto mm_ws = Gio::Menu::create();

    sect = Gio::Menu::create();
    sect->append("(Re)load", "editor_window.work_subject_reload");
    sect->append("Save", "editor_window.work_subject_save");
    sect->append("Save as..", "editor_window.work_subject_save_as");
    mm_ws->append_section(sect);

    sect = Gio::Menu::create();
    sect->append("Close", "editor_window.work_subject_close");
    mm_ws->append_section(sect);

    auto mm_search = Gio::Menu::create();

    sect = Gio::Menu::create();

    sect->append("Search Window..", "editor_window.search_show_window");

    mm_search->append_section(sect);

    auto mm_windows = Gio::Menu::create();

    sect = Gio::Menu::create();
    sect->append("Previous", "editor_window.windows_prev_window");
    sect->append("Next", "editor_window.windows_next_window");
    mm_windows->append_section(sect);

    sect = Gio::Menu::create();
    sect->append(
        "Duplicate Current",
        "editor_window.windows_duplicate_window"
    );
    sect->append("Close Current", "editor_window.windows_close_window");
    mm_windows->append_section(sect);

    auto mm_project_mnu = wmg.createProjectMenu("editor_window");

    menu_model = Gio::Menu::create();
    menu_model->append_submenu("Work Subject", mm_ws);
    menu_model->append_submenu("Search", mm_search);
    menu_model->append_submenu("Editor Windows", mm_windows);
    menu_model->append_submenu("Project Menu", mm_project_mnu);

    menu_bar.set_menu_model(menu_model);

    if (params.menu_maker_cb)
    {
        params.menu_maker_cb(this);
    }
};

/*
void CommonEditorWindow::make_special_menu()
{
    // note: this is to be overriden by inheriting class
    return;
}
*/

void CommonEditorWindow::make_actions()
{
    auto action_group = Gio::SimpleActionGroup::create();

    action_group->add_action(
        "work_subject_reload",
        [this]()
        { action_work_subject_reload(); }
    );

    action_group->add_action(
        "work_subject_save",
        [this]()
        { action_work_subject_save(); }
    );

    action_group->add_action(
        "work_subject_save_as",
        [this]()
        { action_work_subject_save_as(); }
    );

    action_group->add_action(
        "work_subject_close",
        [this]()
        { action_work_subject_close(); }
    );

    action_group->add_action(
        "search_show_window",
        [this]()
        { action_search_show_window(); }
    );

    action_group->add_action(
        "windows_prev_window",
        [this]()
        { action_windows_prev_window(); }
    );

    action_group->add_action(
        "windows_next_window",
        [this]()
        { action_windows_next_window(); }
    );

    action_group->add_action(
        "windows_duplicate_window",
        [this]()
        { action_windows_duplicate_window(); }
    );

    action_group->add_action(
        "windows_close_window",
        [this]()
        { action_windows_close_window(); }
    );

    wmg.addActionsToActionGroup(action_group);

    win.insert_action_group("editor_window", action_group);

    if (params.actions_maker_cb)
    {
        params.actions_maker_cb(this);
    }
}

/*
void CommonEditorWindow::make_special_actions()
{
    // note: this is to be overriden by inheriting class
    return;
}
*/

void CommonEditorWindow::make_hotkeys()
{
    auto controller = Gtk::ShortcutController::create();
    controller->set_scope(Gtk::ShortcutScope::LOCAL);

    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_r,
            Gdk::ModifierType::CONTROL_MASK
        ),
        Gtk::NamedAction::create("editor_window.work_subject_reload")
    ));
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_s,
            Gdk::ModifierType::CONTROL_MASK
        ),
        Gtk::NamedAction::create("editor_window.work_subject_save")
    ));
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_s,
            Gdk::ModifierType::CONTROL_MASK
                | Gdk::ModifierType::SHIFT_MASK
        ),
        Gtk::NamedAction::create("editor_window.work_subject_save_as")
    ));
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_f,
            Gdk::ModifierType::CONTROL_MASK
        ),
        Gtk::NamedAction::create("editor_window.search_show_window")
    ));
    win.add_controller(controller);

    if (params.hotkeys_maker_cb)
    {
        params.hotkeys_maker_cb(this);
    }
}

WorkSubject_shared CommonEditorWindow::getWorkSubject() const
{
    return subject;
}

bool CommonEditorWindow::workSubjectIs(WorkSubject_shared subj) const
{
    if (!subj || !subject)
    {
        return false;
    }
    return subj == subject;
}

/*
void CommonEditorWindow::make_special_hotkeys()
{
    // note: this is to be overriden by inheriting class
    return;
}*/

Glib::RefPtr<Gio::Menu> CommonEditorWindow::getMenuModel() const
{
    return menu_model;
}

void CommonEditorWindow::updateTitle()
{
    std::string new_title;

    auto pth = subject->getPath();

    std::string mod_bullet("");
    if (subject->modified())
    {
        mod_bullet = "*";
    }

    std::string proj_name("(global)");
    if (!project_ctl->isGlobalProject())
    {
        proj_name = std::format(
            "(proj: {})",
            project_ctl->getProjectName()
        );
    }

    new_title = std::format(
        "{}{} {} - Code Editor",
        mod_bullet,
        (pth.filename()).string(),
        proj_name
    );

    win.set_title(new_title);
}

void CommonEditorWindow::saveState()
{
    // todo: maybe use get_cursor_locations() instead of directly using TextBuffer
    auto tb = subject->getTextBuffer();

    auto cur_pos             = tb->get_insert();
    auto cur_pos_iter        = tb->get_iter_at_mark(cur_pos);
    auto cur_pos_iter_offset = cur_pos_iter.get_offset();
    auto v_scrollbar         = text_view_sw.get_vscrollbar();
    auto adj                 = v_scrollbar->get_adjustment()->get_value();

    saved_editor_state.cur_pos_iter_offset = cur_pos_iter_offset;
    saved_editor_state.scroll_adj          = adj;
}

void CommonEditorWindow::restoreState()
{
    auto context = Glib::MainContext::get_default();

    context->signal_idle().connect_once(
        [this]()
        {
            auto tb = subject->getTextBuffer();

            auto new_iter = tb->get_iter_at_offset(
                saved_editor_state.cur_pos_iter_offset
            );
            tb->place_cursor(new_iter);

            auto v_scrollbar = this->text_view_sw.get_vscrollbar();
            v_scrollbar->get_adjustment()->set_value(saved_editor_state.scroll_adj);
        }
    );
}

void CommonEditorWindow::redraw_linum(
    const Cairo::RefPtr<Cairo::Context> &cont,
    int                                  width,
    int                                  height
)
{
    Gdk::Rectangle r;
    text_view.get_visible_rect(r);

    auto modifier = r.get_y();
    auto size     = r.get_height();

    unsigned int new_line     = 0;
    unsigned int current_line = 0;

    auto text_buff = text_view.get_buffer();

    for (ssize_t i = 0; i != size; i++)
    {
        Gtk::TextIter new_iter;
        text_view.get_iter_at_location(new_iter, 0, i + modifier);
        new_line = new_iter.get_line();
        if (new_line != current_line)
        {
            cont->move_to(5, i);
            cont->show_text(std::format("{}", new_line));
            current_line = new_line;
        }
    }
}

void CommonEditorWindow::force_redraw_linum()
{
    linum_area.queue_draw();
}

Gtk::Window *CommonEditorWindow::getWindowPtr()
{
    return &win;
}

Gtk::Window &CommonEditorWindow::getWindowRef()
{
    return win;
}

std::size_t CommonEditorWindow::getCursorOffsetPosition() const
{
    auto tb = subject->getTextBuffer();

    auto cur_pos      = tb->get_insert();
    auto cur_pos_iter = tb->get_iter_at_mark(cur_pos);
    auto offset       = cur_pos_iter.get_offset();
    return offset;
}

void CommonEditorWindow::setCursorOffsetPosition(
    std::size_t new_pos,
    bool        scroll
)
{
    auto tb = subject->getTextBuffer();

    auto itr = tb->get_iter_at_offset(new_pos);
    tb->place_cursor(itr);
    if (scroll)
    {
        text_view.scroll_to(itr, 0, 0.5, 0.5);
    }
}

std::size_t CommonEditorWindow::getCurrentLine() const
{
    auto tb = subject->getTextBuffer();

    auto cur_pos      = tb->get_insert();
    auto cur_pos_iter = tb->get_iter_at_mark(cur_pos);
    auto line         = cur_pos_iter.get_line();
    if (line != std::numeric_limits<typeof(line)>::max())
    {
        line += 1;
    }
    return line;
}

void CommonEditorWindow::setCurrentLine(std::size_t line, bool scroll)
{
    if (line != std::numeric_limits<typeof(line)>::min())
    {
        line -= 1;
    }

    std::cout << std::format("setCurrentLine({}, {})", line, scroll) << std::endl;

    auto tb = subject->getTextBuffer();

    auto line_iter = tb->get_iter_at_line(line);
    tb->place_cursor(line_iter);
    if (scroll)
    {
        text_view.scroll_to(line_iter, 0, 0.5, 0.5);
    }
}

void CommonEditorWindow::selectSlice(std::size_t start, std::size_t end)
{
    auto tb = subject->getTextBuffer();

    auto it1 = tb->get_iter_at_offset(start);
    auto it2 = tb->get_iter_at_offset(end);

    tb->select_range(it1, it2);
}

void CommonEditorWindow::unselect()
{
    auto tb = subject->getTextBuffer();
    tb->place_cursor(tb->get_iter_at_mark(tb->get_insert())); // is there better way to deselect?
}

std::string CommonEditorWindow::getText() const
{
    return subject->getText();
}

void CommonEditorWindow::action_work_subject_reload()
{
    std::cout << "reload" << std::endl;
    subject->reload();
}

void CommonEditorWindow::action_work_subject_save()
{
    std::cout << "save" << std::endl;
    subject->save();
}

void CommonEditorWindow::action_work_subject_save_as()
{
    std::cout << "save as" << std::endl;
    // todo: todo
}

void CommonEditorWindow::action_work_subject_close()
{
    subject->destroy();
}

void CommonEditorWindow::action_search_show_window()
{
    auto w = FindText::create(own_ptr);
    w->show();

    // todo: move registration inside of FindText constructor
    // project_ctl->getController()->registerWindow(w);
    // setTransientWindow(w);
    // w->set_destroy_with_parent(true);
}

void CommonEditorWindow::action_windows_prev_window()
{
    std::cout << "CommonEditorWindow::action_windows_prev_window" << std::endl;
    project_ctl->showPrevEditor(own_ptr);
}

void CommonEditorWindow::action_windows_next_window()
{
    std::cout << "CommonEditorWindow::action_windows_next_window" << std::endl;
    project_ctl->showNextEditor(own_ptr);
}

void CommonEditorWindow::action_windows_duplicate_window()
{
    project_ctl->workSubjectNewEditor(subject);
}

void CommonEditorWindow::action_windows_close_window()
{
    destroy();
}

} // namespace wayround_i2p::ccedit
