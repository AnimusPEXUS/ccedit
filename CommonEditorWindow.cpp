#include "CommonEditorWindow.hpp"

using namespace wayround_i2p::codeeditor;

CommonEditorWindow::CommonEditorWindow(
    std::shared_ptr<ProjectCtl>  project_ctl,
    std::shared_ptr<WorkSubject> subject
) :
    main_box(Gtk::Orientation::VERTICAL, 5)
{
    this->project_ctl = project_ctl;
    this->subject     = subject;

    set_child(main_box);

    main_box.append(menu_bar);
    main_box.append(paned);

    paned.set_start_child(text_view_sw);
    paned.set_end_child(outline_view_sw);

    text_view_sw.set_policy(
        Gtk::PolicyType::ALWAYS,
        Gtk::PolicyType::ALWAYS
    );
    outline_view_sw.set_policy(
        Gtk::PolicyType::ALWAYS,
        Gtk::PolicyType::ALWAYS
    );

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

    updateTitle();

    signal_destroy().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_destroy_sig)
    );
}

CommonEditorWindow::~CommonEditorWindow()
{
    std::cout << "~CommonEditorWindow()" << std::endl;
}

void CommonEditorWindow::make_menubar()
{
    menu_model = Gio::Menu::create();
    menu_bar.set_menu_model(menu_model);

    mm_buffer = Gio::Menu::create();
    menu_model->append_submenu("Buffer", mm_buffer);

    mm_buffer_reload = Gio::MenuItem::create(
        "(Re)load",
        "editor_window.buffer_reload"
    );
    mm_buffer_save = Gio::MenuItem::create(
        "Save",
        "editor_window.buffer_save"
    );
    mm_buffer_save_as = Gio::MenuItem::create(
        "Save as..",
        "editor_window.buffer_save_as"
    );

    mm_buffer->append_item(mm_buffer_reload);
    mm_buffer->append_item(mm_buffer_save);
    mm_buffer->append_item(mm_buffer_save_as);
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
        "buffer_reload",
        sigc::mem_fun(*this, &CommonEditorWindow::action_buffer_reload)
    );
    action_group->add_action(
        "buffer_save",
        sigc::mem_fun(*this, &CommonEditorWindow::action_buffer_save)
    );
    action_group->add_action(
        "buffer_save_as",
        sigc::mem_fun(*this, &CommonEditorWindow::action_buffer_save_as)
    );
    insert_action_group("editor_window", action_group);
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
        Gtk::NamedAction::create("editor_window.buffer_reload")
    ));
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_s,
            Gdk::ModifierType::CONTROL_MASK
        ),
        Gtk::NamedAction::create("editor_window.buffer_save")
    ));
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(
            GDK_KEY_s,
            Gdk::ModifierType::CONTROL_MASK
                | Gdk::ModifierType::SHIFT_MASK
        ),
        Gtk::NamedAction::create("editor_window.buffer_save_as")
    ));
    add_controller(controller);
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
    auto pth = subject->getPath();
    set_title((pth.filename()).string());
}

void CommonEditorWindow::saveOwnPtr(std::shared_ptr<CodeEditorAbstract> val)
{
    own_ptr = val;
}

void CommonEditorWindow::show()
{
    auto x = (Gtk::Window *)this;
    x->show();
}

void CommonEditorWindow::close()
{
    auto x = (Gtk::Window *)this;
    x->close();
}

void CommonEditorWindow::action_buffer_reload()
{
    std::cout << "reload" << std::endl;
    subject->reload();
}

void CommonEditorWindow::action_buffer_save()
{
    std::cout << "save" << std::endl;
    subject->save();
}

void CommonEditorWindow::action_buffer_save_as()
{
    std::cout << "save as" << std::endl;
}

void CommonEditorWindow::on_destroy_sig()
{
    project_ctl->unregisterEditor(own_ptr);
    own_ptr.reset();
}

std::shared_ptr<ProjectCtl> CommonEditorWindow::getProjectCtl()
{
    return project_ctl;
}
