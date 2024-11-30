#ifndef WAYROUND_I2P_20241125_061830_592537
#define WAYROUND_I2P_20241125_061830_592537

#include <memory>

#include "../../CommonEditorWindow.hpp"

#include "../../utils.hpp"

namespace wayround_i2p::ccedit
{

class CodeEditorGo;
using CodeEditorGo_shared = std::shared_ptr<CodeEditorGo>;
using CodeEditorGo_weak   = std::weak_ptr<CodeEditorGo>;

class CodeEditorGo : public CommonEditorWindow
{
  public:
    static CodeEditorGo_shared create(
        ProjectCtl_shared  project_ctl,
        WorkSubject_shared subject
    );

  protected:
    CodeEditorGo(
        ProjectCtl_shared  project_ctl,
        WorkSubject_shared subject
    );

  public:
    ~CodeEditorGo();

    CodeEditorAbstract_shared getOwnPtr();

  private:
    CodeEditorGo_shared own_ptr;

    RunOnce destroyer;

    ProjectCtl_shared  project_ctl;
    WorkSubject_shared subject;


    Glib::RefPtr<Gio::Menu>     mm_special;
    Glib::RefPtr<Gio::MenuItem> mm_special_go_fmt;

    void go_fmt_buffer();

    void make_special_menu();
    void make_special_actions();
    void make_special_hotkeys();
};

} // namespace wayround_i2p::ccedit

#endif
