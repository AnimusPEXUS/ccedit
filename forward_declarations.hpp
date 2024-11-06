#ifndef WAYROUND_I2P_20241106_133730_891108
#define WAYROUND_I2P_20241106_133730_891108

#include <memory>

namespace wayround_i2p::ccedit
{

// #include "forward_declarations.hpp"

#define GEN_FORV_DECL(NAME)                      \
    class NAME;                                  \
    using NAME##_shared = std::shared_ptr<NAME>; \
    using NAME##_weak   = std::weak_ptr<NAME>;

GEN_FORV_DECL(CodeEditorAbstract);
GEN_FORV_DECL(CommonEditorWindow);
GEN_FORV_DECL(Controller);
GEN_FORV_DECL(EditorListView);
GEN_FORV_DECL(FindTextWidget);
GEN_FORV_DECL(FileExplorer);
GEN_FORV_DECL(FileExplorerWidget);
GEN_FORV_DECL(FindFile);
GEN_FORV_DECL(ProjectCtl);
GEN_FORV_DECL(ProjectCtlWin);
GEN_FORV_DECL(ProjectMgr);
GEN_FORV_DECL(ProjectMgrEditor);
GEN_FORV_DECL(WorkSubject);

#undef GEN_FORV_DECL

} // namespace wayround_i2p::ccedit

#endif
