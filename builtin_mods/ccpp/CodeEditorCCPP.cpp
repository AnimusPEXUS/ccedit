#include "CodeEditorCCPP.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    CodeEditorCCPP::CodeEditorCCPP(
        std::shared_ptr<ProjectCtl>  project_ctl,
        std::shared_ptr<WorkSubject> subject
    ) :
        CommonEditorWindow(project_ctl, subject)
    {
    }

    CodeEditorCCPP::~CodeEditorCCPP()
    {
        std::cout << "~CodeEditorCCPP()" << std::endl;
    }

} // namespace codeeditor
} // namespace wayround_i2p
