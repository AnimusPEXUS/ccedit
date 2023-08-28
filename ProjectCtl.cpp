
#include "ProjectCtl.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"
#include "WorkSubjectListView.hpp"

using namespace wayround_i2p::codeeditor;

ProjectCtl::ProjectCtl(std::shared_ptr<Controller> controller)
{
    this->controller = controller;

    show_file_explorer_btn.set_label("File Explorer");

    set_child(main_box);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(show_file_explorer_btn);
    main_box.append(show_new_worksubject_list_btn);

    show_file_explorer_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_file_explorer_btn)
    );

    show_new_worksubject_list_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_new_worksubject_list_btn)
    );

    signal_hide().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_hide_sig)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_destroy_sig)
    );
}

std::shared_ptr<Controller> ProjectCtl::getController()
{
    return controller;
}

void ProjectCtl::on_show_file_explorer_btn()
{
    auto x = new FileExplorer(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::on_show_new_worksubject_list_btn()
{
    auto x = new WorkSubjectListView(this->own_ptr);
    x->show();
    controller->getGtkApp()->add_window(*x);
}

void ProjectCtl::on_hide_sig()
{
    std::cout << "ProjectCtl sig hide" << std::endl;
}

void ProjectCtl::on_destroy_sig()
{
    std::cout << "ProjectCtl sig destroy" << std::endl;
    controller->cleanupGlobalProjCtl();
}
