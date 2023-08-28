#include "WorkSubject.hpp"

using namespace wayround_i2p::codeeditor;

WorkSubject::WorkSubject(
    std::shared_ptr<Controller> controller,
    std::shared_ptr<ProjectCtl> project_ctl,
    std::filesystem::path       path
)
{
    this->controller  = controller;
    this->project_ctl = project_ctl;
    this->path        = path;

    createNew();
}

std::shared_ptr<Controller> WorkSubject::getController()
{
    return controller;
}

std::shared_ptr<ProjectCtl> WorkSubject::getProject()
{
    return project_ctl;
}

std::filesystem::path WorkSubject::getPath()
{
    return path;
}

std::filesystem::path WorkSubject::getFullPath()
{
    return path; // fixme
}

void WorkSubject::createNew()
{
    buff = Gtk::TextBuffer::create();
}

int WorkSubject::load()
{
    auto fp = getFullPath();

    if (!std::filesystem::exists(fp))
    {
        return 1;
    }

    // todo

    return 0;
}

int WorkSubject::save()
{
    return 1;
}

bool WorkSubject::modified()
{
    return 1;
}
