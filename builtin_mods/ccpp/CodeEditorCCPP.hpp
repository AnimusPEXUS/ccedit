#ifndef WAYROUND_I2P_20241125_061830_594615
#define WAYROUND_I2P_20241125_061830_594615

#include <memory>

#include "../../CommonEditorWindow.hpp"

#include "../../utils.hpp"

namespace wayround_i2p::ccedit
{

void ccpp_menu_maker_cb(CommonEditorWindow *ed_win);
void ccpp_actions_maker_cb(CommonEditorWindow *ed_win);
void ccpp_hotkeys_maker_cb(CommonEditorWindow *ed_win);

std::tuple<std::string, int> clang_format_txt(
    std::string           txt,
    std::filesystem::path wd
);

CommonEditorWindow_shared createCCPPeditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
);

} // namespace wayround_i2p::ccedit

#endif
