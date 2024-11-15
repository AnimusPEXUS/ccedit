
#include <format>

#include "CodeEditorAbstract.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtl.hpp"
#include "ProjectCtlWin.hpp"
#include "WorkSubject.hpp"

using namespace wayround_i2p::ccedit;

ProjectCtl_shared ProjectCtl::create(Controller_shared controller)
{
    auto ret     = ProjectCtl_shared(new ProjectCtl(controller));
    ret->own_ptr = ret;
    return ret;
}

ProjectCtl::ProjectCtl(Controller_shared controller) :
    destroyer(
        [this]()
        {
            this->controller->destroyProjCtl(this->own_ptr);
            this->destroyAllEditors();
            this->destroyAllWorkSubjects();

            this->own_ptr.reset();
        }
    )
{
    this->controller = controller;

    work_subj_list_store = Gio::ListStore<WorkSubjectTableRow>::create();
    editors_list_store   = Gio::ListStore<CodeEditorTableRow>::create();
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
    return controller->isGlobalProjCtl(own_ptr);
}

std::string ProjectCtl::getProjectName()
{
    auto [name, err] = controller->getNameProject(own_ptr);
    if (err != 0)
    {
        // todo: better action needed. atlease message to user
        destroy();
    }

    return name;
}

std::filesystem::path ProjectCtl::getProjectPath()
{
    auto [pth, err] = controller->getPathProject(own_ptr);
    if (err != 0)
    {
        // todo: better action needed. at least message to user
        // close();
        destroy();
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
    for (
        std::size_t i = 0;
        i < work_subj_list_store->get_n_items();
        i++
    )
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

    if ((ret = getWorkSubject(fpth)))
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

CodeEditorAbstract_shared
    ProjectCtl::workSubjectNewEditor(WorkSubject_shared val)
{
    std::cout << "workSubjectNewEditor(" << val << ")" << std::endl;
    auto ed = createBestEditorForWorkSubject(val);
    std::cout << "  createBestEditorForWorkSubject res: " << ed << std::endl;
    if (!ed)
    {
        return nullptr;
    }
    registerEditor(ed);
    ed->show();
    ed->present();
    return ed;
}

CodeEditorAbstract_shared
    ProjectCtl::createBestEditorForWorkSubject(WorkSubject_shared subj)
{
    std::cout << "createBestEditorForWorkSubject(" << subj << ")" << std::endl;

    // todo: plain text or hex viewver should be used by default
    CodeEditorMod *best_editor_mod = nullptr;

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

    CodeEditorAbstract_shared ret;
    if (best_editor_mod)
    {
        ret = best_editor_mod->newEditorForSubject(own_ptr, subj);
    }
    else
    {
        // todo: here and/or before
    }

    return ret;
}

void ProjectCtl::registerEditor(CodeEditorAbstract_shared val)
{
    // todo: register in Application?
    // todo: check is already exists
    std::cout << "registerEditor(" << val << ")" << std::endl;
    auto v    = CodeEditorTableRow::create();
    v->editor = val;
    editors_list_store->append(v);
    controller->registerWindow(val->getWindowPtr());
}

void ProjectCtl::unregisterEditor(CodeEditorAbstract_shared val)
{
    std::size_t i = 0;
    while (i < editors_list_store->get_n_items())
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->editor;
        if (x_ed == val)
        {
            // x_ed->destroy();
            editors_list_store->remove(i);
            continue;
        }

        i++;
    }
}

void ProjectCtl::destroyAllEditors()
{
    std::deque<CodeEditorAbstract_shared> editors;

    for (std::size_t i = 0; i < editors_list_store->get_n_items(); i++)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->editor;
        editors.push_back(x_ed);
    }

    for (auto i : editors)
    {
        i->destroy();
    }
}

void ProjectCtl::destroyAllWorkSubjects()
{
    std::deque<WorkSubject_shared> wss;

    for (std::size_t i = 0; i < work_subj_list_store->get_n_items(); i++)
    {
        auto x    = work_subj_list_store->get_item(i);
        auto x_ws = x->work_subj;
        wss.push_back(x_ws);
    }

    for (auto i : wss)
    {
        i->destroy();
    }
}

void ProjectCtl::destroyEditor(CodeEditorAbstract_shared val)
{
    val->destroy();
}

Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> ProjectCtl::getWorkSubjectListStore()
{
    return work_subj_list_store;
}

Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>> ProjectCtl::getCodeEditorListStore()
{
    return editors_list_store;
}

void ProjectCtl::destroy()
{
    // todo: close and destroy all subwindows and work subjects
    destroyer.run();
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
    priv_signal_updated_name.emit();
}

void ProjectCtl::updatedPath()
{
    priv_signal_updated_path.emit();
}

void ProjectCtl::showWindow()
{
    if (!proj_ctl_win)
    {
        proj_ctl_win = ProjectCtlWin::create(own_ptr);
    }

    proj_ctl_win->show();

    return;
}

void ProjectCtl::destroyWindow()
{
    if (proj_ctl_win)
    {
        proj_ctl_win->destroy();
        // todo: try moving this inside ProjectCtlWin
        // proj_ctl_win.reset();
    }
}

void ProjectCtl::showNewFileExplorer()
{
    auto x = FileExplorer::create(own_ptr);
    x->show();
}

sigc::signal<void()> &ProjectCtl::signal_updated_name()
{
    return priv_signal_updated_name;
}

sigc::signal<void()> &ProjectCtl::signal_updated_path()
{
    return priv_signal_updated_path;
}
