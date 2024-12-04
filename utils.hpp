#ifndef WAYROUND_I2P_20241204_051018_571792
#define WAYROUND_I2P_20241204_051018_571792

#include <filesystem>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

#include <unicode/stringpiece.h>
#include <unicode/unistr.h>

#include "forward_declarations.hpp"

// todo: probably some of this should be moved to ccutils

namespace wayround_i2p::ccedit
{

const size_t size2mib = 1024 * 1024 * 2;

int saveStringToDesc(
    FILE       *f,
    std::string str
);

std::tuple<std::string, int> loadStringFromDesc(
    FILE *f
);

std::tuple<icu::UnicodeString, int> loadStringFromDescICU(
    FILE *f
);

int saveStringToFile(
    std::filesystem::path pth,
    std::string           str
);

int saveStringToFileICU(
    std::filesystem::path pth,
    icu::UnicodeString    str
);

// shortcut to loadStringFromFile() with allow_nonexist=false
std::tuple<std::string, int> loadStringFromFile(
    std::filesystem::path pth,
    bool                  allow_nonexist = false
);

std::tuple<icu::UnicodeString, int> loadStringFromFileICU(
    std::filesystem::path pth,
    bool                  allow_nonexist = false
);

class LineStarts
{
  public:
    LineStarts(std::string text);
    ~LineStarts() = default;

    // note: first line index is 1, not 0
    unsigned int getLineByOffset(unsigned int offset);

    // note: first line index is 1, not 0. 0 for index is invalid value
    std::tuple<
        unsigned int, // first char index
        unsigned int, // last char index
        int           // 0 if no error
        >
        getLineInfo(unsigned int index);

    void printParsingResult(std::string text);

  private:
    std::vector<unsigned int> starts;
    unsigned int              text_size = 0;
};

class LineStartsICU
{
  public:
    LineStartsICU(std::string text);
    LineStartsICU(icu::UnicodeString text);
    ~LineStartsICU() = default;

    // note: first line index is 1, not 0
    unsigned int getLineByOffset(unsigned int offset);

    // note: first line index is 1, not 0. 0 for index is invalid value
    std::tuple<
        unsigned int, // first char index
        unsigned int, // last char index
        int           // 0 if no error
        >
        getLineInfo(unsigned int index);

    void printParsingResult(icu::UnicodeString text);

  private:
    std::vector<unsigned int> starts;
    unsigned int              text_size = 0;
};

void mutexed_println(std::string s);

// this function makes sure, what this path is relative and it
// doesn't point to parrent dir:
// (lexically normalized pth does not starts with '..' or '.')
// result: 0 is ok. not 0 - not ok
int check_relpath_is_relative_and_sane(std::filesystem::path pth);

// removes space characters from right side of the string
std::string        trim_right(std::string);
icu::UnicodeString trim_right(icu::UnicodeString s);

// todo: add mutex?
// todo: move this to ccutils
struct RunOnce
{
  public:
    RunOnce(std::function<void()> f);
    ~RunOnce();

    void run();

  private:
    std::function<void()> f;

    bool runned = false;
};

} // namespace wayround_i2p::ccedit

#endif
