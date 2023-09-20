

#include <iostream>

#include "ProjectMgr.hpp"
#include "ProjectMgrEditor.hpp"

using namespace wayround_i2p::codeeditor;

ProjectMgr::ProjectMgr(std::shared_ptr<Controller> controller)
{
    this->controller = controller;

    project_list_view_selection = Gtk::MultiSelection::create(
        this->controller->getProjectListStore()
    );
    // project_list_view_selection->set_autoselect(false);
    // project_list_view_selection->set_can_unselect(true);

    set_title("Project Manager :: Create/Edit/Remove/Open - Code Editor");
    set_child(main_box);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    project_list_view.set_hexpand();
    project_list_view.set_vexpand();
    project_list_view.set_halign(Gtk::Align::FILL);
    project_list_view.set_valign(Gtk::Align::FILL);

    project_list_view.set_reorderable(false);

    project_list_view.set_model(project_list_view_selection);

    add_columns();

    main_box.set_spacing(5);
    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    main_box.append(project_list_sw);
    main_box.append(button_box);

    project_list_sw.set_child(project_list_view);
    project_list_sw.set_has_frame(true);

    button_box.set_spacing(5);
    button_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    button_box.set_halign(Gtk::Align::FILL);
    button_box.set_hexpand(true);

    button_box_sec.set_hexpand(true);
    button_box_sec.set_spacing(5);
    button_box_sec.set_orientation(Gtk::Orientation::HORIZONTAL);
    button_box_sec.set_halign(Gtk::Align::END);

    // separ.set_orientation(Gtk::Orientation::VERTICAL);

    button_box.append(add_proj);
    button_box.append(rm_proj);
    button_box.append(edit_proj);

    button_box.append(sep0);
    button_box.append(open_proj);

    button_box.append(sep1);
    button_box.append(open_global);

    button_box.append(button_box_sec);

    button_box_sec.append(modules_info_print);
    button_box_sec.append(save_cfg);

    add_proj.set_label("add");
    rm_proj.set_label("rm");
    edit_proj.set_label("edit");
    open_proj.set_label("open proj ctrl");

    open_global.set_label("open global ctrl");

    modules_info_print.set_label("print mods");
    save_cfg.set_label("rewrite config");

    // save_cfg.set_hexpand(true);
    // save_cfg.set_halign(Gtk::Align::END);

    add_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_add_click)
    );

    rm_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_rm_click)
    );

    edit_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_edit_click)
    );

    open_proj.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_open_click)
    );

    open_global.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_open_global_click)
    );

    save_cfg.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_save_cfg)
    );

    modules_info_print.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_modules_info_print)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectMgr::on_destroy_sig)
    );

    // project_list.set_model(project_list_store);
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
    label->set_text(col->proj_name);
}

void ProjectMgr::table_path_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text(col->proj_path.string());
}

void ProjectMgr::on_add_click()
{
    // todo: mem leak?
    auto w = new ProjectMgrEditor(controller, "", "");
    w->set_transient_for(*this);
    w->set_destroy_with_parent(true);
    w->show();
    controller->getGtkApp()->add_window(*w);
}

void ProjectMgr::on_rm_click()
{
    auto x = project_list_view_selection->get_selection();

    auto list = controller->getProjectListStore();

    while (!(x->is_empty()))
    {
        list->remove(*(x->begin()));
        x = project_list_view_selection->get_selection();
    }
}

void ProjectMgr::on_edit_click()
{
    std::cout << "edited" << std::endl;
    auto x = project_list_view_selection->get_selection();
    auto i = x->begin();

    auto list = controller->getProjectListStore();

    while (i != x->end())
    {
        auto item = list->get_item(*i);

        // todo: mem leak?
        auto w = new ProjectMgrEditor(
            controller,
            item->proj_name,
            item->proj_path
        );

        w->set_transient_for(*this);
        w->set_destroy_with_parent(true);
        w->show();
        controller->getGtkApp()->add_window(*w);
        ++i;
    }
}

void ProjectMgr::on_open_click()
{
    auto x    = project_list_view_selection->get_selection();
    auto i    = x->begin();
    auto list = controller->getProjectListStore();
    while (i != x->end())
    {
        auto item = list->get_item(*i);
        controller->showProjCtl(item->proj_name);
        ++i;
    }
}

void ProjectMgr::on_open_global_click()
{
    controller->showGlobalProjCtl();
}

void ProjectMgr::on_save_cfg()
{
    controller->saveConfig();
}

void ProjectMgr::on_modules_info_print()
{
    auto mods = controller->getBuiltinMods();

    std::cout << "mod count: " << mods.size() << std::endl;

    for (auto x : mods)
    {
        printInfoCodeEditorMod(x);
    }
}

void ProjectMgr::on_destroy_sig()
{
    std::cout << "ProjectMgr sig destroy" << std::endl;
    controller->cleanupProjectMgr();
}
