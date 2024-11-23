#include "ShowWindowMenu.hpp"

#include "Controller.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p::ccedit
{

WindowsMenuGenerator::WindowsMenuGenerator(ProjectCtl_shared project_ctl) :
    project_ctl(project_ctl)
{
}

WindowsMenuGenerator::~WindowsMenuGenerator()
{
}

Glib::RefPtr<Gio::Menu> WindowsMenuGenerator::createWindowsMenu(
    std::string group_name
)
{
    auto sect_m = Gio::Menu::create();

    auto sect_m1 = Gio::Menu::create();

    sect_m1->append(
        "Project Manager",
        group_name + ".action_show_project_mgr"
    );
    sect_m1->append(
        "Project Controller",
        group_name + ".action_show_project_ctl"
    );
    sect_m1->append(
        "Create New FileExplorer",
        group_name + ".action_create_new_explorer"
    );

    auto sect_m2 = Gio::Menu::create();

    sect_m2->append(
        "Close Project",
        group_name + ".action_close_project"
    );

    sect_m2->append(
        "Close ccedit",
        group_name + ".action_close_ccedit"
    );

    sect_m->append_section(sect_m1);
    sect_m->append_section(sect_m2);

    return sect_m;
}

void WindowsMenuGenerator::addActionsToActionGroup(
    Glib::RefPtr<Gio::SimpleActionGroup> action_group
)
{
    action_group->add_action(
        "action_show_project_mgr",
        sigc::mem_fun(*this, &WindowsMenuGenerator::action_show_project_mgr)
    );

    action_group->add_action(
        "action_show_project_ctl",
        sigc::mem_fun(*this, &WindowsMenuGenerator::action_show_project_ctl)
    );

    action_group->add_action(
        "action_create_new_explorer",
        sigc::mem_fun(*this, &WindowsMenuGenerator::action_create_new_explorer)
    );

    action_group->add_action(
        "action_close_project",
        sigc::mem_fun(*this, &WindowsMenuGenerator::action_close_project)
    );

    action_group->add_action(
        "action_close_ccedit",
        sigc::mem_fun(*this, &WindowsMenuGenerator::action_close_ccedit)
    );
}

void WindowsMenuGenerator::action_show_project_mgr()
{
    project_ctl->getController()->showProjectMgr();
}

void WindowsMenuGenerator::action_show_project_ctl()
{
    project_ctl->showWindow();
}

void WindowsMenuGenerator::action_create_new_explorer()
{
    project_ctl->createNewFileExplorer();
}

void WindowsMenuGenerator::action_close_project()
{
    project_ctl->destroy();
}

void WindowsMenuGenerator::action_close_ccedit()
{
    project_ctl->getController()->destroy();
}

} // namespace wayround_i2p::ccedit
