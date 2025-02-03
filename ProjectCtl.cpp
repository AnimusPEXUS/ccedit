
#include <format>
#include <iostream>

#include "CodeEditorAbstract.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "FindFile.hpp"
#include "FindText.hpp"
#include "ProjectCtl.hpp"
#include "ProjectCtlWin.hpp"
#include "WorkSubject.hpp"

#include "posix_interface.hpp"

namespace wayround_i2p::ccedit
{

// todo: order functions

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
            std::cout << "ProjectCtl::destroyer.run()" << std::endl;
            this->controller->destroyProjCtl(this->own_ptr);
            this->destroyAllFileExplorer();
            this->destroyAllFindFile();
            this->destroyAllFindText();
            this->destroyAllEditors();
            this->destroyAllWorkSubjects();

            this->destroyWindow();

            this->own_ptr.reset();
        }
    )
{
    this->controller = controller;

    work_subj_list_store = Gio::ListStore<TableItemTpl<WorkSubject_shared>>::create();
    editors_list_store   = Gio::ListStore<TableItemTpl<CodeEditorAbstract_shared>>::create();

    updatedName();
    updatedPath();
}

ProjectCtl::~ProjectCtl()
{
    std::cout << "ProjectCtl::~ProjectCtl()" << std::endl;
    destroyer.run();
}

void ProjectCtl::destroy()
{
    // todo: close and destroy all subwindows and work subjects
    destroyer.run();
}

Controller_shared
    ProjectCtl::getController()
{
    return controller;
}

bool ProjectCtl::isGlobalProject()
{
    return controller->isGlobalProjCtl(own_ptr);
}

std::string
    ProjectCtl::getProjectName()
{
    auto [name, err] = controller->getNameProject(own_ptr);
    if (err != 0)
    {
        // todo: better action needed. at least message to user
        destroy();
        throw "error";
    }

    return name;
}

std::filesystem::path
    ProjectCtl::getProjectPath()
{
    auto [pth, err] = controller->getPathProject(own_ptr);
    if (err != 0)
    {
        // todo: better action needed. at least message to user
        // close();
        destroy();
        throw "error";
    }

    auto x = pth.lexically_normal();
    if (x.string() != pth.string())
    {
        // todo: check error?
        controller->editProject(getProjectName(), "", x);
    }

    return x;
}

std::tuple<
    std::filesystem::path,
    int>
    ProjectCtl::pathNormalizeAndTrimPrefix(std::filesystem::path fpth)
{
    if (fpth.begin() == fpth.end())
    {
        // fpth.begin() == fpth.end()
        return {"", 1};
    }

    std::filesystem::path result;

    auto fpth_first_val = (*(fpth.begin())).string();

    // std::cout << "fpth_first_val: " << fpth_first_val << std::endl;

    if (fpth_first_val == "..")
    {
        //  "not allowed to go outside of project path"
        return {"", 2};
    }

    if (fpth_first_val == "/")
    {
        // std::cout << R"--(fpth_first_val == "/")--" << std::endl;

        auto pp       = getProjectPath();
        auto pp_len   = std::distance(pp.begin(), pp.end());
        auto fpth_len = std::distance(fpth.begin(), fpth.end());
        if (fpth_len <= pp_len)
        {
            // "fpth_len <= pp_len";
            return {"", 3};
        }

        auto pp_it_b   = pp.begin();
        auto fpth_it_b = fpth.begin();

        for (std::size_t i = 0; i != pp_len; i++)
        {
            if ((*pp_it_b).string() != (*fpth_it_b).string())
            {
                // throw "(*pp_it_b).string() !=(*fpth_it_b).string()";
                return {"", 4};
            }
            pp_it_b++;
            fpth_it_b++;
        }

        // fpth_it_b = fpth.begin();
        while (true)
        {
            if (fpth_it_b == fpth.end())
            {
                break;
            }
            // std::cout << "result 1: " << result << std::endl;
            // std::cout << "  adding: " << *fpth_it_b << std::endl;
            result /= *fpth_it_b;
            // std::cout << "result 2: " << result << std::endl;
            ++fpth_it_b;
        }

        return {result, 0};
    }

    return {fpth, 0};
}

bool ProjectCtl::workSubjectExists(
    std::filesystem::path fpth
)
{
    return bool(getWorkSubject(fpth));
}

WorkSubject_shared
    ProjectCtl::getWorkSubject(
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
        auto sbj  = x->value;
        auto x_fp = sbj->getPath();
        if (x_fp == fpth)
        {
            return sbj;
        }
    }
    return nullptr;
}

WorkSubject_shared
    ProjectCtl::workSubjectEnsureExistance(
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
        auto [ws, err] = WorkSubject::create(
            own_ptr,
            fpth
        );
        if (err != 0)
        {
            // todo: report
            return nullptr;
        }
        ret = ws;
        ret->reload();
        return ret;
    }
}

CodeEditorAbstract_shared
    ProjectCtl::workSubjectExistingOrNewEditor(
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

CodeEditorAbstract_shared
    ProjectCtl::workSubjectNewEditor(
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

CodeEditorAbstract_shared
    ProjectCtl::workSubjectExistingOrNewEditor(
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
        auto x_ed = x->value;
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
    // registerEditor(ed);
    ed->show();
    return ed;
}

void ProjectCtl::editorShiftUpDown(
    CodeEditorAbstract_shared val,
    bool                      move_down
)
{
    auto [pos, err] = editorFindPos(val);
    if (err != 0)
    {
        return;
    }

    if ((!move_down) && pos == 0)
    {
        return;
    }

    if (move_down && pos == editorCount())
    {
        return;
    }

    auto o = editors_list_store->get_item(pos);
    editors_list_store->remove(pos);
    editors_list_store->insert(pos + (move_down ? 1 : -1), o);
}

void ProjectCtl::editorShiftUp(CodeEditorAbstract_shared val)
{
    editorShiftUpDown(val, false);
}

void ProjectCtl::editorShiftDown(CodeEditorAbstract_shared val)
{
    editorShiftUpDown(val, true);
}

void ProjectCtl::workSubjectMoveUp(WorkSubject_shared val)
{
    auto [pos, err] = workSubjectFindPos(val);
    if (err != 0)
    {
        return;
    }

    if (pos == 0)
    {
        return;
    }

    auto o = work_subj_list_store->get_item(pos);
    work_subj_list_store->remove(pos);
    work_subj_list_store->insert(0, o);
}

CodeEditorAbstract_shared
    ProjectCtl::createBestEditorForWorkSubject(WorkSubject_shared subj)
{
    std::cout << "createBestEditorForWorkSubject(" << subj << ")" << std::endl;

    // todo: plain text or hex viewver should be used by default
    CodeEditorMod *best_editor_mod = nullptr;

    auto subj_pth      = subj->getPath().string();
    auto subj_filename = subj->getPath().filename();

    for (CodeEditorMod *x : controller->getBuiltinMods())
    {
        for (auto msk : x->supported_fn_masks)
        {
            auto [ok, fn_res] = fnmatch_simple(msk, subj_filename);
            if (fn_res == 0 && ok)
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

void ProjectCtl::registerWorkSubject(WorkSubject_shared val)
{
    auto new_item   = TableItemTpl<WorkSubject_shared>::create();
    new_item->value = val;
    work_subj_list_store->append(new_item);
}

void ProjectCtl::unregisterWorkSubject(WorkSubject_shared val)
{
    std::size_t i = 0;
    while (i < work_subj_list_store->get_n_items())
    {
        auto x    = work_subj_list_store->get_item(i);
        auto x_we = x->value;
        if (x_we == val)
        {
            work_subj_list_store->remove(i);
            continue;
        }

        i++;
    }
}

void ProjectCtl::registerEditor(CodeEditorAbstract_shared val)
{
    // todo: register in Application?
    std::cout << "registerEditor(" << val << ")" << std::endl;

    std::size_t i = 0;
    while (i < editors_list_store->get_n_items())
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->value;
        if (x_ed == val)
        {
            return;
        }

        i++;
    }

    auto v   = TableItemTpl<CodeEditorAbstract_shared>::create();
    v->value = val;
    editors_list_store->append(v);
}

void ProjectCtl::unregisterEditor(CodeEditorAbstract_shared val)
{
    std::size_t i = 0;
    while (i < editors_list_store->get_n_items())
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->value;
        if (x_ed == val)
        {
            // x_ed->destroy();
            editors_list_store->remove(i);
            continue;
        }

        i++;
    }
}

void ProjectCtl::showPrevNextEditor(CodeEditorAbstract_shared val, bool prev)
{

    std::cout << "ProjectCtl::showPrevNextEditor "
              << val.get() << " " << prev << std::endl;

    std::size_t i = 0;
    while (i < editors_list_store->get_n_items())
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->value;
        if (x_ed == val)
        {
            std::cout << "   editor index " << i << std::endl;
            std::size_t j = 0;

            if (prev)
            {
                if (i == 0)
                {
                    return;
                }
                j = i - 1;
            }
            else
            {
                if (i >= editors_list_store->get_n_items() - 1)
                {
                    return;
                }
                j = i + 1;
            }

            auto y    = editors_list_store->get_item(j);
            auto y_ed = y->value;
            y_ed->show();
            return;
        }

        i++;
    }
    std::cout << "not found" << std::endl;
}

void ProjectCtl::showPrevEditor(CodeEditorAbstract_shared val)
{
    showPrevNextEditor(val, true);
}

void ProjectCtl::showNextEditor(CodeEditorAbstract_shared val)
{
    showPrevNextEditor(val, false);
}

void ProjectCtl::destroyWorkSubjectEditors(WorkSubject_shared val)
{
    std::deque<CodeEditorAbstract_shared> editors;

    for (std::size_t i = 0; i < editors_list_store->get_n_items(); i++)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->value;
        if (x_ed->workSubjectIs(val))
        {
            editors.push_back(x_ed);
        }
    }

    for (auto i : editors)
    {
        i->destroy();
    }
}

#define GEN_SUBWINDOW_FUNCTIONS(entity_name, entity_name_lower)        \
                                                                       \
    void ProjectCtl::destroyAll##entity_name()                         \
    {                                                                  \
        auto deq = entity_name_lower##_wins;                           \
        for (auto &i : deq)                                            \
        {                                                              \
            i->destroy();                                              \
        }                                                              \
    }                                                                  \
                                                                       \
    void ProjectCtl::register##entity_name(entity_name##_shared val)   \
    {                                                                  \
        for (auto &i : entity_name_lower##_wins)                       \
        {                                                              \
            if (i == val)                                              \
            {                                                          \
                return;                                                \
            }                                                          \
        }                                                              \
        entity_name_lower##_wins.push_back(val);                       \
    }                                                                  \
                                                                       \
    void ProjectCtl::unregister##entity_name(entity_name##_shared val) \
    {                                                                  \
        auto i = entity_name_lower##_wins.begin();                     \
                                                                       \
        while (i != entity_name_lower##_wins.end())                    \
        {                                                              \
            if (*i == val)                                             \
            {                                                          \
                i = entity_name_lower##_wins.erase(i);                 \
                continue;                                              \
            }                                                          \
                                                                       \
            ++i;                                                       \
        }                                                              \
    }

GEN_SUBWINDOW_FUNCTIONS(FileExplorer, fileexplorer);
GEN_SUBWINDOW_FUNCTIONS(FindFile, findfile);
GEN_SUBWINDOW_FUNCTIONS(FindText, findtext);

#undef GEN_SUBWINDOW_FUNCTIONS

void ProjectCtl::destroyAllEditors()
{
    std::deque<CodeEditorAbstract_shared> editors;

    for (std::size_t i = 0; i < editors_list_store->get_n_items(); i++)
    {
        auto x    = editors_list_store->get_item(i);
        auto x_ed = x->value;
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

    for (
        std::size_t i = 0;
        i < work_subj_list_store->get_n_items();
        i++
    )
    {
        auto x    = work_subj_list_store->get_item(i);
        auto x_ws = x->value;
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

Glib::RefPtr<Gio::ListStore<TableItemTpl<WorkSubject_shared>>>
    ProjectCtl::getWorkSubjectListStore()
{
    return work_subj_list_store;
}

Glib::RefPtr<Gio::ListStore<TableItemTpl<CodeEditorAbstract_shared>>>
    ProjectCtl::getCodeEditorListStore()
{
    return editors_list_store;
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
    }
    proj_ctl_win.reset();
}

FileExplorer_shared ProjectCtl::createNewFileExplorer()
{
    auto ret = FileExplorer::create(own_ptr);
    ret->show();
    return ret;
}

wayround_i2p::ccutils::signal::Signal<void()> &ProjectCtl::signal_updated_name()
{
    return priv_signal_updated_name;
}

wayround_i2p::ccutils::signal::Signal<void()> &ProjectCtl::signal_updated_path()
{
    return priv_signal_updated_path;
}

std::size_t ProjectCtl::workSubjectCount()
{
    return work_subj_list_store->get_n_items();
}

std::size_t ProjectCtl::editorCount()
{
    return editors_list_store->get_n_items();
}

std::tuple<std::size_t, int> ProjectCtl::workSubjectFindPos(WorkSubject_shared val)
{
    for (
        std::size_t i = 0;
        i < work_subj_list_store->get_n_items();
        i++
    )
    {
        auto x   = work_subj_list_store->get_item(i);
        auto sbj = x->value;
        if (val == sbj)
        {
            return {i, 0};
        }
    }
    return {0, -1};
}

std::tuple<std::size_t, int> ProjectCtl::editorFindPos(CodeEditorAbstract_shared val)
{
    for (
        std::size_t i = 0;
        i < editors_list_store->get_n_items();
        i++
    )
    {
        auto x   = editors_list_store->get_item(i);
        auto sbj = x->value;
        if (val == sbj)
        {
            return {i, 0};
        }
    }
    return {0, -1};
}

} // namespace wayround_i2p::ccedit
