#include "module_ccpp.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    CodeEditorModule module_info_ccpp = {
        name : "CCPP",
        title : "C/C++",
        description : "C/C++ editor",
        supported_extensions : {".cpp", ".c", ".cc", ".hpp", ".h", ".hh"}
    };

    CodeEditorModule *get_module_info_ccpp()
    {
        return &module_info_ccpp;
    }

} // namespace codeeditor
} // namespace wayround_i2p
