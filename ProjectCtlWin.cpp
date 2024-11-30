
#include <format>

#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtl.hpp"

#include "ProjectCtlWin.hpp"

namespace wayround_i2p::ccedit
{

ProjectCtlWin_shared ProjectCtlWin::create(ProjectCtl_shared project_ctl)
{
    auto ret     = ProjectCtlWin_shared(new ProjectCtlWin(project_ctl));
    ret->own_ptr = ret;
    return ret;
}

ProjectCtlWin::ProjectCtlWin(ProjectCtl_shared project_ctl) :
    destroyer(
        [this]()
        {
            std::cout << "ProjectCtlWin: destroyer.run()" << std::endl;
            win.destroy();
            this->project_ctl->destroyWindow();
            own_ptr.reset();
        }
    ),
    main_box(Gtk::Orientation::VERTICAL, 0),
    wmg(project_ctl)
{

    this->project_ctl = project_ctl;
    this->controller  = project_ctl->getController();

    win.set_child(main_box);

    //    main_box.set_margin_top(5);
    //     main_box.set_margin_start(5);
    //     main_box.set_margin_end(5);
    //     main_box.set_margin_bottom(5);

    main_box.append(b_box);

    b_box.append(show_windows_btn);
    b_box.append(rotate_paned_btn);

    b_box.add_css_class("toolbar");

    main_box.append(ws_ed_paned);

    ws_ed_paned.set_start_child(ws_main_box);
    ws_ed_paned.set_end_child(eds_main_box);
    // ws_ed_paned.set_wide_handle(true);

    show_windows_btn.set_menu_model(
        wmg.createProjectMenu("project_ctl_window")
    );
    // show_windows_btn.add_css_class("circular");
    // show_windows_btn.set_has_frame(true);
    show_windows_btn.set_icon_name("applications-utilities");
    show_windows_btn.set_tooltip_text("Project Menu");

    rotate_paned_btn.set_icon_name("object-rotate-right");
    rotate_paned_btn.set_tooltip_text("Rorate Lists");

    // ------- work subjects -------

    ws_view_sel = Gtk::MultiSelection::create(
        this->project_ctl->getWorkSubjectListStore()
    );

    ws_main_box.append(ws_tools_box);
    ws_main_box.append(ws_view_sw);

    ws_view_sw.set_child(ws_view);
    ws_view_sw.set_hexpand(true);
    ws_view_sw.set_vexpand(true);
    // ws_view_sw.set_has_frame(true);

    ws_add_columns();

    ws_view.set_model(ws_view_sel);

    // ------- editors -------

    eds_view_sel = Gtk::MultiSelection::create(
        this->project_ctl->getCodeEditorListStore()
    );

    eds_main_box.append(eds_tools_box);
    eds_main_box.append(eds_view_sw);

    eds_view_sw.set_child(eds_view);
    eds_view_sw.set_hexpand(true);
    eds_view_sw.set_vexpand(true);
    // eds_view_sw.set_has_frame(true);

    eds_add_columns();

    eds_view.set_model(eds_view_sel);

    // -------------------------

    project_ctl->signal_updated_name().connect(
        [this]()
        { updateTitle(); }
    );

    win.signal_destroy().connect(
        [this]()
        { on_destroy_sig(); }
    );

    win.signal_close_request().connect(
        [this]() -> bool
        { return on_signal_close_request(); },
        true
    );

    rotate_paned_btn.signal_clicked().connect(
        [this]()
        { on_rotate_paned_btn(); }
    );

    auto action_group = Gio::SimpleActionGroup::create();
    wmg.addActionsToActionGroup(action_group);
    win.insert_action_group("project_ctl_window", action_group);

    controller->registerWindow(&win);

    updateTitle();
}

ProjectCtlWin::~ProjectCtlWin()
{
    std::cout << "~ProjectCtlWin()" << std::endl;
    destroyer.run();
}

void ProjectCtlWin::destroy()
{
    std::cout << "ProjectCtlWin::destroy()" << std::endl;
    destroyer.run();
}

void ProjectCtlWin::on_destroy_sig()
{
    std::cout << "ProjectCtlWin::on_destroy_sig()" << std::endl;
    destroyer.run();
}

bool ProjectCtlWin::on_signal_close_request()
{
    std::cout << "ProjectCtlWin::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

void ProjectCtlWin::on_rotate_paned_btn()
{
    ws_ed_paned.set_orientation(
        (
            ws_ed_paned.get_orientation() == Gtk::Orientation::HORIZONTAL ?
                Gtk::Orientation::VERTICAL :
                Gtk::Orientation::HORIZONTAL
        )
    );
}

void ProjectCtlWin::ws_add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            list_item->set_child(*Gtk::make_managed<WorkSubjectTableRowWidget>());
        }
    );
    factory->signal_bind().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            auto col = std::dynamic_pointer_cast<TableItemTpl<WorkSubject_shared>>(
                list_item->get_item()
            );
            if (!col)
                return;
            auto x = dynamic_cast<WorkSubjectTableRowWidget *>(list_item->get_child());
            if (!x)
                return;
            x->bind(list_item);
        }
    );

    auto column = Gtk::ColumnViewColumn::create("Open Work Subjects", factory);
    column->set_expand(true);
    ws_view.append_column(column);
}

void ProjectCtlWin::eds_add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            list_item->set_child(*Gtk::make_managed<CodeEditorTableRowWidget>());
        }
    );
    factory->signal_bind().connect(
        [](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            auto col = std::dynamic_pointer_cast<TableItemTpl<CodeEditorAbstract_shared>>(
                list_item->get_item()
            );
            if (!col)
                return;
            auto x = dynamic_cast<CodeEditorTableRowWidget *>(list_item->get_child());
            if (!x)
                return;
            x->bind(list_item);
        }
    );

    auto column = Gtk::ColumnViewColumn::create("Open Editors", factory);
    column->set_expand(true);
    eds_view.append_column(column);
}

void ProjectCtlWin::updateTitle()
{
    std::string new_title;

    std::string proj_name("(global)");
    if (!project_ctl->isGlobalProject())
    {
        proj_name = project_ctl->getProjectName();
    }

    new_title = std::format(
        "{} - (Project) - Code Editor",
        proj_name
    );

    win.set_title(new_title);
}

void ProjectCtlWin::show()
{
    win.present();
}

} // namespace wayround_i2p::ccedit
