#ifndef MODULARITY_HPP
#define MODULARITY_HPP

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "CodeEditorAbstract.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    struct CodeEditorModule
    {
        std::string              name;
        std::string              title;
        std::string              description;
        std::vector<std::string> tags;
        std::vector<std::string> supported_extensions;

        bool feature_text_editor;
        bool feature_binary_editor;

        std::tuple<std::shared_ptr<CodeEditorAbstract>, int> (*newEditorForSubject)(
            std::shared_ptr<ProjectCtl>  proj_ctl,
            std::shared_ptr<WorkSubject> subj
        );
    };

    void printInfoCodeEditorModule(CodeEditorModule *info);

} // namespace codeeditor
} // namespace wayround_i2p

#endif
