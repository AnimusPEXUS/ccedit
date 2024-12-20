#ifndef WAYROUND_I2P_20241210_171354_653180
#define WAYROUND_I2P_20241210_171354_653180

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "forward_declarations.hpp"
#include "signal.hpp"
#include "utils.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubject
{
  public:
    // path(fpth) may be absolute or relative, but must be inside
    // of project directory. internally path converted to relative.
    // second result non 0 on error.
    static std::tuple<WorkSubject_shared, int>
        create(
            ProjectCtl_shared     project_ctl,
            std::filesystem::path fpth
        );

  protected:
    WorkSubject(
        ProjectCtl_shared     project_ctl,
        std::filesystem::path fpth
    );

  public:
    ~WorkSubject();

    void destroy();

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

    sigc::signal<void()> &signal_editors_save_state();
    sigc::signal<void()> &signal_editors_restore_state();

    sigc::signal<void()> &signal_modified_changed();
    Signal<void()>       &signal_modified_changed2();

  private:
    RunOnce destroyer;

    WorkSubject_shared own_ptr;

    Controller_shared     controller;
    ProjectCtl_shared     project_ctl;
    std::filesystem::path fpth;

    Glib::RefPtr<Gtk::TextBuffer> txt_buff;

    sigc::signal<void()> priv_signal_editors_save_state;
    sigc::signal<void()> priv_signal_editors_restore_state;

    sigc::signal<void()> priv_signal_modified_changed;
    Signal<void()>       priv_signal_modified_changed2;

    void emit_signal_modified_changed();

    void sanitizeFilePath(std::filesystem::path &fpth);
};

} // namespace wayround_i2p::ccedit

#endif
