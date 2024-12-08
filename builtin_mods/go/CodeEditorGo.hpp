#ifndef WAYROUND_I2P_20241208_155643_196380
#define WAYROUND_I2P_20241208_155643_196380

#include <memory>

#include "../../CommonEditorWindow.hpp"

#include "../../utils.hpp"

namespace wayround_i2p::ccedit
{

void golang_menu_maker_cb(CommonEditorWindow *ed_win);
void golang_actions_maker_cb(CommonEditorWindow *ed_win);
void golang_hotkeys_maker_cb(CommonEditorWindow *ed_win);

std::tuple<std::string, int> golang_fmt_txt(
    std::string           txt,
    std::filesystem::path wd
);

CommonEditorWindow_shared createGolangEditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
);

} // namespace wayround_i2p::ccedit

#endif
