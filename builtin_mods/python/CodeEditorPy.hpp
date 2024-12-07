#ifndef WAYROUND_I2P_20241208_155643_200123
#define WAYROUND_I2P_20241208_155643_200123

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p::ccedit
{

void python_menu_maker_cb(CommonEditorWindow *ed_win);
void python_actions_maker_cb(CommonEditorWindow *ed_win);
void python_hotkeys_maker_cb(CommonEditorWindow *ed_win);

std::tuple<std::string, int> autopep8_text(
    std::string           txt,
    std::filesystem::path wd
);

CommonEditorWindow_shared createPythonEditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
);

} // namespace wayround_i2p::ccedit

#endif
