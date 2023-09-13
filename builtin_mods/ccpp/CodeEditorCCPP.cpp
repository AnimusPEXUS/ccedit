#include <experimental/scope>

#include "CodeEditorCCPP.hpp"

extern "C"
{
#include <sys/wait.h>
#include <unistd.h>
}

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
        action_group->add_action(
            "clang_format_buffer",
            sigc::mem_fun(*this, &CodeEditorCCPP::clang_format_buffer)
        );
        insert_action_group("editor_window_scpecial", action_group);
        return;
    }

    void CodeEditorCCPP::make_special_hotkeys()
    {
        auto controller = Gtk::ShortcutController::create();
        controller->add_shortcut(
            Gtk::Shortcut::create(
                Gtk::KeyvalTrigger::create(
                    GDK_KEY_i,
                    Gdk::ModifierType::CONTROL_MASK
                        | Gdk::ModifierType::SHIFT_MASK
                ),
                Gtk::NamedAction::create(
                    "editor_window_scpecial.clang_format_buffer"
                )
            )
        );
        add_controller(controller);
        return;
    }

    void CodeEditorCCPP::clang_format_buffer()
    {
        std::cout << "format" << std::endl;

        int  err          = 0;
        bool fork_reached = false;

        int pipe_stdin[2];
        int pipe_stdout[2];

        err = pipe(pipe_stdin);
        if (err != 0)
        {
            return;
        }

        auto se01 = std::experimental::fundamentals_v3::scope_exit(
            [fork_reached, pipe_stdin]()
            {
                if (!fork_reached)
                {
                    ::close(pipe_stdin[0]);
                    ::close(pipe_stdin[1]);
                }
            }
        );

        err = pipe(pipe_stdout);
        if (err != 0)
        {
            return;
        }

        auto se02 = std::experimental::fundamentals_v3::scope_exit(
            [fork_reached, pipe_stdout]()
            {
                if (!fork_reached)
                {
                    ::close(pipe_stdout[0]);
                    ::close(pipe_stdout[1]);
                }
            }
        );

        auto pid = ::fork();
        switch (pid)
        {
        case -1:
            return;
        case 0:
        {
            fork_reached = true;
            auto dup_r1  = dup2(pipe_stdin[0], STDIN_FILENO);
            auto dup_r2  = dup2(pipe_stdout[1], STDOUT_FILENO);
            // todo: add stderr code?
            ::close(pipe_stdin[0]);
            ::close(pipe_stdin[1]);
            ::close(pipe_stdout[0]);
            ::close(pipe_stdout[1]);
            auto [pth, err] = getProjectCtl()->getProjectPath();
            if (err != 0)
            {
                // todo: err msg
                return;
            }
            std::filesystem::current_path(pth);
            ::execlp("clang-format", (char *)NULL);
            return;
        }
        default:
        {
            fork_reached = true;

            auto se03 = std::experimental::fundamentals_v3::scope_exit(
                [pipe_stdin, pipe_stdout, fork_reached]()
                {
                    if (!fork_reached)
                    {
                        ::close(pipe_stdin[1]);
                        ::close(pipe_stdin[0]);
                    }
                }
            );
            return;
        }
        }
    }

} // namespace codeeditor
} // namespace wayround_i2p
