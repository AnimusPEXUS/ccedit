#ifndef WAYROUND_I2P_20241208_155643_203085
#define WAYROUND_I2P_20241208_155643_203085

#include <string>
#include <tuple>

namespace wayround_i2p::ccedit
{

// result:
//   1 - match/dismatch (bool)
//   2 - fnmatch error code
std::tuple<bool, int> fnmatch_simple(
    std::string pattern,
    std::string value
);

} // namespace wayround_i2p::ccedit

#endif
