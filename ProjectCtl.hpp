#ifndef PROJECT_CTL_HPP
#define PROJECT_CTL_HPP

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

        bool                                   isGlobalProject();
        std::tuple<std::string, int>           getProjectName();
        std::tuple<std::filesystem::path, int> getProjectPath();

        bool workSubjectExists(std::filesystem::path fpth);
        void workSubjectEnsureExistance(std::filesystem::path fpth); // todo: return new WorkSubject?
        int  workSubjectNewEditor(std::filesystem::path fpth);       // todo: return new editor?
        std::tuple<
            std::shared_ptr<WorkSubject>,
            int>
            getWorkSubject(std::filesystem::path fpth);

        void unregisterEditor(CodeEditorAbstract *);
        void unregisterEditor(std::shared_ptr<CodeEditorAbstract>);

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

        Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
        Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

        std::shared_ptr<sigc::signal<void()>> priv_signal_updated_name;
        std::shared_ptr<sigc::signal<void()>> priv_signal_updated_path;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
