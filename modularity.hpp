#ifndef WAYROUND_I2P_20241210_171354_668036
#define WAYROUND_I2P_20241210_171354_668036

#include <functional>

#include <memory>
#include <string>

#include <deque>
#include <tuple>
#include <vector>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{
struct CodeEditorMod
{
    std::string             name;
    std::string             title;
    std::string             description;
    std::deque<std::string> tags;
    std::deque<std::string> supported_fn_masks;

    bool feature_text_editor;
    bool feature_binary_editor;

    std::function<
        CodeEditorAbstract_shared(
            ProjectCtl_shared  proj_ctl,
            WorkSubject_shared subj
        )>
        newEditorForSubject;
};

void printInfoCodeEditorMod(CodeEditorMod *info);

} // namespace wayround_i2p::ccedit

#endif
