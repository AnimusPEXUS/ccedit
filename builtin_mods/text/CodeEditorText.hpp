#ifndef WAYROUND_I2P_20241204_051018_592054
#define WAYROUND_I2P_20241204_051018_592054

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p::ccedit
{

void text_menu_maker_cb(CommonEditorWindow *ed_win);
void text_actions_maker_cb(CommonEditorWindow *ed_win);
void text_hotkeys_maker_cb(CommonEditorWindow *ed_win);

CommonEditorWindow_shared createTextEditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
);

} // namespace wayround_i2p::ccedit

#endif
