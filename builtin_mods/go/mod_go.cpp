
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"
#include "CodeEditorGo.hpp"
#include "mod_go.hpp"

namespace wayround_i2p::ccedit
{

CodeEditorMod mod_info_go = {
    name : "Golang",
    title : "Golang",
    description : "Golang editor",
    supported_fn_masks : {"*.go"},
    newEditorForSubject : &createGolangEditor
};

CodeEditorMod *get_mod_info_go()
{
    return &mod_info_go;
}

} // namespace wayround_i2p::ccedit
