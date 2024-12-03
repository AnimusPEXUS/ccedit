#include "posix_interface.hpp"

#include <iostream>

#include <fnmatch.h>

namespace wayround_i2p::ccedit
{

// result:
//   1 - match/dismatch (bool)
//   2 - fnmatch error code
std::tuple<bool, int> fnmatch_simple(
    std::string pattern,
    std::string value
)
{
    std::cout << "fnmatch(" << pattern << ", " << value << ")" << std::endl;

    auto fn_res = ::fnmatch(
        pattern.c_str(),
        value.c_str(),
        0
    );

    std::cout << "   fn_res:" << fn_res << std::endl;

    switch (fn_res)
    {
        case 0:
            return {true, 0};
        case FNM_NOMATCH:
            return {false, 0};
        default:
            return {false, fn_res};
    }
}

} // namespace wayround_i2p::ccedit
