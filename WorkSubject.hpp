#ifndef WAYROUND_I2P_20241106_133730_889735
#define WAYROUND_I2P_20241106_133730_889735

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubject
{
  public:
    // path should be always relative to project root
    static WorkSubject_shared create(
        Controller_shared     controller,
        ProjectCtl_shared     project_ctl,
        std::filesystem::path fpth
    );

    ~WorkSubject();

    Controller_shared     getController();
    ProjectCtl_shared     getProject();
    std::filesystem::path getPath();
    std::filesystem::path getFullPath();
    void                  createNew();
    int                   reload();
    int                   reload(bool allow_nonexist);
    int                   save();
    bool                  modified();
    void                  modified(bool val);

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
        Controller_shared     controller,
        ProjectCtl_shared     project_ctl,
        std::filesystem::path fpth
    );

  private:
    Controller_shared     controller;
    ProjectCtl_shared     project_ctl;
    std::filesystem::path fpth;

    Glib::RefPtr<Gtk::TextBuffer> txt_buff;

    std::shared_ptr<sigc::signal<void()>> priv_signal_editors_save_state;
    std::shared_ptr<sigc::signal<void()>> priv_signal_editors_restore_state;

    std::shared_ptr<sigc::signal<void()>> priv_signal_modified_changed;

    void emit_signal_modified_changed();
};

} // namespace wayround_i2p::ccedit

#endif
