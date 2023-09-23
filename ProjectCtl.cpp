
#include <format>

#include "Controller.hpp"
#include "EditorListView.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubjectListView.hpp"

using namespace wayround_i2p::codeeditor;

ProjectCtl::ProjectCtl(std::shared_ptr<Controller> controller)
{
    this->controller = controller;

    work_subj_list_store = Gio::ListStore<WorkSubjectTableRow>::create();
    editors_list_store   = Gio::ListStore<CodeEditorTableRow>::create();

    priv_signal_updated_name = std::shared_ptr<sigc::signal<void()>>(
        new sigc::signal<void()>()
    );

    priv_signal_updated_path = std::shared_ptr<sigc::signal<void()>>(
        new sigc::signal<void()>()
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
    for (int i = 0; i != work_subj_list_store->get_n_items(); i++)
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

void ProjectCtl::close()
{
    // todo: close and destroy all subwindows and work subjects
    controller->closeProjCtl(this);
}

bool ProjectCtl::isGlobalProject()
{
    return controller->isGlobalProjCtl(this);
}

std::tuple<std::string, int> ProjectCtl::getProjectName()
{
    return controller->getNameProject(this);
}

std::tuple<std::filesystem::path, int> ProjectCtl::getProjectPath()
{
    return controller->getPathProject(this);
}

void ProjectCtl::projectControllerRegisteredInController()
{
    // updateTitle();
    updatedName();
    updatedPath();
}

void ProjectCtl::updatedName()
{
    // updateTitle();
    priv_signal_updated_name->emit();
}

void ProjectCtl::updatedPath()
{
    priv_signal_updated_path->emit();
}

void ProjectCtl::showWindow()
{
    if (!proj_ctl_win)
    {
        proj_ctl_win = std::shared_ptr<ProjectCtlWin>(
            new ProjectCtlWin(own_ptr)
        );
        proj_ctl_win->own_ptr = proj_ctl_win;
    }

    proj_ctl_win->show();

    return;
}

void ProjectCtl::closeWindow()
{
    if (proj_ctl_win)
    {
        proj_ctl_win->close();
        proj_ctl_win->own_ptr.reset();
        proj_ctl_win.reset();
    }
}

void ProjectCtl::showNewFileExplorer()
{
    auto x = new FileExplorer(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::showNewWorkSubjectList()
{
    auto x = new WorkSubjectListView(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::showNewEditorList()
{
    auto x = new EditorListView(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

std::shared_ptr<sigc::signal<void()>> ProjectCtl::signal_updated_name()
{
    return priv_signal_updated_name;
}

std::shared_ptr<sigc::signal<void()>> ProjectCtl::signal_updated_path()
{
    return priv_signal_updated_path;
}
