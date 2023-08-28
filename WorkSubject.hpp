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
    class WorkSubject
    {
      public:
        // path should be always relative to project root
        WorkSubject(
            std::shared_ptr<Controller> controller,
            std::shared_ptr<ProjectCtl> project_ctl,
            std::filesystem::path       path
        );

        std::shared_ptr<Controller> getController();
        std::shared_ptr<ProjectCtl> getProject();
        std::filesystem::path       getPath();
        std::filesystem::path       getFullPath();
        void                        createNew();
        int                         load();
        int                         save();
        bool                        modified();

      private:
        std::shared_ptr<Controller> controller;
        std::shared_ptr<ProjectCtl> project_ctl;
        std::filesystem::path       path;

        Glib::RefPtr<Gtk::TextBuffer> buff;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
