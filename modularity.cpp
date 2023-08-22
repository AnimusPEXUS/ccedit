#include <iostream>

#include "modularity.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    void printInfoCodeEditorModule(CodeEditorModule *info)
    {
        std::cout
            << "Module Name: " << info->name << std::endl
            << "  Title    : " << info->title << std::endl

            ;
    }

} // namespace codeeditor
} // namespace wayround_i2p
