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
        static std::shared_ptr<CodeEditorAbstract> create(
            std::shared_ptr<ProjectCtl>  proj_ctl,
            std::shared_ptr<WorkSubject> subj
        );

        ~CodeEditorCCPP();

        std::shared_ptr<CodeEditorAbstract> getOwnPtr() override;
        void                                resetOwnPtr() override;

        // todo: is this good place for this?
        std::vector<std::tuple<unsigned int, std::string>>
            genOutlineContents() override;

      protected:
        CodeEditorCCPP(
            std::shared_ptr<ProjectCtl>  project_ctl,
            std::shared_ptr<WorkSubject> subject
        );

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;

        std::shared_ptr<CodeEditorCCPP> own_ptr;

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
