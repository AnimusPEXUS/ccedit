
#include "tables.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    std::string ProjectTableRow::proj_name()
    {
        return priv_proj_name;
    }

    void ProjectTableRow::proj_name(std::string val)
    {
        priv_proj_name = val;
        send_title_update_notification();
    }

    std::filesystem::path ProjectTableRow::proj_path()
    {
        return priv_proj_path;
    }

    void ProjectTableRow::proj_path(std::filesystem::path val)
    {
        priv_proj_path = val;
        send_title_update_notification();
    }

    void ProjectTableRow::send_title_update_notification()
    {
        if (proj_ctl)
        {
            proj_ctl->updateTitle();
        }
    }
} // namespace codeeditor
} // namespace wayround_i2p
