#ifndef WAYROUND_I2P_20241123_193654_646730
#define WAYROUND_I2P_20241123_193654_646730

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class WindowsMenuGenerator
{
  public:
    WindowsMenuGenerator(ProjectCtl_shared project_ctl);
    ~WindowsMenuGenerator();

    Glib::RefPtr<Gio::Menu> createWindowsMenu(
        std::string group_name
    );

    void addActionsToActionGroup(
        Glib::RefPtr<Gio::SimpleActionGroup> action_group
    );

  private:
    ProjectCtl_shared project_ctl;

    void action_show_project_mgr();
    void action_show_project_ctl();
    void action_create_new_explorer();
    void action_close_project();
    void action_close_ccedit();
};

} // namespace wayround_i2p::ccedit

#endif
