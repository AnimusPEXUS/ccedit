#ifndef WAYROUND_I2P_20241204_051018_585474
#define WAYROUND_I2P_20241204_051018_585474

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class ProjectMenuGenerator
{
  public:
    ProjectMenuGenerator(ProjectCtl_shared project_ctl);
    ~ProjectMenuGenerator();

    Glib::RefPtr<Gio::Menu> createProjectMenu(
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
    void action_close_project_work_subjects();
    void action_close_project_editors();
    void action_close_project();
    void action_close_ccedit();
};

} // namespace wayround_i2p::ccedit

#endif
