#include "EditorListView.hpp"

using namespace wayround_i2p::codeeditor;

EditorListView::EditorListView(
    std::shared_ptr<ProjectCtl> project_ctl
)
{
    this->project_ctl = project_ctl;

    main_box.append(tools_box);
    main_box.append(eds_view_sw);

    eds_view_sw.set_child(eds_view);

    add_columns();
}

void EditorListView::add_columns()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::bind(
            sigc::mem_fun(*this, &EditorListView::table_cell_setup),
            Gtk::Align::START
        )
    );
    factory->signal_bind().connect(
        sigc::bind(
            sigc::mem_fun(*this, &EditorListView::table_subject_cell_bind)
        )
    );

    auto column = Gtk::ColumnViewColumn::create("Subject", factory);
    // column->set_fixed_width(200);
    // column->set_resizable(true);
    eds_view.append_column(column);

    // -------------
    // todo: add "Changed" column
    // factory = Gtk::SignalListItemFactory::create();
    // factory->signal_setup().connect(
    //     sigc::bind(
    //         sigc::mem_fun(*this, &EditorListView::table_cell_setup),
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
    // eds_view.append_column(column);
}

void EditorListView::table_cell_setup(const Glib::RefPtr<Gtk::ListItem> &list_item, Gtk::Align halign)
{
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", halign));
}

void EditorListView::table_subject_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    auto col = std::dynamic_pointer_cast<ProjectTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text("todo");
}

void EditorListView::on_destroy_sig()
{
    delete this;
}
