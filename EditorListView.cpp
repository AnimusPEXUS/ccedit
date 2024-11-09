#include "EditorListView.hpp"

#include "ProjectCtl.hpp"

namespace wayround_i2p::ccedit
{

EditorListView_shared EditorListView::create(ProjectCtl_shared project_ctl)
{
    auto ret     = EditorListView_shared(new EditorListView(project_ctl));
    ret->own_ptr = ret;
    return ret;
}

EditorListView::EditorListView(ProjectCtl_shared project_ctl) :
    main_box(Gtk::Orientation::VERTICAL, 5),
    tools_box(Gtk::Orientation::HORIZONTAL, 5),
    destroyer([this]() {})
{
    this->project_ctl = project_ctl;

    eds_view_sel = Gtk::MultiSelection::create(
        this->project_ctl->getCodeEditorListStore()
    );

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(tools_box);
    main_box.append(eds_view_sw);

    eds_view_sw.set_child(eds_view);
    eds_view_sw.set_vexpand(true);
    eds_view_sw.set_has_frame(true);

    add_columns();

    eds_view.set_model(eds_view_sel);

    set_child(main_box);

    project_ctl->signal_updated_name()->connect(
        sigc::mem_fun(*this, &EditorListView::updateTitle)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &EditorListView::on_destroy_sig)
    );

    updateTitle();
}

EditorListView::~EditorListView()
{
    std::cout << "~EditorListView()" << std::endl;
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

    auto column = Gtk::ColumnViewColumn::create("FN", factory);
    // column->set_fixed_width(200);
    // column->set_resizable(true);
    column->set_expand(true);
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
    auto col = std::dynamic_pointer_cast<CodeEditorTableRow>(list_item->get_item());
    if (!col)
        return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label)
        return;
    label->set_text("todo");
}

void EditorListView::updateTitle()
{
    std::string new_title("global - Editor List - Code Editor");

    if (!project_ctl->isGlobalProject())
    {
        new_title = std::format(
            "{} - Editor List - Code Editor",
            project_ctl->getProjectName()
        );
    }

    set_title(new_title);
}

void EditorListView::on_destroy_sig()
{
    destroyer.run();
}

} // namespace wayround_i2p::ccedit
