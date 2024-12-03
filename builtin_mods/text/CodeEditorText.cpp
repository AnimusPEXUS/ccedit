

#include <experimental/scope>

#include <format>

#include <thread>

#include <boost/regex.hpp>

#include "CodeEditorText.hpp"

#include "../../Controller.hpp"

namespace wayround_i2p::ccedit
{

CommonEditorWindow_shared createTextEditor(
    ProjectCtl_shared  project_ctl,
    WorkSubject_shared subject
)
{
    auto ed = CommonEditorWindow::create(
        project_ctl,
        subject,
        {}
    );
    return ed;
}

} // namespace wayround_i2p::ccedit
