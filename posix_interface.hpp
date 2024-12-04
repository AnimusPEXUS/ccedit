#ifndef WAYROUND_I2P_20241204_051018_602264
#define WAYROUND_I2P_20241204_051018_602264

#include <string>
#include <tuple>

namespace wayround_i2p::ccedit
{
std::tuple<bool, int> fnmatch_simple(
    std::string pattern,
    std::string value
);
}

#endif
