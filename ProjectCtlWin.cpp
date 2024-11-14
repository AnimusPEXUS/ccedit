
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
            own_ptr.reset();
        }
    ),
    main_box(Gtk::Orientation::VERTICAL, 5)
{

    this->project_ctl = project_ctl;
    this->controller  = project_ctl->getController();

    show_file_explorer_btn.set_label("File Explorer");

    win.set_child(main_box);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(show_file_explorer_btn);

    main_box.append(ws_ed_paned);

    ws_ed_paned.set_start_child(ws_main_box);
    ws_ed_paned.set_end_child(eds_main_box);
    ws_ed_paned.set_wide_handle(true);

    // ------- work subjects -------

    ws_view_sel = Gtk::MultiSelection::create(
        this->project_ctl->getWorkSubjectListStore()
    );

    ws_main_box.append(ws_tools_box);
    ws_main_box.append(ws_view_sw);

    ws_view_sw.set_child(ws_view);
    ws_view_sw.set_hexpand(true);
    ws_view_sw.set_vexpand(true);
    ws_view_sw.set_has_frame(true);

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
    eds_view_sw.set_has_frame(true);

    eds_add_columns();

    eds_view.set_model(eds_view_sel);

    // -------------------------

    show_file_explorer_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_show_file_explorer_btn)
    );

    project_ctl->signal_updated_name().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::updateTitle)
    );

    win.signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_destroy_sig)
    );

    controller->registerWindow(&win);

    updateTitle();
}

ProjectCtlWin::~ProjectCtlWin()
{
    std::cout << "~ProjectCtlWin()" << std::endl;
    destroyer.run();
}

void ProjectCtlWin::ws_add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectCtlWin::ws_table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectCtlWin::ws_table_subject_cell_bind)
        )
    );

    auto column = Gtk::ColumnViewColumn::create("Subject", factory);
    // column->set_fixed_width(200);
    // column->set_resizable(true);
    column->set_expand(true);
    ws_view.append_column(column);
}

void ProjectCtlWin::ws_table_cell_setup(
    const Glib::RefPtr<Gtk::ListItem> &list_item, Gtk::Align halign
)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void ProjectCtlWin::ws_table_subject_cell_bind(
    const Glib::RefPtr<Gtk::ListItem> &list_item
)
{
    auto col = std::dynamic_pointer_cast<WorkSubjectTableRow>(
        list_item->get_item()
    );
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text("todo");
}

void ProjectCtlWin::eds_add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectCtlWin::eds_table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &ProjectCtlWin::eds_table_subject_cell_bind)
        )
    );

    auto column = Gtk::ColumnViewColumn::create("FN", factory);
    // column->set_fixed_width(200);
    // column->set_resizable(true);
    column->set_expand(true);
    eds_view.append_column(column);
}

void ProjectCtlWin::eds_table_cell_setup(
    const Glib::RefPtr<Gtk::ListItem> &list_item, Gtk::Align halign
)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void ProjectCtlWin::eds_table_subject_cell_bind(
    const Glib::RefPtr<Gtk::ListItem> &list_item
)
{
    auto col = std::dynamic_pointer_cast<CodeEditorTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text("todo");
}

void ProjectCtlWin::on_show_file_explorer_btn()
{
    project_ctl->showNewFileExplorer();
}

void ProjectCtlWin::on_destroy_sig()
{
    std::cout << "ProjectCtlWin::on_destroy_sig()" << std::endl;
    destroyer.run();
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
    win.show();
}

void ProjectCtlWin::destroy()
{
    destroyer.run();
}

} // namespace wayround_i2p::ccedit
