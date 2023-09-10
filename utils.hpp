#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <string>
#include <tuple>

namespace wayround_i2p
{
namespace codeeditor
{
    const size_t size2mib = 1024 * 1024 * 2;

    int saveStringToFile(
        std::filesystem::path pth,
        std::string           str
    );

    // shortcut to loadStringFromFile() with allow_nonexist=false
    std::tuple<std::string, int> loadStringFromFile(
        std::filesystem::path pth
    );

    std::tuple<std::string, int> loadStringFromFile(
        std::filesystem::path pth,
        bool                  allow_nonexist
    );
} // namespace codeeditor
} // namespace wayround_i2p

#endif
