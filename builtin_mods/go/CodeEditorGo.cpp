

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

CodeEditorGo_shared create(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
)
{
    auto ret = CodeEditorGo_shared(
        new CodeEditorGo(proj_ctl, subj)
    );
    ret->own_ptr = ret;

    // todo: move this to constructor? also somehow need to move this to
    //       CommonEditor or into some better place
    project_ctl->registerEditor(ret);
    project_ctl->getController()->registerWindow(ret->getWindowPtr());

    return ret;
}

CodeEditorGo::CodeEditorGo(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
) :
    CommonEditorWindow(
        project_ctl,
        subject,
        [this]()
        {
            std::cout << "CodeEditorGo::CommonEditorWindow::destroyer_cb"
                      << std::endl;
            destroyer.run();
        }
    ),
    destroyer(
        [this]()
        {
            std::cout << "CodeEditorGo::destroyer.run()" << std::endl;
            this->project_ctl->unregisterEditor(
                this->own_ptr
            );
            CommonEditorWindow::destroy();
            own_ptr.reset();
        }
    )
{

    this->project_ctl = project_ctl;
    this->subject     = subject;

    make_special_menu();
    make_special_actions();
    make_special_hotkeys();
}

CodeEditorGo::~CodeEditorGo()
{
    std::cout << "CodeEditorGo::~CodeEditorGo()" << std::endl;
    destroyer.run();
}

void CodeEditorGo::destroy()
{
    std::cout << "CodeEditorGo::destroy()" << std::endl;
    destroyer.run();
}

CodeEditorAbstract_shared CodeEditorGo::getOwnPtr()
{
    return dynamic_pointer_cast<CodeEditorAbstract>(own_ptr);
}

void CodeEditorGo::make_special_menu()
{

    mm_special_go_fmt = Gio::MenuItem::create(
        "gofmt on buffer",
        "editor_window_special.go_fmt_buffer"
    );

    mm_special = Gio::Menu::create();

    mm_special->append_item(mm_special_go_fmt);

    getMenuModel()->append_submenu("Golang", mm_special);

    return;
}

void CodeEditorGo::make_special_actions()
{
    auto action_group = Gio::SimpleActionGroup::create();
    action_group->add_action(
        "go_fmt_buffer",
        sigc::mem_fun(*this, &CodeEditorGo::go_fmt_buffer)
    );
    getWindowRef().insert_action_group(
        "editor_window_special",
        action_group
    );
    return;
}

void CodeEditorGo::make_special_hotkeys()
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
                "editor_window_special.go_fmt_buffer"
            )
        )
    );
    add_controller(controller);
    return;
}

std::shared_ptr<WorkSubject> CodeEditorGo::getWorkSubject()
{
    return subject;
}

bool CodeEditorGo::workSubjectIs(std::shared_ptr<WorkSubject> subj)
{
    if (!subj || !subject)
    {
        return false;
    }
    return subj == subject;
}

void CodeEditorGo::go_fmt_buffer()
{
    // todo: display error messages
    int err = 0;

    int in_pipe[2];
    int out_pipe[2];

    auto pp = subject->getFullPath().parent_path();

    /*
        auto [proj_pth, err] = project_ctl->getProjectPath();
        if (err != 0)
        {
            return;
        }
        */

    err = pipe(in_pipe);
    if (err != 0)
    {
        return;
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
        return;
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
        return;
    }

    if (pid == 0)
    {
        se01.release();
        se02.release();

        std::filesystem::current_path(pp);

        auto dup_r1 = dup2(in_pipe[0], 0);
        auto dup_r2 = dup2(out_pipe[1], 1);
        ::close(in_pipe[0]);
        ::close(in_pipe[1]);
        ::close(out_pipe[0]);
        ::close(out_pipe[1]);
        execlp("gofmt", (char *)NULL);
        return;
    }

    if (pid != 0)
    {
        se01.release();
        se02.release();

        ::close(in_pipe[0]);
        ::close(out_pipe[1]);

        std::string str_to_write(subject->getText());
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
            subject->setText(str_to_read);
            subject->modified(true);
        }

        return;
    }

    return;
}

std::vector<std::tuple<unsigned int, std::string>>
    CodeEditorGo::genOutlineContents()
{

    std::vector<std::tuple<unsigned int, std::string>> ret;

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

} // namespace wayround_i2p::ccedit
