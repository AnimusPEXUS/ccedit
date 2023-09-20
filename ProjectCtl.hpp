#ifndef PROJECT_CTL_HPP
#define PROJECT_CTL_HPP

#include <filesystem>
#include <iostream>
#include <tuple>

#include <gtkmm.h>

#include "CodeEditorAbstract.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "WorkSubject.hpp"
#include "tables.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class Controller;

    class FileExplorer;

    class ProjectCtl : public Gtk::Window
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

        void updateTitle();

      private:
        std::shared_ptr<Controller> controller;

        Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
        Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

        Gtk::Box main_box;

        Gtk::Button show_file_explorer_btn;
        Gtk::Button show_new_worksubject_list_btn;
        Gtk::Button show_new_editor_list_btn;

        void on_show_file_explorer_btn();
        void on_show_new_worksubject_list_btn();
        void on_show_new_editor_list_btn();

        void on_hide_sig();
        void on_destroy_sig();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
