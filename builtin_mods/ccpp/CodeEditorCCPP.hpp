#ifndef WAYROUND_I2P_20241106_133730_899172
#define WAYROUND_I2P_20241106_133730_899172

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p::ccedit
{

class CodeEditorCCPP : public CommonEditorWindow
{
  public:
    static CodeEditorAbstract_shared create(
        ProjectCtl_shared  proj_ctl,
        WorkSubject_shared subj
    );

    ~CodeEditorCCPP();

    // void destroy();

    CodeEditorAbstract_shared getOwnPtr() override;

    WorkSubject_shared getWorkSubject() override;
    bool               workSubjectIs(WorkSubject_shared) override;

    std::vector<std::tuple<std::size_t, std::string>>
        genOutlineContents() override;

  protected:
    CodeEditorCCPP(
        ProjectCtl_shared  project_ctl,
        WorkSubject_shared subject
    );

  private:
    ProjectCtl_shared  project_ctl;
    WorkSubject_shared subject;

    std::shared_ptr<CodeEditorCCPP> own_ptr;

    Glib::RefPtr<Gio::Menu>     mm_special;
    Glib::RefPtr<Gio::MenuItem> mm_special_clang_format;

    void clang_format_buffer();

    void make_special_menu() override;
    void make_special_actions() override;
    void make_special_hotkeys() override;
};

} // namespace wayround_i2p::ccedit

#endif
