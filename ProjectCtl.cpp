
#include "ProjectCtl.hpp"
#include "Controller.hpp"
#include "EditorListView.hpp"
#include "FileExplorer.hpp"
#include "WorkSubjectListView.hpp"

using namespace wayround_i2p::codeeditor;

ProjectCtl::ProjectCtl(std::shared_ptr<Controller> controller)
{
    this->controller = controller;

    work_subj_list_store = Gio::ListStore<WorkSubjectTableRow>::create();
    editors_list_store   = Gio::ListStore<CodeEditorTableRow>::create();

    show_file_explorer_btn.set_label("File Explorer");
    show_new_worksubject_list_btn.set_label("Work Subject List");
    show_new_editor_list_btn.set_label("Editor List");

    set_child(main_box);

    main_box.set_spacing(5);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(show_file_explorer_btn);
    main_box.append(show_new_worksubject_list_btn);
    main_box.append(show_new_editor_list_btn);

    show_file_explorer_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_file_explorer_btn)
    );

    show_new_worksubject_list_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_new_worksubject_list_btn)
    );

    show_new_editor_list_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_new_editor_list_btn)
    );

    signal_hide().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_hide_sig)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_destroy_sig)
    );
}

ProjectCtl::~ProjectCtl()
{
    std::cout << "~ProjectCtl()" << std::endl;
}

std::shared_ptr<Controller> ProjectCtl::getController()
{
    return controller;
}

bool ProjectCtl::workSubjectExists(std::filesystem::path fpth)
{
    // todo: fpth value checks
    for (unsigned int i = 0; i < work_subj_list_store->get_n_items(); i++)
    {
        auto x    = work_subj_list_store->get_item(i);
        auto x_fp = x->work_subj->getFullPath();
        if (x_fp == fpth)
        {
            return true;
        }
    }
    return false;
}

void ProjectCtl::workSubjectEnsureExistance(std::filesystem::path fpth)
{
    // todo: fpth value checks
    if (!workSubjectExists(fpth))
    {
        auto new_item       = WorkSubjectTableRow::create();
        new_item->work_subj = std::shared_ptr<WorkSubject>(
            new WorkSubject(
                controller,
                own_ptr,
                fpth
            )
        );
        work_subj_list_store->append(new_item);
        new_item->work_subj->reload();
    }
    return;
}

int ProjectCtl::workSubjectNewEditor(std::filesystem::path fpth)
{
    // todo: fpth value checks
    // todo: more work required here
    workSubjectEnsureExistance(fpth);
    auto [subj, err] = getWorkSubject(fpth);
    if (err != 0)
    {
        return err;
    }

    auto x = controller->getBuiltinMods()[0];

    {
        auto [editor, err] = x->newEditorForSubject(own_ptr, subj);
        if (err != 0)
        {
            return err;
        }

        auto new_editor_item = CodeEditorTableRow::create();

        new_editor_item->editor = editor;

        editors_list_store->append(new_editor_item);

        editor->show();
    }

    return 0;
}

std::tuple<
    std::shared_ptr<WorkSubject>,
    int>
    ProjectCtl::getWorkSubject(std::filesystem::path fpth)
{
    // todo: fpth value checks
    for (unsigned int i = 0; i < work_subj_list_store->get_n_items(); i++)
    {
        auto x    = work_subj_list_store->get_item(i);
        auto sbj  = x->work_subj;
        auto x_fp = sbj->getFullPath();
        if (x_fp == fpth)
        {
            return std::tuple(sbj, 0);
        }
    }
    return std::tuple(std::shared_ptr<WorkSubject>(), 0);
}

void ProjectCtl::unregisterEditor(CodeEditorAbstract *val)
{
    std::vector<std::shared_ptr<CodeEditorAbstract>> vec;

    for (int i = (editors_list_store->get_n_items() - 1); i > -1; i--)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->editor;
        if (x_ed.get() == val)
        {
            vec.push_back(x_ed);
            editors_list_store->remove(i);
        }
    }

    while (!vec.empty())
    {
        auto z = vec.begin();
        ((*z).get())->close();
        vec.erase(z);
    }
}

void ProjectCtl::unregisterEditor(std::shared_ptr<CodeEditorAbstract> val)
{
    unregisterEditor((CodeEditorAbstract *)(val.get()));
}

Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> ProjectCtl::getWorkSubjectListStore()
{
    return work_subj_list_store;
}

Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>> ProjectCtl::getCodeEditorListStore()
{
    return editors_list_store;
}

void ProjectCtl::on_show_file_explorer_btn()
{
    auto x = new FileExplorer(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::on_show_new_worksubject_list_btn()
{
    auto x = new WorkSubjectListView(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::on_show_new_editor_list_btn()
{
    auto x = new EditorListView(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::on_hide_sig()
{
    std::cout << "ProjectCtl sig hide" << std::endl;
}

void ProjectCtl::on_destroy_sig()
{
    work_subj_list_store->remove_all();
    editors_list_store->remove_all();
    std::cout << "ProjectCtl sig destroy" << std::endl;
    controller->cleanupGlobalProjCtl();
}

std::tuple<std::filesystem::path, int> ProjectCtl::getProjectPath()
{
    return controller->getPathProject(this);
}
