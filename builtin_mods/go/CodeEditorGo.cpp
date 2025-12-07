

extern "C" {
#include <sys/wait.h>
#include <unistd.h>
}

#include <experimental/scope>

#include <format>
#include <thread>

#include <boost/regex.hpp>

#include "../../utils.hpp"

#include "CodeEditorGo.hpp"

namespace wayround_i2p::ccedit
{

CommonEditorWindow_shared createGolangEditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
)
{
    auto ed = CommonEditorWindow::create(
        project_ctl,
        subject,
        {
            menu_maker_cb : &golang_menu_maker_cb,
            actions_maker_cb : &golang_actions_maker_cb,
            hotkeys_maker_cb : &golang_hotkeys_maker_cb
        }
    );
    return ed;
}

void golang_menu_maker_cb(CommonEditorWindow *ed_win)
{
    auto mm_special_golang_format = Gio::MenuItem::create(
        "use gofmt on buffer",
        "editor_window_special.golang_fmt_txt"
    );

    auto mm_special = Gio::Menu::create();

    mm_special->append_item(mm_special_golang_format);

    auto target_menu = ed_win->getMenuModel();

    target_menu->append_submenu("Golang", mm_special);
}

void golang_actions_maker_cb(CommonEditorWindow *ed_win)
{
    auto action_group = Gio::SimpleActionGroup::create();

    action_group->add_action(
        "golang_fmt_txt",
        [ed_win]()
        {
            auto subj = ed_win->getWorkSubject();

            auto txt = subj->getText();

            // todo: check what dir is correctly calculated here and in
            //       Subject
            auto [ret, err] = golang_fmt_txt(
                txt,
                subj->getFullPath().parent_path()
            );
            if (err != 0)
            {
                // todo: report
                return;
            }

            subj->setText(ret);
            subj->modified(true);
        }
    );

    ed_win->getWindowRef().insert_action_group(
        "editor_window_special",
        action_group
    );
}

void golang_hotkeys_maker_cb(CommonEditorWindow *ed_win)
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
                "editor_window_special.golang_fmt_txt"
            )
        )
    );

    ed_win->getWindowRef().add_controller(controller);
}

std::tuple<std::string, int> golang_fmt_txt(
    std::string           txt,
    std::filesystem::path wd
)
{
    // todo: display error messages
    int err = 0;

    int in_pipe[2];
    int out_pipe[2];

    err = pipe(in_pipe);
    if (err != 0)
    {
        return {"", 5};
    }

    auto se01 = std::experimental::fundamentals_v3::scope_exit(
        [in_pipe]()
        {
            ::close(in_pipe[0]);
            ::close(in_pipe[1]);
        }
    );

    err = pipe(out_pipe);
    if (err != 0)
    {
        return {"", 4};
    }

    auto se02 = std::experimental::fundamentals_v3::scope_exit(
        [out_pipe]()
        {
            ::close(out_pipe[0]);
            ::close(out_pipe[1]);
        }
    );

    auto pid = fork();

    if (pid < 0)
    {
        return {"", 3};
    }

    if (pid == 0)
    {
        se01.release();
        se02.release();

        std::filesystem::current_path(wd);

        auto dup_r1 = dup2(in_pipe[0], 0);
        auto dup_r2 = dup2(out_pipe[1], 1);
        ::close(in_pipe[0]);
        ::close(in_pipe[1]);
        ::close(out_pipe[0]);
        ::close(out_pipe[1]);
        execlp("gofmt", "gofmt", (char *)NULL);
        return {"", 2};
    }

    if (pid != 0)
    {
        se01.release();
        se02.release();

        ::close(in_pipe[0]);
        ::close(out_pipe[1]);

        std::string str_to_write(txt);
        std::string str_to_read("");

        std::thread w_t(
            [str_to_write, in_pipe]()
            {
                auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                    [in_pipe]()
                    {
                        ::close(in_pipe[1]);
                    }
                );
                auto    str      = str_to_write.c_str();
                auto    str_size = str_to_write.size();
                ssize_t written  = 0;
                while (true)
                {
                    if (written == str_size)
                    {
                        return;
                    }
                    auto wr_res  = write(in_pipe[1], str + written, str_size - written);
                    written     += wr_res;
                }
            }
        );

        std::thread r_t(
            [&str_to_read, out_pipe]()
            {
                std::string ret("");

                auto sg03 = std::experimental::fundamentals_v3::scope_exit(
                    [&str_to_read, &ret]()
                    { str_to_read = ret; }
                );

                auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                    [out_pipe]()
                    { ::close(out_pipe[0]); }
                );

                char *buff = new char[256];

                auto sg02 = std::experimental::fundamentals_v3::scope_exit(
                    [buff]()
                    { delete buff; }
                );

                while (true)
                {
                    auto re_res = read(out_pipe[0], buff, 256);
                    if (re_res == 0)
                    {
                        return;
                    }
                    if (re_res == -1)
                    {
                        return;
                    }
                    ret.append(std::string(buff, re_res));
                }
            }
        );

        w_t.join();
        r_t.join();

        int exitstatus = 0;

        auto res = waitpid(pid, &exitstatus, 0);
        if (exitstatus == 0)
        {
            return {str_to_read, 0};
        }

        return {"", 2};
    }

    return {"", 1};
}

} // namespace wayround_i2p::ccedit
