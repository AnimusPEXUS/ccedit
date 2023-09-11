
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"
#include "mod_ccpp.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    std::tuple<std::shared_ptr<CodeEditorAbstract>, int> create_window_ccpp(
        std::shared_ptr<ProjectCtl>  proj_ctl,
        std::shared_ptr<WorkSubject> subj
    )
    {
        // todo: maybe Controller should pass pointer to module_info_ccpp
        //       via create_window_ccpp() parameter
        auto x = std::shared_ptr<CodeEditorAbstract>(
            new CommonEditorWindow(proj_ctl, subj, get_mod_info_ccpp())
        );
        x->saveOwnPtr(x);
        return std::tuple(x, 0);
    }

    void setupTextView_ccpp(Gtk::TextView *view)
    {
    }

    CodeEditorMod mod_info_ccpp = {
        name : "CCPP",
        title : "C/C++",
        description : "C/C++ editor",
        supported_extensions : {".cpp", ".c", ".cc", ".hpp", ".h", ".hh"},
        newEditorForSubject : &create_window_ccpp,
        setupTextView : &setupTextView_ccpp
    };

    CodeEditorMod *get_mod_info_ccpp()
    {
        return &mod_info_ccpp;
    }

} // namespace codeeditor
} // namespace wayround_i2p
