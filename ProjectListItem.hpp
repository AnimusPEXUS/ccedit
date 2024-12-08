#ifndef WAYROUND_I2P_20241208_155643_184220
#define WAYROUND_I2P_20241208_155643_184220

// todo: is this file used at all?
//       merge it to Controller.hpp?

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{
class ProjectCtl;

struct ProjectListItem
{
    std::string       name;
    std::string       path;
    ProjectCtl_shared project_ctl;
};

} // namespace wayround_i2p::ccedit

#endif
