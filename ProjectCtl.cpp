
#include <format>

#include "Controller.hpp"
#include "EditorListView.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubjectListView.hpp"

using namespace wayround_i2p::ccedit;

ProjectCtl::ProjectCtl(Controller_shared controller)
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

Controller_shared ProjectCtl::getController()
{
    return controller;
}

bool ProjectCtl::isGlobalProject()
{
    return controller->isGlobalProjCtl(this);
}

std::string ProjectCtl::getProjectName()
{
    auto [name, err] = controller->getNameProject(this);
    if (err != 0)
    {
        // todo: better action needed. atlease message to user
        close();
    }

    return name;
}

std::filesystem::path ProjectCtl::getProjectPath()
{
    auto [pth, err] = controller->getPathProject(this);
    if (err != 0)
    {
        // todo: better action needed. atlease message to user
        close();
    }

    return pth;
}

bool ProjectCtl::workSubjectExists(
    std::filesystem::path fpth
)
{
    return bool(getWorkSubject(fpth));
}

WorkSubject_shared ProjectCtl::getWorkSubject(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks here and for WorkSubject
    for (unsigned int i = 0; i < work_subj_list_store->get_n_items(); i++)
    {
        auto x    = work_subj_list_store->get_item(i);
        auto sbj  = x->work_subj;
        auto x_fp = sbj->getPath();
        if (x_fp == fpth)
        {
            return sbj;
        }
    }
    return nullptr;
}

WorkSubject_shared ProjectCtl::workSubjectEnsureExistance(
    std::filesystem::path fpth
)
{
    // todo: fpth value checks
    WorkSubject_shared ret;

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

CodeEditorAbstract_shared ProjectCtl::workSubjectExistingOrNewEditor(
    std::filesystem::path fpth
)
{
    std::cout << "workSubjectExistingOrNewEditor(" << fpth << ")" << std::endl;
    // todo: fpth value checks
    auto subj = workSubjectEnsureExistance(fpth);
    if (!subj)
    {
        return nullptr;
    }
    return workSubjectExistingOrNewEditor(subj);
}

CodeEditorAbstract_shared ProjectCtl::workSubjectNewEditor(
    std::filesystem::path fpth
)
{
    std::cout << "workSubjectNewEditor(" << fpth << ")" << std::endl;
    // todo: fpth value checks
    auto subj = workSubjectEnsureExistance(fpth);
    if (!subj)
    {
        return nullptr;
    }

    return workSubjectNewEditor(subj);
}

CodeEditorAbstract_shared ProjectCtl::workSubjectExistingOrNewEditor(
    WorkSubject_shared val
)
{
    std::cout << "workSubjectExistingOrNewEditor(" << val << ")" << std::endl;
    for (unsigned int i = 0; i != editors_list_store->get_n_items(); i++)
    {
        auto x = editors_list_store->get_item(i);
        if (!x)
        {
            continue;
        }
        auto x_ed = x->editor;
        if (!x_ed)
        {
            continue;
        }
        if (x_ed->workSubjectIs(val))
        {
            return x_ed;
        }
    }

    return workSubjectNewEditor(val);
}

CodeEditorAbstract_shared ProjectCtl::workSubjectNewEditor(
    WorkSubject_shared val
)
{
    std::cout << "workSubjectNewEditor(" << val << ")" << std::endl;
    auto ed = createBestEditorForWorkSubject(val);
    std::cout << "createBestEditorForWorkSubject res: " << ed << std::endl;
    if (!ed)
    {
        return nullptr;
    }
    registerEditor(ed);
    ed->show();
    ed->present();
    return ed;
}

CodeEditorAbstract_shared ProjectCtl::createBestEditorForWorkSubject(
    WorkSubject_ptr subj
)
{
    std::cout << "createBestEditorForWorkSubject(" << subj << ")" << std::endl;

    // todo: plain text or hex viewver should be used by default
    CodeEditorMod *best_editor_mod = controller->getBuiltinMods()[0];

    auto subj_pth = subj->getPath().string();

    for (CodeEditorMod *x : controller->getBuiltinMods())
    {
        for (auto ext : x->supported_extensions)
        {
            if (subj_pth.ends_with(ext))
            {
                best_editor_mod = x;
                break;
            }
        }
    }

    auto editor = best_editor_mod->newEditorForSubject(own_ptr, subj);
    return editor;
}

void ProjectCtl::registerEditor(CodeEditorAbstract_shared val)
{
    // todo: register in Application?
    // todo: check is already exists
    std::cout << "registerEditor(" << val << ")" << std::endl;
    auto v    = CodeEditorTableRow::create();
    v->editor = val;
    editors_list_store->append(v);
    controller->registerWindow(std::dynamic_pointer_cast<Gtk::Window>(val));
}

void ProjectCtl::unregisterEditor(CodeEditorAbstract_shared val)
{
    // todo: redo or maybe even remove this function

    std::vector<CodeEditorAbstract_shared> vec;

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

void ProjectCtl::destroyEditor(CodeEditorAbstract_shared val)
{
    // todo: redo
    unregisterEditor(val);
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
        proj_ctl_win = ProjectCtlWin_shared(
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
