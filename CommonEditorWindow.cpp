
#include <format>
#include <limits>

#include <gtkmm.h>

#include "CommonEditorWindow.hpp"

#include "Controller.hpp"
#include "FindText.hpp"

namespace wayround_i2p::ccedit
{

CommonEditorWindow::CommonEditorWindow(
    ProjectCtl_shared     project_ctl,
    WorkSubject_shared    subject,
    std::function<void()> callback_on_destroy
) :
    destroyer(
        [this]()
        {
            std::cout << "CommonEditorWindow::destroyer.run()" << std::endl;
            if (this->callback_on_destroy)
            {
                this->callback_on_destroy();
            }
            win.destroy();
        }
    ),
    wmg(project_ctl),
    callback_on_destroy(callback_on_destroy),
    main_box(Gtk::Orientation::VERTICAL, 0),
    text_view_box_upper(Gtk::Orientation::VERTICAL, 0),
    text_view_box(Gtk::Orientation::HORIZONTAL, 0),
    outline_box(Gtk::Orientation::VERTICAL, 0)
{
    this->project_ctl = project_ctl;
    this->subject     = subject;

    outline_list_store = Gio::ListStore<OutlineTableRow>::create();

    outline_view_selection = Gtk::SingleSelection::create(
        outline_list_store
    );

    outline_view.set_model(outline_view_selection);

    // maximize();

    // win.set_hide_on_close(false);
    win.set_child(main_box);

    outline_view_refresh_btn.set_label("Refresh");

    main_box.append(menu_bar);
    main_box.append(paned);

    text_view_box.set_vexpand(true);

    text_view_box_upper.append(text_view_box);
    // text_view_box_upper.append(search_exp);

    text_view_box.append(linum_area);
    text_view_box.append(text_view_sw);

    paned.set_start_child(text_view_box_upper);
    paned.set_end_child(outline_box);

    outline_box.append(outline_view_refresh_btn);
    outline_box.append(outline_view_sw);

    text_view_sw.set_policy(
        Gtk::PolicyType::ALWAYS,
        Gtk::PolicyType::ALWAYS
    );
    outline_view_sw.set_policy(
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

    outline_view_sw.set_vexpand(true);
    outline_view_sw.set_valign(Gtk::Align::FILL);

    text_view_sw.set_overlay_scrolling(false);
    outline_view_sw.set_overlay_scrolling(false);

    text_view_sw.set_kinetic_scrolling(false);
    outline_view_sw.set_kinetic_scrolling(false);

    paned.set_vexpand(true);
    paned.set_resize_end_child(false);

    text_view_sw.set_child(text_view);
    outline_view_sw.set_child(outline_view);

    text_view.set_monospace(true);
    text_view.set_buffer(subject->getTextBuffer());

    make_menubar();
    make_actions();
    make_hotkeys();

    setup_outline_columns();

    linum_area.set_draw_func(
        sigc::mem_fun(*this, &CommonEditorWindow::redraw_linum)
    );

    subject->signal_modified_changed().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::updateTitle)
    );

    subject->signal_editors_save_state().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::saveState)
    );

    subject->signal_editors_restore_state().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::restoreState)
    );

    project_ctl->signal_updated_name().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::updateTitle)
    );

    outline_view_refresh_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_outline_refresh_btn)
    );

    outline_view.signal_activate().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_outline_activate)
    );

    win.signal_destroy().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_destroy_sig)
    );

    win.signal_close_request().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_signal_close_request),
        true
    );

    text_view_sw.get_vscrollbar()->get_adjustment()->signal_changed().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::force_redraw_linum)
    );

    text_view_sw.get_vscrollbar()->get_adjustment()->signal_value_changed().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::force_redraw_linum)
    );

    updateTitle();
}

CommonEditorWindow::~CommonEditorWindow()
{
    std::cout << "~CommonEditorWindow()" << std::endl;
    destroyer.run();
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

void CommonEditorWindow::setup_outline_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            [](
                const Glib::RefPtr<Gtk::ListItem> &list_item
            )
            {
                list_item->set_child(
                    *Gtk::make_managed<Gtk::Label>(
                        "",
                        Gtk::Align::START
                    )
                );
            }
        )
    );
    // sigc::mem_fun(*this, &CommonEditorWindow::table_cell_setup),
    // Gtk::Align::START
    factory->signal_bind().connect(
        sigc::bind(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto col = std::dynamic_pointer_cast<OutlineTableRow>(
                    list_item->get_item()
                );
                if (!col)
                    return;
                auto label = dynamic_cast<Gtk::Label *>(
                    list_item->get_child()
                );
                if (!label)
                    return;
                label->set_text(std::format("{}", col->line));
            }
        )
    );

    auto column = Gtk::ColumnViewColumn::create("Line", factory);
    column->set_fixed_width(50);
    column->set_resizable(true);
    outline_view.append_column(column);

    // -------------
    factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            [](
                const Glib::RefPtr<Gtk::ListItem> &list_item
            )
            {
                list_item->set_child(*Gtk::make_managed<Gtk::Label>(
                    "",
                    Gtk::Align::START
                )
                );
            }
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto col = std::dynamic_pointer_cast<OutlineTableRow>(
                    list_item->get_item()
                );
                if (!col)
                    return;
                auto label = dynamic_cast<Gtk::Label *>(
                    list_item->get_child()
                );
                if (!label)
                    return;
                label->set_text(col->text);
            }
        )
    );

    column = Gtk::ColumnViewColumn::create("Text", factory);
    column->set_expand(true);
    outline_view.append_column(column);
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

    sect = wmg.createWindowsMenu("editor_window");
    mm_windows->append_section(sect);

    menu_model = Gio::Menu::create();
    menu_model->append_submenu("Work Subject", mm_ws);
    menu_model->append_submenu("Search", mm_search);
    menu_model->append_submenu("Windows", mm_windows);

    menu_bar.set_menu_model(menu_model);
};

void CommonEditorWindow::make_special_menu()
{
    // note: this is to be overriden by inheriting class
    return;
}

void CommonEditorWindow::make_actions()
{
    auto action_group = Gio::SimpleActionGroup::create();

    action_group->add_action(
        "work_subject_reload",
        sigc::mem_fun(*this, &CommonEditorWindow::action_work_subject_reload)
    );

    action_group->add_action(
        "work_subject_save",
        sigc::mem_fun(*this, &CommonEditorWindow::action_work_subject_save)
    );

    action_group->add_action(
        "work_subject_save_as",
        sigc::mem_fun(*this, &CommonEditorWindow::action_work_subject_save_as)
    );

    action_group->add_action(
        "work_subject_close",
        sigc::mem_fun(*this, &CommonEditorWindow::action_work_subject_close)
    );

    action_group->add_action(
        "windows_prev_window",
        sigc::mem_fun(*this, &CommonEditorWindow::action_windows_prev_window)
    );

    action_group->add_action(
        "windows_next_window",
        sigc::mem_fun(*this, &CommonEditorWindow::action_windows_next_window)
    );

    action_group->add_action(
        "windows_duplicate_window",
        sigc::mem_fun(*this, &CommonEditorWindow::action_windows_duplicate_window)
    );

    action_group->add_action(
        "windows_close_window",
        sigc::mem_fun(*this, &CommonEditorWindow::action_windows_close_window)
    );

    wmg.addActionsToActionGroup(action_group);

    win.insert_action_group("editor_window", action_group);
}

void CommonEditorWindow::make_special_actions()
{
    // note: this is to be overriden by inheriting class
    return;
}

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
}

void CommonEditorWindow::make_special_hotkeys()
{
    // note: this is to be overriden by inheriting class
    return;
}

Glib::RefPtr<Gio::Menu> CommonEditorWindow::getMenuModel()
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

void CommonEditorWindow::setOutlineContents(
    std::vector<std::tuple<std::size_t, std::string>> val
)
{
    // todo: save and restore current view
    outline_list_store->remove_all();

    auto i = val.begin();
    while (i != val.end())
    {
        auto x = OutlineTableRow::create();

        std::tie(x->line, x->text) = *i;
        outline_list_store->append(x);
        ++i;
    }

    outline_list_store->sort(
        sigc::slot<
            int(
                const Glib::RefPtr<const OutlineTableRow> &,
                const Glib::RefPtr<const OutlineTableRow> &
            )>(
            [](const Glib::RefPtr<const OutlineTableRow> &p1,
               const Glib::RefPtr<const OutlineTableRow> &p2)
            {
                if (p1->line == p2->line)
                {
                    return 0;
                }
                if (p1->line < p2->line)
                {
                    return -1;
                }
                if (p1->line > p2->line)
                {
                    return 1;
                }
                return 0;
            }
        )
    );
}

std::vector<std::tuple<std::size_t, std::string>>
    CommonEditorWindow::genOutlineContents()
{
    return {};
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
            cont->move_to(0, i);
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

std::size_t CommonEditorWindow::getCursorOffsetPosition()
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

std::size_t CommonEditorWindow::getCurrentLine()
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

std::string CommonEditorWindow::getText()
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
    auto ed1 = dynamic_cast<CodeEditorAbstract *>(this);
    auto w   = FindText::create(ed1->getOwnPtr());
    w->show();

    // todo: move registration inside of FindText constructor
    // project_ctl->getController()->registerWindow(w);
    // setTransientWindow(w);
    // w->set_destroy_with_parent(true);
}

void CommonEditorWindow::action_windows_prev_window()
{
    std::cout << "CommonEditorWindow::action_windows_prev_window" << std::endl;
    project_ctl->showPrevEditor(getOwnPtr());
}

void CommonEditorWindow::action_windows_next_window()
{
    std::cout << "CommonEditorWindow::action_windows_next_window" << std::endl;
    project_ctl->showNextEditor(getOwnPtr());
}

void CommonEditorWindow::action_windows_duplicate_window()
{
    project_ctl->workSubjectNewEditor(subject);
}

void CommonEditorWindow::action_windows_close_window()
{
    destroy();
}

void CommonEditorWindow::on_outline_refresh_btn()
{
    auto oc = genOutlineContents();
    setOutlineContents(oc);
}

void CommonEditorWindow::on_outline_activate(guint val)
{
    auto x = outline_list_store->get_item(val);
    // std::cout << "x == " << x->line << std::endl;

    auto tb = subject->getTextBuffer();

    auto iter_at_line = tb->get_iter_at_line(x->line);
    text_view.scroll_to(iter_at_line, 0, 0.5, 0.5);
    tb->place_cursor(iter_at_line);
}

} // namespace wayround_i2p::ccedit
