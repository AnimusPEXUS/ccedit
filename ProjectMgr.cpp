

#include <iostream>

#include "Controller.hpp"
#include "ProjectMgr.hpp"
#include "ProjectMgrEditor.hpp"

namespace wayround_i2p::ccedit
{

ProjectMgr_shared ProjectMgr::create(Controller_shared controller)
{
    ProjectMgr_shared ret = ProjectMgr_shared(new ProjectMgr(controller));
    ret->own_ptr          = ret;
    // controller->registerWindow(ret->getWindowPtr());
    return ret;
}

ProjectMgr::ProjectMgr(Controller_shared controller) :
    destroyer(
        [this]()
        {
            std::cout
                << "ProjectMgr::united_destroy_routines.run()"
                << std::endl;
            this->win.destroy();
            this->controller->destroyProjectMgr();
            own_ptr.reset();
        }
    )
{

    if (!controller)
    {
        throw "controller is null";
    }

    this->controller = controller;

    project_list_view_selection = Gtk::MultiSelection::create(
        this->controller->getProjectListStore()
    );
    // project_list_view_selection->set_autoselect(false);
    // project_list_view_selection->set_can_unselect(true);

    win.set_title("Project Manager :: Create/Edit/Remove/Open - Code Editor");
    win.set_child(main_box);

    // main_box.set_margin_top(5);
    // main_box.set_margin_start(5);
    // main_box.set_margin_end(5);
    // main_box.set_margin_bottom(5);

    project_list_view.set_hexpand();
    project_list_view.set_vexpand();
    project_list_view.set_halign(Gtk::Align::FILL);
    project_list_view.set_valign(Gtk::Align::FILL);

    // project_list_view.set_enable_rubberband(true);

    project_list_view.set_reorderable(false);

    project_list_view.set_model(project_list_view_selection);

    add_columns();

    //     main_box.set_spacing(5);
    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    main_box.append(project_list_sw);
    main_box.append(button_box);

    project_list_sw.set_child(project_list_view);
    //     project_list_sw.set_has_frame(true);

    button_box.set_spacing(5);
    button_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // button_box.set_halign(Gtk::Align::FILL);
    // button_box.set_hexpand(true);
    button_box.add_css_class("toolbar");

    button_box_separator.set_hexpand(true);
    //  button_box_sec.set_spacing(5);
    button_box_separator.set_orientation(Gtk::Orientation::HORIZONTAL);

    // separ.set_orientation(Gtk::Orientation::VERTICAL);

    bb1.add_css_class("linked");
    bb2.add_css_class("linked");
    bb3.add_css_class("linked");
    bb5.add_css_class("linked");
    bb1.set_orientation(Gtk::Orientation::HORIZONTAL);
    bb2.set_orientation(Gtk::Orientation::HORIZONTAL);
    bb3.set_orientation(Gtk::Orientation::HORIZONTAL);
    bb5.set_orientation(Gtk::Orientation::HORIZONTAL);

    bb1.append(btn_add_proj);
    bb1.append(btn_rm_proj);
    bb1.append(btn_edit_proj);

    bb2.append(btn_open_proj);

    bb3.append(btn_open_global);

    bb5.append(btn_quit);

    button_box.append(bb1);
    button_box.append(bb2);
    button_box.append(bb3);
    button_box.append(button_box_separator);
    button_box.append(bb5);

    btn_add_proj.set_label("add");
    btn_rm_proj.set_label("rm");
    btn_edit_proj.set_label("edit");
    btn_open_proj.set_label("open proj ctrl");

    btn_open_global.set_label("open global ctrl");

    btn_quit.set_label("quit");

    // save_cfg.set_hexpand(true);
    // save_cfg.set_halign(Gtk::Align::END);

    btn_add_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_add_click)
    );

    btn_rm_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_rm_click)
    );

    btn_edit_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_edit_click)
    );

    btn_open_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_open_click)
    );

    btn_open_global.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_open_global_click)
    );

    btn_quit.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_btn_quit_click)
    );

    win.signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_destroy_sig)
    );

    win.signal_close_request().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_signal_close_request),
        true
    );
}

ProjectMgr::~ProjectMgr()
{
    std::cout << "ProjectMgr::~ProjectMgr()" << std::endl;
    destroyer.run();
}

void ProjectMgr::on_destroy_sig()
{
    std::cout << "ProjectMgr::on_destroy_sig()" << std::endl;
    destroyer.run();
}

bool ProjectMgr::on_signal_close_request()
{
    std::cout << "ProjectMgr::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

void ProjectMgr::show()
{
    win.present();
}

void ProjectMgr::destroy()
{
    std::cout << "ProjectMgr::destroy()" << std::endl;
    destroyer.run();
}

Controller_shared ProjectMgr::getController()
{
    return controller;
}

Gtk::Window *ProjectMgr::getWindowPtr()
{
    return &win;
}

Gtk::Window &ProjectMgr::getWindowRef()
{
    return win;
}

void ProjectMgr::add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_name_cell_bind)
        )
    );
    factory->signal_unbind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_name_cell_unbind)
        )
    );

    auto column = Gtk::ColumnViewColumn::create("Project", factory);
    column->set_fixed_width(200);
    column->set_resizable(true);
    project_list_view.append_column(column);

    // -------------
    factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_path_cell_bind)
        )
    );
    factory->signal_unbind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectMgr::table_path_cell_unbind)
        )
    );

    column = Gtk::ColumnViewColumn::create("Path", factory);
    column->set_expand(true);
    project_list_view.append_column(column);
}

void ProjectMgr::table_cell_setup(const Glib::RefPtr<Gtk::ListItem> &list_item, Gtk::Align halign)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void ProjectMgr::table_name_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text(col->proj_name());
    col->signal_proj_name_changed().connect(
        [label, col]() -> void
        {
            label->set_text(col->proj_name());
        }
    );
}

void ProjectMgr::table_name_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    col->signal_proj_name_changed().clear();
}

void ProjectMgr::table_path_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text(col->proj_path().string());
    col->signal_proj_path_changed().connect(
        [label, col]() -> void
        {
            label->set_text(col->proj_path().string());
        }
    );
}

void ProjectMgr::table_path_cell_unbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    col->signal_proj_path_changed().clear();
}

void ProjectMgr::on_btn_add_click()
{
    auto w = ProjectMgrEditor::create(own_ptr, "", "");
    w->show();
}

void ProjectMgr::on_btn_rm_click()
{
    auto x = project_list_view_selection->get_selection();

    auto list = controller->getProjectListStore();

    while (!(x->is_empty()))
    {
        list->remove(*(x->begin()));
        x = project_list_view_selection->get_selection();
    }
}

void ProjectMgr::on_btn_edit_click()
{
    std::cout << "edited" << std::endl;
    auto x = project_list_view_selection->get_selection();
    auto i = x->begin();

    auto list = controller->getProjectListStore();

    while (i != x->end())
    {
        auto item = list->get_item(*i);

        auto w = ProjectMgrEditor::create(
            own_ptr,
            item->proj_name(),
            item->proj_path()
        );

        w->show();

        ++i;
    }
}

void ProjectMgr::on_btn_open_click()
{
    auto x    = project_list_view_selection->get_selection();
    auto i    = x->begin();
    auto list = controller->getProjectListStore();
    while (i != x->end())
    {
        auto item = list->get_item(*i);
        // todo: error check?
        controller->showProjCtlWin(item->proj_name());
        ++i;
    }
}

void ProjectMgr::on_btn_open_global_click()
{
    controller->showGlobalProjCtlWin();
}

void ProjectMgr::on_btn_quit_click()
{
    controller->quit();
}

} // namespace wayround_i2p::ccedit
