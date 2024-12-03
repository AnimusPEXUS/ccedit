
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"

#include "CodeEditorPy.hpp"

#include "mod_python.hpp"

namespace wayround_i2p::ccedit
{

CodeEditorMod mod_info_python = {
    name : "PyEdit",
    title : "Python",
    description : "Python editor",
    supported_fn_masks : {"*.py", "*.pyx", "*.pyd"},
    newEditorForSubject : &createPythonEditor
};

CodeEditorMod *get_mod_info_python()
{
    return &mod_info_python;
}

} // namespace wayround_i2p::ccedit
