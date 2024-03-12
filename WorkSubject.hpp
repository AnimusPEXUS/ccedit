#ifndef WAYROUND_I2P_20240311_144846_112744
#define WAYROUND_I2P_20240311_144846_112744

#include <filesystem>
#include <memory>
#include <string>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "Controller.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace ccedit
{
    class Controller;

    class WorkSubject
    {
      public:
        // path should be always relative to project root
        static std::shared_ptr<WorkSubject> create(
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

        std::shared_ptr<sigc::signal<void()>> signal_editors_save_state();
        std::shared_ptr<sigc::signal<void()>> signal_editors_restore_state();

        std::shared_ptr<sigc::signal<void()>> signal_modified_changed();

      protected:
        WorkSubject(
            std::shared_ptr<Controller> controller,
            std::shared_ptr<ProjectCtl> project_ctl,
            std::filesystem::path       fpth
        );

      private:
        std::shared_ptr<Controller> controller;
        std::shared_ptr<ProjectCtl> project_ctl;
        std::filesystem::path       fpth;

        Glib::RefPtr<Gtk::TextBuffer> txt_buff;

        std::shared_ptr<sigc::signal<void()>> priv_signal_editors_save_state;
        std::shared_ptr<sigc::signal<void()>> priv_signal_editors_restore_state;

        std::shared_ptr<sigc::signal<void()>> priv_signal_modified_changed;

        void emit_signal_modified_changed();
    };

} // namespace ccedit
} // namespace wayround_i2p

#endif
