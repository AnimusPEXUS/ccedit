
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"
#include "CodeEditorGo.hpp"
#include "mod_go.hpp"

namespace wayround_i2p
{
namespace ccedit
{

    CodeEditorMod mod_info_go = {
        name : "Golang",
        title : "Golang",
        description : "Golang editor",
        supported_extensions : {".go"},
        newEditorForSubject : &CodeEditorGo::create
    };

    CodeEditorMod *get_mod_info_go()
    {
        return &mod_info_go;
    }

} // namespace ccedit
} // namespace wayround_i2p
