#ifndef WORK_SUBJECT_HPP
#define WORK_SUBJECT_HPP

#include <filesystem>
#include <memory>
#include <string>

#include <gtkmm.h>

#include "Controller.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class Controller;

    class WorkSubject
    {
      public:
        // path should be always relative to project root
        WorkSubject(
            std::shared_ptr<Controller> controller,
            std::shared_ptr<ProjectCtl> project_ctl,
            std::filesystem::path       fpth
        );

        ~WorkSubject();

        std::shared_ptr<Controller> getController();
        std::shared_ptr<ProjectCtl> getProject();
        std::filesystem::path       getPath();
        std::filesystem::path       getFullPath();
        void                        createNew();
        int                         reload();
        int                         reload(bool allow_nonexist);
        int                         save();
        bool                        modified();

        Glib::RefPtr<Gtk::TextBuffer> getTextBuffer();

        // todo:
        void getBinaryBuffer();

      private:
        std::shared_ptr<Controller> controller;
        std::shared_ptr<ProjectCtl> project_ctl;
        std::filesystem::path       fpth;

        Glib::RefPtr<Gtk::TextBuffer> txt_buff;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
