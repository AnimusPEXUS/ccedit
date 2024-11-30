

extern "C" {
#include <sys/wait.h>
#include <unistd.h>
}

#include <experimental/scope>

#include <format>
// #include <regex>
#include <thread>

#include <boost/regex.hpp>

#include "CodeEditorCCPP.hpp"

#include "../../Controller.hpp"

namespace wayround_i2p::ccedit
{

CommonEditorWindow_shared createCCPPeditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
)
{
    auto ed = CommonEditorWindow::create(
        project_ctl,
        subject,
        {
            menu_maker_cb : &ccpp_menu_maker_cb,
            actions_maker_cb : &ccpp_actions_maker_cb,
            hotkeys_maker_cb : &ccpp_hotkeys_maker_cb
        }
    );
    return ed;
}

void ccpp_menu_maker_cb(CommonEditorWindow *ed_win)
{
    auto mm_special_clang_format = Gio::MenuItem::create(
        "clang-format on buffer",
        "editor_window_special.clang_format_buffer"
    );

    auto mm_special = Gio::Menu::create();

    mm_special->append_item(mm_special_clang_format);

    auto target_menu = ed_win->getMenuModel();

    target_menu->append_submenu("C/C++", mm_special);
}

void ccpp_actions_maker_cb(CommonEditorWindow *ed_win)
{
    auto action_group = Gio::SimpleActionGroup::create();

    action_group->add_action(
        "clang_format_buffer",
        [ed_win]()
        {
            auto subj = ed_win->getWorkSubject();

            auto txt = subj->getText();

            // todo: check what dir is correctly calculated here and in
            //       Subject
            auto [ret, err] = clang_format_txt(
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

void ccpp_hotkeys_maker_cb(CommonEditorWindow *ed_win)
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
                "editor_window_special.clang_format_buffer"
            )
        )
    );

    ed_win->getWindowRef().add_controller(controller);
}

std::tuple<std::string, int> clang_format_txt(
    std::string           txt,
    std::filesystem::path wd
)
{
    int err = 0;

    int in_pipe[2];
    int out_pipe[2];

    err = pipe(in_pipe);
    if (err != 0)
    {
        return {"", 9};
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
        return {"", 8};
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
        return {"", 7};
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
        // todo: compiler says null is wrong here
        execlp("clang-format", (char *)NULL);
        return {"", 6};
    }

    if (pid != 0)
    {
        se01.release();
        se02.release();

        ::close(in_pipe[0]);
        ::close(out_pipe[1]);

        std::string str_to_write(txt);
        std::string str_to_read;

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
                std::string ret;

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
                    [&buff]()
                    {
                        delete[] buff;
                        buff = nullptr;
                    }
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

        return {"", 1};
    }

    return {"", 2};
}

/*
std::vector<std::tuple<std::size_t, std::string>>
    CodeEditorCCPP::genOutlineContents()
{

    std::vector<std::tuple<std::size_t, std::string>> ret;

    auto text_TextBuffer = subject->getTextBuffer();
    auto text            = subject->getText();

    std::vector<std::tuple<boost::regex, boost::regex_constants::match_flag_type>> rexps = {
        {boost::regex(
             R"%(^.*?(public|private|protected).*?\:)%",
         boost::regex_constants::ECMAScript
         ),
         boost::regex_constants::match_not_dot_newline},

        {boost::regex(
             R"%(^.*?(class|struct)(.|\n)*?[{;])%",
         boost::regex_constants::ECMAScript
         ),
         boost::regex_constants::match_not_dot_newline},
        {boost::regex(
             R"%(^\s*[a-zA-Z_0-9]+(<(.|\n)*?>)\((.|\n)*?\)(.|\n)*?[{;]?)%",
         boost::regex_constants::ECMAScript
         ),
         boost::regex_constants::match_not_dot_newline},

        // todo: next regex requires improvments
        // todo: clang-format settings also requires improvments
        {boost::regex(
             R"%((\@\w+\s*)?)%"
             R"%(((public|protected|private|abstract|static|final|strictfp)\s+)*)%"
             R"%(\w+\s*(?!(new|catch|if|for|while)\s+)\(.*?\).*?[{;])%",
         boost::regex_constants::ECMAScript
         ),
         boost::regex_constants::match_not_dot_newline}
    };

    auto match_end = boost::sregex_iterator();

    std::cout << "for" << std::endl;
    for (auto i = rexps.begin(); i != rexps.end(); i++)
    {
        std::cout << "iter" << std::endl;
        auto match_begin = boost::sregex_iterator(text.begin(), text.end(), std::get<0>(*i), std::get<1>(*i));
        for (auto j = match_begin; j != match_end; j++)
        {
            auto match     = *j;
            auto pos       = match.position();
            auto iter      = text_TextBuffer->get_iter_at_offset(pos);
            auto iter_line = iter.get_line();
            auto text      = match.str();

            ret.push_back({iter_line, text});
        }
    }

    return ret;
}
*/

} // namespace wayround_i2p::ccedit
