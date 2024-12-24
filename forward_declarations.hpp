#ifndef WAYROUND_I2P_20241223_224139_485205
#define WAYROUND_I2P_20241223_224139_485205

#include <memory>

namespace wayround_i2p::ccedit
{

// #include "forward_declarations.hpp"

#define GEN_FORV_DECL(name01)                        \
    class name01;                                    \
    using name01##_shared = std::shared_ptr<name01>; \
    using name01##_weak   = std::weak_ptr<name01>;

GEN_FORV_DECL(CodeEditorAbstract);
GEN_FORV_DECL(CommonEditorWindow);
GEN_FORV_DECL(Controller);
GEN_FORV_DECL(FileExplorer);
GEN_FORV_DECL(FileExplorerMakeFileDir);
GEN_FORV_DECL(FileExplorerWidget);
GEN_FORV_DECL(FindFile);
GEN_FORV_DECL(FindText);
GEN_FORV_DECL(FindTextWidget);
GEN_FORV_DECL(ProjectCtl);
GEN_FORV_DECL(ProjectCtlWin);
GEN_FORV_DECL(ProjectMgr);
GEN_FORV_DECL(ProjectMgrEditor);
GEN_FORV_DECL(ProjectSelectorForFile);
GEN_FORV_DECL(WorkSubject);

#undef GEN_FORV_DECL

} // namespace wayround_i2p::ccedit

#endif
