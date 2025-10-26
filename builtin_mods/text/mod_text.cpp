
#include <memory>
#include <tuple>

#include "../../CommonEditorWindow.hpp"

#include "CodeEditorText.hpp"

#include "mod_text.hpp"

namespace wayround_i2p::ccedit
{

CodeEditorMod mod_info_text = {
    name : "text",
    title : "Plain Text Editor",
    description : "Editor for editing plain texts",
    supported_fn_masks : {"*.txt", "*.md", "*.cmake", "README"},
    newEditorForSubject : &createTextEditor
};

CodeEditorMod *get_mod_info_text()
{
    return &mod_info_text;
}

} // namespace wayround_i2p::ccedit
