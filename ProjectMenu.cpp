#include "ProjectMenu.hpp"

#include "Controller.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p::ccedit
{

ProjectMenuGenerator::ProjectMenuGenerator(ProjectCtl_shared project_ctl) :
    project_ctl(project_ctl)
{
}

ProjectMenuGenerator::~ProjectMenuGenerator()
{
}

Glib::RefPtr<Gio::Menu> ProjectMenuGenerator::createProjectMenu(
    std::string group_name
)
{
    auto sect_m = Gio::Menu::create();

    auto sect_m0 = Gio::Menu::create();

    sect_m0->append(
        "New FileExplorer",
        group_name + ".action_create_new_explorer"
    );

    auto sect_m1 = Gio::Menu::create();

    sect_m1->append(
        "Project Manager",
        group_name + ".action_show_project_mgr"
    );
    sect_m1->append(
        "Project Controller",
        group_name + ".action_show_project_ctl"
    );

    auto sect_m2 = Gio::Menu::create();

    sect_m2->append(
        "Close Project's Work Subjects",
        group_name + ".action_close_project_work_subjects"
    );

    auto sect_m3 = Gio::Menu::create();

    sect_m3->append(
        "Close Project",
        group_name + ".action_close_project"
    );

    sect_m3->append(
        "Quit Program",
        group_name + ".action_close_ccedit"
    );

    sect_m->append_section(sect_m0);
    sect_m->append_section(sect_m1);
    sect_m->append_section(sect_m2);
    sect_m->append_section(sect_m3);

    return sect_m;
}

void ProjectMenuGenerator::addActionsToActionGroup(
    Glib::RefPtr<Gio::SimpleActionGroup> action_group
)
{
    action_group->add_action(
        "action_show_project_mgr",
        sigc::mem_fun(*this, &ProjectMenuGenerator::action_show_project_mgr)
    );

    action_group->add_action(
        "action_show_project_ctl",
        sigc::mem_fun(*this, &ProjectMenuGenerator::action_show_project_ctl)
    );

    action_group->add_action(
        "action_create_new_explorer",
        sigc::mem_fun(*this, &ProjectMenuGenerator::action_create_new_explorer)
    );

    action_group->add_action(
        "action_close_project_work_subjects",
        sigc::mem_fun(
            *this,
            &ProjectMenuGenerator::action_close_project_work_subjects
        )
    );

    action_group->add_action(
        "action_close_project",
        sigc::mem_fun(*this, &ProjectMenuGenerator::action_close_project)
    );

    action_group->add_action(
        "action_close_ccedit",
        sigc::mem_fun(*this, &ProjectMenuGenerator::action_close_ccedit)
    );
}

void ProjectMenuGenerator::action_show_project_mgr()
{
    project_ctl->getController()->showProjectMgr();
}

void ProjectMenuGenerator::action_show_project_ctl()
{
    project_ctl->showWindow();
}

void ProjectMenuGenerator::action_create_new_explorer()
{
    project_ctl->createNewFileExplorer();
}

void ProjectMenuGenerator::action_close_project_work_subjects()
{
    project_ctl->destroyAllWorkSubjects();
}

void ProjectMenuGenerator::action_close_project()
{
    project_ctl->destroy();
}

void ProjectMenuGenerator::action_close_ccedit()
{
    project_ctl->getController()->destroy();
}

} // namespace wayround_i2p::ccedit
