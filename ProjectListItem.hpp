#ifndef WAYROUND_I2P_20240311_144846_112522
#define WAYROUND_I2P_20240311_144846_112522

#include "ProjectCtl.hpp"

// todo: is this file used at all?
//       merge it to Controller.hpp?

namespace wayround_i2p
{
namespace codeeditor
{
    class ProjectCtl;

    struct ProjectListItem
    {
        std::string                 name;
        std::string                 path;
        std::shared_ptr<ProjectCtl> project_ctl;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
