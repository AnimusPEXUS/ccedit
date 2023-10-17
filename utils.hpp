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

    int saveStringToDesc(
        FILE       *f,
        std::string str
    );

    std::tuple<std::string, int> loadStringFromDesc(
        FILE *f
    );

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

    void mutexed_println(std::string s);

    // this function makes sure, what this path is relative and it
    // doesn't point to parrent dir:
    // (lexically normalized pth does not starts with '..' or '.')
    // result: 0 is ok. not 0 - not ok
    int check_relpath_is_relative_and_sane(std::filesystem::path pth);

} // namespace codeeditor
} // namespace wayround_i2p

#endif
