#include "WorkSubjectListView.hpp"

using namespace wayround_i2p::ccedit;

WorkSubjectListView::WorkSubjectListView(
    ProjectCtl_shared project_ctl
) :
    main_box(Gtk::Orientation::VERTICAL, 5),
    tools_box(Gtk::Orientation::HORIZONTAL, 5)
{
    this->project_ctl = project_ctl;

    ws_view_sel = Gtk::MultiSelection::create(
        this->project_ctl->getWorkSubjectListStore()
    );

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(tools_box);
    main_box.append(ws_view_sw);

    ws_view_sw.set_child(ws_view);
    ws_view_sw.set_vexpand(true);
    ws_view_sw.set_has_frame(true);

    add_columns();

    ws_view.set_model(ws_view_sel);

    set_child(main_box);

    project_ctl->signal_updated_name()->connect(
        sigc::mem_fun(*this, &WorkSubjectListView::updateTitle)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &WorkSubjectListView::on_destroy_sig)
    );

    updateTitle();
}

WorkSubjectListView::~WorkSubjectListView()
{
    std::cout << "~WorkSubjectListView()" << std::endl;
}

void WorkSubjectListView::add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &WorkSubjectListView::table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &WorkSubjectListView::table_subject_cell_bind)
        )
    );

    auto column = Gtk::ColumnViewColumn::create("Subject", factory);
    // column->set_fixed_width(200);
    // column->set_resizable(true);
    column->set_expand(true);
    ws_view.append_column(column);

    // -------------
    // todo: add "Changed" column
    // factory = Gtk::SignalListItemFactory::create();
    // factory->signal_setup().connect(
    //     sigc::bind(
    //         sigc::mem_fun(*this, &WorkSubjectListView::table_cell_setup),
    //         Gtk::Align::START
    //     )
    // );
    // factory->signal_bind().connect(
    //     sigc::bind(
    //         sigc::mem_fun(*this, &ProjectMgr::table_path_cell_bind)
    //     )
    // );
    // column = Gtk::ColumnViewColumn::create("Changed", factory);
    // column->set_expand(true);
    // ws_view.append_column(column);
}

void WorkSubjectListView::table_cell_setup(const Glib::RefPtr<Gtk::ListItem> &list_item, Gtk::Align halign)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void WorkSubjectListView::table_subject_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<WorkSubjectTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text("todo");
}

void WorkSubjectListView::updateTitle()
{
    std::string new_title("");

    if (project_ctl->isGlobalProject())
    {
        new_title = "global - Subject List - Code Editor";
    }
    else
    {
        new_title = std::format(
            "{} - Subject List - Code Editor",
            project_ctl->getProjectName()
        );
    }

    set_title(new_title);
}

void WorkSubjectListView::on_destroy_sig()
{
    delete this;
}
