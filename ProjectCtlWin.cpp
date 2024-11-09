
#include <format>

#include "Controller.hpp"
#include "EditorListView.hpp"
#include "FileExplorer.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubjectListView.hpp"

#include "ProjectCtlWin.hpp"

namespace wayround_i2p::ccedit
{

ProjectCtlWin_shared ProjectCtlWin::create(ProjectCtl_shared proj_ctl)
{
    auto ret     = ProjectCtlWin_shared(new ProjectCtlWin(proj_ctl));
    ret->own_ptr = ret;
    return ret;
}

ProjectCtlWin::ProjectCtlWin(ProjectCtl_shared proj_ctl) :
    destroyer(
        [this]()
        {
            std::cout << "ProjectCtlWin: destroyer.run()" << std::endl;
            controller->unregisterWindow(&win);
        }
    )
{

    this->proj_ctl   = proj_ctl;
    this->controller = proj_ctl->getController();

    show_file_explorer_btn.set_label("File Explorer");
    show_new_worksubject_list_btn.set_label("Work Subject List");
    show_new_editor_list_btn.set_label("Editor List");

    win.set_child(main_box);

    main_box.set_spacing(5);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(show_file_explorer_btn);
    main_box.append(show_new_worksubject_list_btn);
    main_box.append(show_new_editor_list_btn);

    show_file_explorer_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_show_file_explorer_btn)
    );

    show_new_worksubject_list_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_show_new_worksubject_list_btn)
    );

    show_new_editor_list_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_show_new_editor_list_btn)
    );

    proj_ctl->signal_updated_name()->connect(
        sigc::mem_fun(*this, &ProjectCtlWin::updateTitle)
    );

    win.signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectCtlWin::on_destroy_sig)
    );

    controller->registerWindow(&win);

    updateTitle();
}

ProjectCtlWin::~ProjectCtlWin()
{
    std::cout << "~ProjectCtlWin()" << std::endl;
    destroyer.run();
}

void ProjectCtlWin::on_show_file_explorer_btn()
{
    proj_ctl->showNewFileExplorer();
}

void ProjectCtlWin::on_show_new_worksubject_list_btn()
{
    proj_ctl->showNewWorkSubjectList();
}

void ProjectCtlWin::on_show_new_editor_list_btn()
{
    proj_ctl->showNewEditorList();
}

void ProjectCtlWin::on_destroy_sig()
{
    std::cout << "ProjectCtlWin::on_destroy_sig()" << std::endl;
    destroyer.run();
}

void ProjectCtlWin::updateTitle()
{
    std::string new_title;

    std::string proj_name("(global)");
    if (!proj_ctl->isGlobalProject())
    {
        proj_name = proj_ctl->getProjectName();
    }

    new_title = std::format(
        "{} - ProjCtl - Code Editor",
        proj_name
    );

    win.set_title(new_title);
}

void ProjectCtlWin::show()
{
    win.show();
}

void ProjectCtlWin::destroy()
{
    destroyer.run();
}

} // namespace wayround_i2p::ccedit
