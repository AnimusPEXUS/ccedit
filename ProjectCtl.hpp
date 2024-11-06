#ifndef WAYROUND_I2P_20241106_133730_892452
#define WAYROUND_I2P_20241106_133730_892452

#include <filesystem>
#include <iostream>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"

#include "tables.hpp"

namespace wayround_i2p::ccedit
{

class ProjectCtl
{
  public:
    ProjectCtl(Controller_shared controller);

    ~ProjectCtl();

    ProjectCtl_shared own_ptr;

    Controller_shared getController();

    bool                  isGlobalProject();
    std::string           getProjectName();
    std::filesystem::path getProjectPath();

    // ----------

    bool workSubjectExists(std::filesystem::path fpth);

    // tries to find and return existing WorkSubject. result is empty if
    // there's no existing WS for fpath

    WorkSubject_shared getWorkSubject(std::filesystem::path fpth);

    // tries to find and return existing WorkSubject. if WS not exists already -
    // creates and returns new one

    WorkSubject_shared workSubjectEnsureExistance(
        std::filesystem::path fpth
    );

    // ----------

    // if subject not exists - create.
    // tries to find and return existing editor.

    CodeEditorAbstract_shared workSubjectExistingOrNewEditor(
        std::filesystem::path fpth
    );

    // if subject not exists - create.
    // allways create new editor for resulting subject

    CodeEditorAbstract_shared workSubjectNewEditor(
        std::filesystem::path fpth
    );

    // ----------

    // tries to find and return existing editor.

    CodeEditorAbstract_shared workSubjectExistingOrNewEditor(
        WorkSubject_shared
    );

    // allways create new editor for subject

    CodeEditorAbstract_shared workSubjectNewEditor(
        WorkSubject_shared
    );

    // ----------

    void destroyEditor(CodeEditorAbstract_shared);

    Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> getWorkSubjectListStore();
    Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  getCodeEditorListStore();

    void projectControllerRegisteredInController();

    void updatedName();
    void updatedPath();

    void showWindow();
    void closeWindow();

    void showNewFileExplorer();
    void showNewWorkSubjectList();
    void showNewEditorList();

    void close();

    std::shared_ptr<sigc::signal<void()>> signal_updated_name();
    std::shared_ptr<sigc::signal<void()>> signal_updated_path();

  private:
    Controller_shared controller;

    ProjectCtlWin_shared proj_ctl_win;

    CodeEditorAbstract_shared createBestEditorForWorkSubject(
        WorkSubject_shared
    );

    void registerEditor(CodeEditorAbstract_shared);
    void unregisterEditor(CodeEditorAbstract_shared);

    Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
    Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

    // todo: cant remember why I did those signals in form of ptr
    std::shared_ptr<sigc::signal<void()>> priv_signal_updated_name;
    std::shared_ptr<sigc::signal<void()>> priv_signal_updated_path;
};

} // namespace wayround_i2p::ccedit

#endif
