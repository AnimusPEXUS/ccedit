#ifndef WAYROUND_I2P_20241123_193654_655539
#define WAYROUND_I2P_20241123_193654_655539

#include <memory>

#include "../../CommonEditorWindow.hpp"

#include "../../utils.hpp"

namespace wayround_i2p::ccedit
{

class CodeEditorCCPP;
using CodeEditorCCPP_shared = std::shared_ptr<CodeEditorCCPP>;
using CodeEditorCCPP_weak   = std::weak_ptr<CodeEditorCCPP>;

class CodeEditorCCPP : public CommonEditorWindow
{
  public:
    static CodeEditorCCPP_shared create(
        ProjectCtl_shared  project_ctl,
        WorkSubject_shared subject
    );

  protected:
    CodeEditorCCPP(
        ProjectCtl_shared  project_ctl,
        WorkSubject_shared subject
    );

  public:
    ~CodeEditorCCPP();

    void destroy() override;

    CodeEditorAbstract_shared getOwnPtr() override;

    WorkSubject_shared getWorkSubject() override;
    bool               workSubjectIs(WorkSubject_shared) override;

    std::vector<std::tuple<std::size_t, std::string>>
        genOutlineContents() override;

  private:
    CodeEditorCCPP_shared own_ptr;

    RunOnce destroyer;

    ProjectCtl_shared  project_ctl;
    WorkSubject_shared subject;

    Glib::RefPtr<Gio::Menu>     mm_special;
    Glib::RefPtr<Gio::MenuItem> mm_special_clang_format;

    void clang_format_buffer();

    void make_special_menu() override;
    void make_special_actions() override;
    void make_special_hotkeys() override;
};

} // namespace wayround_i2p::ccedit

#endif
