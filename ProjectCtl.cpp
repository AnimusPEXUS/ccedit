
#include "ProjectCtl.hpp"
#include "Controller.hpp"
#include "FileExplorer.hpp"

using namespace wayround_i2p::codeeditor;

ProjectCtl::ProjectCtl(std::shared_ptr<Controller> controller)
{
    this->controller = controller;

    show_file_explorer_btn.set_label("File Explorer");

    set_child(show_file_explorer_btn);

    show_file_explorer_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectCtl::on_show_file_explorer_btn)
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

void ProjectCtl::registerFileExplorer(std::shared_ptr<FileExplorer> fe)
{
    auto i = explorer_windows.begin();
    while (i != explorer_windows.end())
    {
        if (fe == *i)
        {
            return;
        }
        ++i;
    }
    explorer_windows.push_back(fe);
}

void ProjectCtl::unregisterFileExplorer(FileExplorer *fe)
{
    auto i = explorer_windows.begin();
    while (i != explorer_windows.end())
    {
        if ((*i).get() == fe)
        {
            std::cout << " unregisterFileExplorer :: match found - deleting" << std::endl;
            i = explorer_windows.erase(i);
            std::cout << " unregisterFileExplorer :: match found - deleted" << std::endl;
            continue;
        }
        ++i;
    }
}

void ProjectCtl::unregisterFileExplorer(std::shared_ptr<FileExplorer> fe)
{
    unregisterFileExplorer(fe.get());
}

void ProjectCtl::on_show_file_explorer_btn()
{

    auto x = std::shared_ptr<FileExplorer>(
        new FileExplorer(this->own_ptr)
    );
    registerFileExplorer(x);
    x->show();
    // todo: mem leak?
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
