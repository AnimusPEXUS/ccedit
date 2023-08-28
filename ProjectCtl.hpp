#ifndef PROJECT_CTL_HPP
#define PROJECT_CTL_HPP

#include <iostream>
#include <tuple>

#include <gtkmm.h>

#include "CodeEditorAbstract.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"
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

        ~ProjectCtl()
        {
            std::cout << "~ProjectCtl()" << std::endl;
        }

        std::shared_ptr<ProjectCtl> own_ptr;

        std::shared_ptr<Controller> getController();

        bool workSubjectExists(std::string path);
        int  workSubjectLoad(std::string path);      // todo: return new WorkSubject?
        int  workSubjectNewEditor(std::string path); // todo: return new editor?
        std::tuple<
            std::shared_ptr<CodeEditorAbstract>,
            int>
            getWorkSubject(std::string path);

      private:
        std::shared_ptr<Controller> controller;

        Glib::RefPtr<Gio::ListStore<WorkSubjectTableRow>> work_subj_list_store;
        Glib::RefPtr<Gio::ListStore<CodeEditorTableRow>>  editors_list_store;

        Gtk::Box main_box;

        Gtk::Button show_file_explorer_btn;
        Gtk::Button show_new_worksubject_list_btn;

        void on_show_file_explorer_btn();
        void on_show_new_worksubject_list_btn();

        void on_hide_sig();
        void on_destroy_sig();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
