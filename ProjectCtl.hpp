#ifndef WAYROUND_I2P_20241123_193654_648854
#define WAYROUND_I2P_20241123_193654_648854

#include <filesystem>
#include <iostream>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"

#include "tables.hpp"

#include "utils.hpp"

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

    void
        destroyEditor(CodeEditorAbstract_shared);

    Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>>
        getWorkSubjectListStore();
    Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>
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

    void destroyWorkSubjectEditors(WorkSubject_shared val);

    void destroyAllExplorers();

    void destroyAllWorkSubjects();
    void destroyAllEditors();

    void registerFileExplorer(FileExplorer_shared fe);
    void unregisterFileExplorer(FileExplorer_shared fe);

    void registerWorkSubject(WorkSubject_shared val);
    void unregisterWorkSubject(WorkSubject_shared val);

    void registerEditor(CodeEditorAbstract_shared val);
    void unregisterEditor(CodeEditorAbstract_shared val);

    void showPrevNextEditor(CodeEditorAbstract_shared val, bool prev);
    void showPrevEditor(CodeEditorAbstract_shared val);
    void showNextEditor(CodeEditorAbstract_shared val);

    sigc::signal<void()> &signal_updated_name();
    sigc::signal<void()> &signal_updated_path();

  private:
    Controller_shared controller;

    ProjectCtlWin_shared proj_ctl_win;

    std::deque<FileExplorer_shared>                   explorers;
    Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
    Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

    sigc::signal<void()> priv_signal_updated_name;
    sigc::signal<void()> priv_signal_updated_path;
};

} // namespace wayround_i2p::ccedit

#endif
