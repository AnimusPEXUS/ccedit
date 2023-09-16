#ifndef CODE_EDITOR_CCPP_HPP
#define CODE_EDITOR_CCPP_HPP

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class CodeEditorCCPP : public CommonEditorWindow
    {
      public:
        CodeEditorCCPP(std::shared_ptr<ProjectCtl> project_ctl, std::shared_ptr<WorkSubject> subject);
        ~CodeEditorCCPP();

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;

        Glib::RefPtr<Gio::Menu>     mm_special;
        Glib::RefPtr<Gio::MenuItem> mm_special_clang_format;

        void clang_format_buffer();

        void make_special_menu() override;
        void make_special_actions() override;
        void make_special_hotkeys() override;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
