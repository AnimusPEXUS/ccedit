#ifndef WAYROUND_I2P_20241125_061830_589939
#define WAYROUND_I2P_20241125_061830_589939

#include <filesystem>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class ProjectTableRow : public Glib::Object
{
  public:
    std::string proj_name();
    void        proj_name(std::string);

    std::filesystem::path proj_path();
    void                  proj_path(std::filesystem::path);

    // Controller_shared controller;
    ProjectCtl_shared proj_ctl;

    static Glib::RefPtr<ProjectTableRow> create();

    sigc::signal<void()> &signal_proj_name_changed();
    sigc::signal<void()> &signal_proj_path_changed();

  protected:
    ProjectTableRow();

  public:
    ~ProjectTableRow();

  private:
    std::string           priv_proj_name;
    std::filesystem::path priv_proj_path;

    sigc::signal<void()> priv_signal_proj_name_changed;
    sigc::signal<void()> priv_signal_proj_path_changed;
};

} // namespace wayround_i2p::ccedit

#endif
