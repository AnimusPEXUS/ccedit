#ifndef CODE_EDITOR_CCPP_HPP
#define CODE_EDITOR_CCPP_HPP

#include <memory>

#include "../../CommonEditorWindow.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class CodeEditorCCPP : public CommonEditorWindow
    {
      public:
        CodeEditorCCPP(
            std::shared_ptr<ProjectCtl>  project_ctl,
            std::shared_ptr<WorkSubject> subject
        );
        ~CodeEditorCCPP();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
