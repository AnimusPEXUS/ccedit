#include "ProjectMgrTables.hpp"

#include "ProjectCtl.hpp"

namespace wayround_i2p::ccedit
{

Glib::RefPtr<ProjectTableRow> ProjectTableRow::create()
{
    return Glib::make_refptr_for_instance<ProjectTableRow>(
        new ProjectTableRow()
    );
}

ProjectTableRow::ProjectTableRow() :
    Glib::ObjectBase(typeid(ProjectTableRow))
{
    //  priv_signal_proj_name_changed = std::shared_ptr<sigc::signal<void()>>(new sigc::signal<void()>());

    //  priv_signal_proj_path_changed = std::shared_ptr<sigc::signal<void()>>(new sigc::signal<void()>());
}

ProjectTableRow::~ProjectTableRow()
{
}

std::string ProjectTableRow::proj_name()
{
    return priv_proj_name;
}

void ProjectTableRow::proj_name(std::string val)
{
    priv_proj_name = val;
    priv_signal_proj_name_changed.emit();

    if (proj_ctl)
    {
        proj_ctl->updatedName();
    }
}

std::filesystem::path ProjectTableRow::proj_path()
{
    return priv_proj_path;
}

void ProjectTableRow::proj_path(std::filesystem::path val)
{
    priv_proj_path = val;
    priv_signal_proj_path_changed.emit();

    if (proj_ctl)
    {
        proj_ctl->updatedPath();
    }
}

sigc::signal<void()> &ProjectTableRow::signal_proj_name_changed()
{
    return priv_signal_proj_name_changed;
}

sigc::signal<void()> &ProjectTableRow::signal_proj_path_changed()
{
    return priv_signal_proj_path_changed;
}

} // namespace wayround_i2p::ccedit
