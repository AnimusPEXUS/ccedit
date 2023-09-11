#include "CodeEditorCCPP.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    CodeEditorCCPP::CodeEditorCCPP(
        std::shared_ptr<ProjectCtl>  project_ctl,
        std::shared_ptr<WorkSubject> subject
    ) :
        CommonEditorWindow(project_ctl, subject)
    {

        make_special_menu();
        make_special_actions();
        make_special_hotkeys();
    }

    CodeEditorCCPP::~CodeEditorCCPP()
    {
        std::cout << "~CodeEditorCCPP()" << std::endl;
    }

    void CodeEditorCCPP::make_special_menu()
    {

        mm_special_clang_format = Gio::MenuItem::create(
            "clang-format on buffer",
            "editor_window_scpecial.clang_format_buffer"
        );

        mm_special = Gio::Menu::create();

        mm_special->append_item(mm_special_clang_format);

        getMenuModel()->append_submenu("C/C++", mm_special);

        return;
    }

    void CodeEditorCCPP::make_special_actions()
    {
        auto action_group = Gio::SimpleActionGroup::create();
        insert_action_group("editor_window_scpecial", action_group);
        return;
    }

    void CodeEditorCCPP::make_special_hotkeys()
    {
        auto controller = Gtk::ShortcutController::create();
        add_controller(controller);
        return;
    }

} // namespace codeeditor
} // namespace wayround_i2p
