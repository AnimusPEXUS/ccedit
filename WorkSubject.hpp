#ifndef WORK_SUBJECT_HPP
#define WORK_SUBJECT_HPP

#include <filesystem>
#include <memory>
#include <string>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

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
        void                        modified(bool val);

        Glib::RefPtr<Gtk::TextBuffer> getTextBuffer();

        // todo:
        void getBinaryBuffer();

        std::string getText();
        void        setText(std::string txt);

        std::shared_ptr<sigc::signal<void()>> signal_modified_changed();

      private:
        std::shared_ptr<Controller> controller;
        std::shared_ptr<ProjectCtl> project_ctl;
        std::filesystem::path       fpth;

        Glib::RefPtr<Gtk::TextBuffer> txt_buff;

        // todo: make it unique_ptr instead of shared?
        std::shared_ptr<sigc::signal<void()>> priv_signal_modified_changed;

        void emit_signal_modified_changed();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
