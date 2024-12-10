#ifndef WAYROUND_I2P_20241210_171354_661154
#define WAYROUND_I2P_20241210_171354_661154

#include <filesystem>
#include <iostream>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"
#include "utils.hpp"

#include "ProjectCtlTables.hpp"

namespace wayround_i2p::ccedit
{

class ProjectCtl
{
  public:
    static ProjectCtl_shared create(Controller_shared controller);

    ~ProjectCtl();

  protected:
    ProjectCtl(Controller_shared controller);

  private:
    ProjectCtl_shared own_ptr;

    RunOnce destroyer;

  public:
    void destroy();

    Controller_shared getController();

    bool                  isGlobalProject();
    std::string           getProjectName();
    std::filesystem::path getProjectPath();

    // ----------

    // second parameter is error value. 0 is ok
    std::tuple<
        std::filesystem::path,
        int>
        pathNormalizeAndTrimPrefix(std::filesystem::path fpth);

    // ----------

    bool
        workSubjectExists(std::filesystem::path fpth);

    // tries to find and return existing WorkSubject. result is empty if
    // there's no existing WS for fpath

    WorkSubject_shared
        getWorkSubject(std::filesystem::path fpth);

    // tries to find and return existing WorkSubject. if WS not exists already -
    // creates and returns new one

    WorkSubject_shared
        workSubjectEnsureExistance(
            std::filesystem::path fpth
        );

    // ----------

    // if subject not exists - create.
    // tries to find and return existing editor.

    CodeEditorAbstract_shared
        workSubjectExistingOrNewEditor(
            std::filesystem::path fpth
        );

    // if subject not exists - create.
    // allways create new editor for resulting subject

    CodeEditorAbstract_shared
        workSubjectNewEditor(
            std::filesystem::path fpth
        );

    // ----------

    // tries to find and return existing editor.

    CodeEditorAbstract_shared
        workSubjectExistingOrNewEditor(
            WorkSubject_shared
        );

    // allways create new editor for subject

    CodeEditorAbstract_shared
        workSubjectNewEditor(
            WorkSubject_shared
        );

    // ----------

    void editorShiftUp(CodeEditorAbstract_shared val);
    void editorShiftDown(CodeEditorAbstract_shared val);

    // ----------

    void workSubjectMoveUp(WorkSubject_shared val);

    // ----------

    void
        destroyEditor(CodeEditorAbstract_shared);

    Glib::RefPtr<Gio::ListStore<TableItemTpl<WorkSubject_shared>>>
        getWorkSubjectListStore();
    Glib::RefPtr<Gio::ListStore<TableItemTpl<CodeEditorAbstract_shared>>>
        getCodeEditorListStore();

    void updatedName();
    void updatedPath();

    void showWindow();
    void destroyWindow();

    FileExplorer_shared
        createNewFileExplorer();

    CodeEditorAbstract_shared
        createBestEditorForWorkSubject(
            WorkSubject_shared
        );

    std::size_t workSubjectCount();
    std::size_t editorCount();

    std::tuple<std::size_t, int> workSubjectFindPos(WorkSubject_shared val);
    std::tuple<std::size_t, int> editorFindPos(CodeEditorAbstract_shared val);

    void destroyWorkSubjectEditors(WorkSubject_shared val);

    // void destroyAllExplorers();

    void destroyAllWorkSubjects();
    void destroyAllEditors();

    void registerWorkSubject(WorkSubject_shared val);
    void unregisterWorkSubject(WorkSubject_shared val);

    void registerEditor(CodeEditorAbstract_shared val);
    void unregisterEditor(CodeEditorAbstract_shared val);

#define GEN_SUBWINDOW_FUNCTIONS(entity_name, entity_name_lower) \
                                                                \
  public:                                                       \
    void destroyAll##entity_name();                             \
    void register##entity_name(entity_name##_shared val);       \
    void unregister##entity_name(entity_name##_shared val);     \
                                                                \
  private:                                                      \
    std::deque<entity_name##_shared> entity_name_lower##_wins;  \
                                                                \
  public:

    GEN_SUBWINDOW_FUNCTIONS(FileExplorer, fileexplorer);
    GEN_SUBWINDOW_FUNCTIONS(FindFile, findfile);
    GEN_SUBWINDOW_FUNCTIONS(FindText, findtext);

#undef GEN_SUBWINDOW_FUNCTIONS

    void showPrevNextEditor(CodeEditorAbstract_shared val, bool prev);
    void showPrevEditor(CodeEditorAbstract_shared val);
    void showNextEditor(CodeEditorAbstract_shared val);

    sigc::signal<void()> &signal_updated_name();
    sigc::signal<void()> &signal_updated_path();

  private:
    Controller_shared controller;

    ProjectCtlWin_shared proj_ctl_win;

    void editorShiftUpDown(
        CodeEditorAbstract_shared val,
        bool                      move_down
    );

    Glib::RefPtr<Gio::ListStore<TableItemTpl<WorkSubject_shared>>>        work_subj_list_store;
    Glib::RefPtr<Gio::ListStore<TableItemTpl<CodeEditorAbstract_shared>>> editors_list_store;

    sigc::signal<void()> priv_signal_updated_name;
    sigc::signal<void()> priv_signal_updated_path;
};

} // namespace wayround_i2p::ccedit

#endif
