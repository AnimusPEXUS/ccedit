#include <iostream>

#include "modularity.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    void printInfoCodeEditorMod(CodeEditorMod *info)
    {
        std::cout
            << "Mod Name: " << info->name << std::endl
            << "  Title    : " << info->title << std::endl

            ;
    }

} // namespace codeeditor
} // namespace wayround_i2p
