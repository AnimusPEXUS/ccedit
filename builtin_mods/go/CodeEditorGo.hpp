#ifndef WAYROUND_I2P_20240311_144846_119265
#define WAYROUND_I2P_20240311_144846_119265

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p
{
namespace ccedit
{
    class CodeEditorGo : public CommonEditorWindow
    {
      public:
        static std::shared_ptr<CodeEditorAbstract> create(
            std::shared_ptr<ProjectCtl>  proj_ctl,
            std::shared_ptr<WorkSubject> subj
        );

        ~CodeEditorGo();

        std::shared_ptr<CodeEditorAbstract> getOwnPtr() override;
        void                                resetOwnPtr() override;

        std::shared_ptr<WorkSubject> getWorkSubject() override;
        bool                         workSubjectIs(std::shared_ptr<WorkSubject>) override;

        // todo: is this good place for this?
        std::vector<std::tuple<unsigned int, std::string>>
            genOutlineContents() override;

      protected:
        CodeEditorGo(
            std::shared_ptr<ProjectCtl>  project_ctl,
            std::shared_ptr<WorkSubject> subject
        );

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;

        std::shared_ptr<CodeEditorGo> own_ptr;

        Glib::RefPtr<Gio::Menu>     mm_special;
        Glib::RefPtr<Gio::MenuItem> mm_special_go_fmt;

        void go_fmt_buffer();

        void make_special_menu() override;
        void make_special_actions() override;
        void make_special_hotkeys() override;
    };
} // namespace ccedit
} // namespace wayround_i2p

#endif
