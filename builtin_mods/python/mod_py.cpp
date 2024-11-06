
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"
#include "CodeEditorPy.hpp"
#include "mod_py.hpp"

namespace wayround_i2p ::ccedit
{

CodeEditorMod mod_info_ccpp = {
    name : "PyEdit",
    title : "Python",
    description : "Python editor",
    supported_extensions : {".py", ".pyx", ".pyd"},
    newEditorForSubject : &CodeEditorPy::create
};

CodeEditorMod *get_mod_info_py()
{
    return &mod_info_py;
}

} // namespace wayround_i2p::ccedit
