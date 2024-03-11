#ifndef WAYROUND_I2P_20240311_144846_114162
#define WAYROUND_I2P_20240311_144846_114162

#include <filesystem>
#include <iostream>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtlWin.hpp"
#include "WorkSubject.hpp"
#include "tables.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class Controller;
    class FileExplorer;
    class WorkSubjectTableRow;
    class CodeEditorTableRow;

    class ProjectCtl
    {
      public:
        ProjectCtl(std::shared_ptr<Controller> controller);

        ~ProjectCtl();

        std::shared_ptr<ProjectCtl> own_ptr;

        std::shared_ptr<Controller> getController();

        bool                  isGlobalProject();
        std::string           getProjectName();
        std::filesystem::path getProjectPath();

        // ----------

        bool workSubjectExists(std::filesystem::path fpth);

        // tries to find and return existing WorkSubject. result is empty if
        // there's no existing WS for fpath

        std::shared_ptr<WorkSubject> getWorkSubject(std::filesystem::path fpth);

        // tries to find and return existing WorkSubject. if WS not exists already -
        // creates and returns new one

        std::shared_ptr<WorkSubject> workSubjectEnsureExistance(
            std::filesystem::path fpth
        );

        // ----------

        // if subject not exists - create.
        // tries to find and return existing editor.

        std::shared_ptr<CodeEditorAbstract> workSubjectExistingOrNewEditor(
            std::filesystem::path fpth
        );

        // if subject not exists - create.
        // allways create new editor for resulting subject

        std::shared_ptr<CodeEditorAbstract> workSubjectNewEditor(
            std::filesystem::path fpth
        );

        // ----------

        // tries to find and return existing editor.

        std::shared_ptr<CodeEditorAbstract> workSubjectExistingOrNewEditor(
            std::shared_ptr<WorkSubject>
        );

        // allways create new editor for subject

        std::shared_ptr<CodeEditorAbstract> workSubjectNewEditor(
            std::shared_ptr<WorkSubject>
        );

        // ----------

        void destroyEditor(std::shared_ptr<CodeEditorAbstract>);

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
        std::shared_ptr<Controller> controller;

        std::shared_ptr<ProjectCtlWin> proj_ctl_win;

        std::shared_ptr<CodeEditorAbstract> createBestEditorForWorkSubject(
            std::shared_ptr<WorkSubject>
        );

        void registerEditor(std::shared_ptr<CodeEditorAbstract>);
        void unregisterEditor(std::shared_ptr<CodeEditorAbstract>);

        Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
        Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

        // todo: cant remember why I did those signals in form of ptr
        std::shared_ptr<sigc::signal<void()>> priv_signal_updated_name;
        std::shared_ptr<sigc::signal<void()>> priv_signal_updated_path;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
