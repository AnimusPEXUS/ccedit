
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"
#include "CodeEditorCCPP.hpp"
#include "mod_ccpp.hpp"

namespace wayround_i2p
{
namespace ccedit
{

    CodeEditorMod mod_info_ccpp = {
        name : "CCPP",
        title : "C/C++",
        description : "C/C++ editor",
        supported_fn_masks :
            {
               "*.cpp",
               "*.c",
               "*.cc",
               "*.hpp",
               "*.h",
               "*.hh"
            },
        newEditorForSubject : &createCCPPEditor
    };

    CodeEditorMod *get_mod_info_ccpp()
    {
        return &mod_info_ccpp;
    }

} // namespace ccedit
} // namespace wayround_i2p
