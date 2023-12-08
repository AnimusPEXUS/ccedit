
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

bool ProjectCtl::workSubjectExists(
    std::filesystem::path fpth
)
{
    return bool(getWorkSubject(fpth));
}

std::shared_ptr<WorkSubject> ProjectCtl::getWorkSubject(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks
    for (unsigned int i = 0; i < work_subj_list_store->get_n_items(); i++)
    {
        auto x    = work_subj_list_store->get_item(i);
        auto sbj  = x->work_subj;
        auto x_fp = sbj->getFullPath();
        if (x_fp == fpth)
        {
            return sbj;
        }
    }
    return nullptr;
}

std::shared_ptr<WorkSubject> ProjectCtl::workSubjectEnsureExistance(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks
    std::shared_ptr<WorkSubject> ret;

    if (ret = getWorkSubject(fpth))
    {
        return ret;
    }
    else
    {
        auto new_item = WorkSubjectTableRow::create();
        ret           = WorkSubject::create(
            controller,
            own_ptr,
            fpth
        );
        new_item->work_subj = ret;
        work_subj_list_store->append(new_item);
        new_item->work_subj->reload();
        return ret;
    }
}

std::shared_ptr<CodeEditorAbstract> ProjectCtl::workSubjectExistingOrNewEditor(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks
    auto subj = workSubjectEnsureExistance(fpth);
    if (!subj)
    {
        return nullptr;
    }
    return workSubjectExistingOrNewEditor(subj);
}

std::shared_ptr<CodeEditorAbstract> ProjectCtl::workSubjectNewEditor(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks
    auto subj = workSubjectEnsureExistance(fpth);
    if (!subj)
    {
        return nullptr;
    }

    return workSubjectNewEditor(subj);
}

std::shared_ptr<CodeEditorAbstract> ProjectCtl::workSubjectExistingOrNewEditor(
    std::shared_ptr<WorkSubject> val
)
{
    for (unsigned int i = 0; i != editors_list_store->get_n_items(); i++)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->editor;
        if (x_ed->workSubjectIs(val))
        {
            return x_ed;
        }
    }

    return workSubjectNewEditor(val);
}

std::shared_ptr<CodeEditorAbstract> ProjectCtl::workSubjectNewEditor(
    std::shared_ptr<WorkSubject> val
)
{
    auto ed = createBestEditorForWorkSubject(val);
    if (!ed)
    {
        return nullptr;
    }
    registerEditor(ed);
    return ed;
}

std::shared_ptr<CodeEditorAbstract> ProjectCtl::createBestEditorForWorkSubject(
    std::shared_ptr<WorkSubject> subj
)
{
    auto x = controller->getBuiltinMods()[0];

    auto editor             = x->newEditorForSubject(own_ptr, subj);
    auto new_editor_item    = CodeEditorTableRow::create();
    new_editor_item->editor = editor;

    // todo: create special function for registering/unregistering editor
    //       windows - don't use stores directly
    editors_list_store->append(new_editor_item);
    editor->show();

    return editor;
}

void ProjectCtl::registerEditor(std::shared_ptr<CodeEditorAbstract> val)
{
    // todo: register in Application?
    // todo: check is already exists

    auto v    = CodeEditorTableRow::create();
    v->editor = val;
    editors_list_store->append(v);
    controller->registerWindow(std::dynamic_pointer_cast<Gtk::Window>(val));
}

void ProjectCtl::unregisterEditor(std::shared_ptr<CodeEditorAbstract> val)
{
    std::vector<std::shared_ptr<CodeEditorAbstract>> vec;

    for (int i = (editors_list_store->get_n_items() - 1); i > -1; i--)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->editor;
        if (x_ed == val)
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

void ProjectCtl::destroyEditor(std::shared_ptr<CodeEditorAbstract> val)
{
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
    auto x = FileExplorer::create(own_ptr);
    x->show();
    controller->registerWindow(x);
}

void ProjectCtl::showNewWorkSubjectList()
{
    auto x = new WorkSubjectListView(this->own_ptr);
    x->show();
    controller->registerWindow(x);
}

void ProjectCtl::showNewEditorList()
{
    auto x = new EditorListView(this->own_ptr);
    x->show();
    controller->registerWindow(x);
}

std::shared_ptr<sigc::signal<void()>> ProjectCtl::signal_updated_name()
{
    return priv_signal_updated_name;
}

std::shared_ptr<sigc::signal<void()>> ProjectCtl::signal_updated_path()
{
    return priv_signal_updated_path;
}
