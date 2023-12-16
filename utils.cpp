
#include <experimental/scope>
#include <format>
#include <iostream>
#include <mutex>

#include <boost/regex/icu.hpp>

#include "utils.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    int saveStringToFile(
        std::filesystem::path pth,
        std::string           str
    )
    {
        // todo: pth must always be absolute?
        // todo: ensure directory exists. create if needed.

        int err = 0;

        auto f = std::fopen(pth.c_str(), "w");
        if (err = std::ferror(f); err != 0)
        {
            return err;
        }
        auto se02 = std::experimental::fundamentals_v3::scope_exit(
            [f]()
            {
                // todo: remove debug code
                std::cout << "scope_exit 2" << std::endl;
                std::fclose(f);
            }
        );

        return saveStringToDesc(f, str);
    }

    int saveStringToFileICU(
        std::filesystem::path pth,
        icu::UnicodeString    str
    )
    {
        std::string x;
        return saveStringToFile(pth, str.toUTF8String(x));
    }

    std::tuple<icu::UnicodeString, int> loadStringFromFileICU(
        std::filesystem::path pth,
        bool                  allow_nonexist
    )
    {
        auto r = loadStringFromFile(pth, allow_nonexist);
        return std::tuple<icu::UnicodeString, int>(
            icu::UnicodeString::fromUTF8(icu::StringPiece(std::get<0>(r))),
            std::get<1>(r)
        );
    }

    std::tuple<std::string, int> loadStringFromFile(
        std::filesystem::path pth,
        bool                  allow_nonexist
    )
    {
        // todo: pth must always be absolute?
        // todo: mode tests?
        if (!std::filesystem::exists(pth))
        {
            if (allow_nonexist)
            {
                return std::tuple("", 0);
            }
            else
            {
                return std::tuple("", 1);
            }
        }

        int err = 0;

        auto f = std::fopen(pth.c_str(), "r");
        if (err = std::ferror(f); err != 0)
        {
            return std::tuple("", err);
        }
        auto se02 = std::experimental::fundamentals_v3::scope_exit(
            [f]()
            {
                // todo: remove debug code
                std::cout << "scope_exit 2" << std::endl;
                std::fclose(f);
            }
        );

        return loadStringFromDesc(f);
    }

    int saveStringToDescICU(
        FILE              *f,
        icu::UnicodeString str
    )
    {
        std::string x;
        return saveStringToDesc(
            f,
            str.toUTF8String(x)
        );
    }

    int saveStringToDesc(
        FILE       *f,
        std::string str
    )
    {

        int err = 0;

        auto str_c    = str.c_str();
        auto str_size = str.size();

        if (sizeof(str_c[0]) != 1)
        {
            throw std::length_error("unsupported char size: not 1");
        }

        auto offset = 0;

        while (true)
        {
            if (offset == str_size)
            {
                break;
            }
            auto length_minus_offset  = str_size - offset;
            auto count_to_write       = (size2mib <= length_minus_offset ? size2mib : length_minus_offset);
            auto writen_count         = std::fwrite(str_c + offset, sizeof(str_c[0]), count_to_write, f);
            offset                   += writen_count;
        }

        return 0;
    }

    std::tuple<icu::UnicodeString, int> loadStringFromDescICU(
        FILE *f
    )
    {
        auto r = loadStringFromDesc(f);
        return std::tuple<icu::UnicodeString, int>(
            icu::UnicodeString::fromUTF8(icu::StringPiece(std::get<0>(r))),
            std::get<1>(r)
        );
    }

    std::tuple<std::string, int> loadStringFromDesc(
        FILE *f
    )
    {
        int         err     = 0;
        int         err_eof = 0;
        std::string txt("");
        char       *buffer = new char[size2mib];

        if (sizeof(buffer[0]) != 1)
        {
            throw std::length_error("unsupported char size: not 1");
        }

        auto se01 = std::experimental::fundamentals_v3::scope_exit(
            [buffer]()
            {
                // todo: remove debug code
                std::cout << "scope_exit 1" << std::endl;
                delete buffer;
            }
        );

        std::cout << "while" << std::endl;
        while (true)
        {
            err     = std::ferror(f);
            err_eof = std::feof(f);

            // todo: remove debug code
            std::cout << "err: " << err << " err_eof: " << err_eof << std::endl;

            if (err != 0)
            {
                return std::tuple("", err);
            }

            if (err_eof != 0)
            {
                return std::tuple(txt, 0);
            }
            auto readed = std::fread(buffer, sizeof(buffer[0]), size2mib, f);
            txt.append(std::string(buffer, readed));
            // std::cout << "while2" << std::endl;
        }
    }

    void mutexed_println(std::string s)
    {
        static std::mutex mut;
        mut.lock();
        std::cout << s << std::endl;
        mut.unlock();
    }

    int check_relpath_is_relative_and_sane(std::filesystem::path pth)
    {
        // todo: task tracker should support 'recurring todo's
        // recurring todo: make sure path sanity functions are used
        //                 everywhere where needed

        if (!pth.is_relative())
        {
            return 2;
        }

        pth      = pth.lexically_normal();
        auto val = *(pth.begin());
        if (val == "." || val == "..")
        {
            return 1;
        }
        return 0;
    }

    LineStarts::LineStarts(std::string text)
    {
        auto match_end = std::sregex_iterator();
        auto nl_rex    = std::regex(R"x(\n)x");
        auto it        = std::sregex_iterator(text.begin(), text.end(), nl_rex);

        text_size = text.length();

        starts.push_back(0);

        for (;;)
        {
            if (it == match_end)
            {
                break;
            }

            auto z = *it;

            starts.push_back(z.position() + z.length());
            it++;
        }
    }

    LineStartsICU::LineStartsICU(std::string text) :
        LineStartsICU(icu::UnicodeString::fromUTF8(icu::StringPiece(text)))
    {
    }

    LineStartsICU::LineStartsICU(icu::UnicodeString text)
    {
        // todo: I don't like this part: "<const char16_t *>" - need to
        //       somehow forcibly make it 32bit (4 bytes) or even bigger
        auto match_end = boost::u32regex_iterator<const char16_t *>();
        auto nl_rex    = boost::make_u32regex(
            icu::UnicodeString::fromUTF8(
                icu::StringPiece(R"x(\n)x")
            )
        );
        auto it = boost::make_u32regex_iterator(text, nl_rex);

        text_size = text.length();

        starts.push_back(0);

        for (;;)
        {
            if (it == match_end)
            {
                break;
            }

            auto z = *it;

            starts.push_back(z.position() + z.length());
            it++;
        }
    }

    void LineStarts::printParsingResult(std::string text)
    {
        std::cout << "LineStarts : parsing result :" << std::endl;
        {
            for (int i = 0; i != starts.size(); i++)
            {
                auto j    = i + 1;
                auto info = getLineInfo(j);
                auto r0   = std::get<0>(info);
                auto r1   = std::get<1>(info);
                std::cout << format(
                    "{} ({}, {}): `{}`",
                    j,
                    r0,
                    r1,
                    trim_right(text.substr(r0, r1 - r0))
                ) << std::endl;
            }
        }
    }

    void LineStartsICU::printParsingResult(icu::UnicodeString text)
    {
        std::cout << "LineStartsICU : parsing result :" << std::endl;
        {
            for (int i = 0; i != starts.size(); i++)
            {
                auto        j    = i + 1;
                auto        info = getLineInfo(j);
                auto        r0   = std::get<0>(info);
                auto        r1   = std::get<1>(info);
                std::string sinc_buff("");

                icu::UnicodeString substr("");

                text.extract(r0, r1 - r0, substr);

                std::cout << std::format(
                    "{} ({}, {}): `{}`",
                    j,
                    r0,
                    r1,
                    trim_right(substr)
                        .toUTF8String(sinc_buff)
                ) << std::endl;
            }
        }
    }

    unsigned int LineStarts::getLineByOffset(unsigned int offset)
    {

        for (int i = 0; i != starts.size(); i++)
        {
            auto j    = i + 1;
            auto info = getLineInfo(j);
            auto r0   = std::get<0>(info);
            auto r1   = std::get<1>(info);
            if ((offset >= r0) && (offset < r1))
            {
                return j;
            }
        }

        return starts.size();
    }

    // todo: how to avoid code duplication here and in LineStarts::getLineByOffset?
    unsigned int LineStartsICU::getLineByOffset(unsigned int offset)
    {

        for (int i = 0; i != starts.size(); i++)
        {
            auto j    = i + 1;
            auto info = getLineInfo(j);
            auto r0   = std::get<0>(info);
            auto r1   = std::get<1>(info);
            if ((offset >= r0) && (offset < r1))
            {
                return j;
            }
        }

        return starts.size();
    }

    std::tuple<
        unsigned int,
        unsigned int,
        int>
        LineStarts::getLineInfo(unsigned int index)
    {
        unsigned int start = 0;
        unsigned int end   = 0;
        int          err   = 0;

        if (index == 0)
        {
            err = 3;
            goto return_res;
        }

        index -= 1; // because line 1 saved at index 0

        if (index > starts.size() - 1)
        {
            err = 1;
            goto return_res;
        }

        if (index == 0)
        {
            start = 0;
        }
        else
        {
            start = starts[index];
        }

        if (index == starts.size() - 1)
        {
            end = text_size;
        }
        else
        {
            end = starts[index + 1];
        }

    return_res:
        return std::tuple(start, end, err);
    }

    // todo: use template to create LineStarts::getLineInfo
    //       and LineStartsICU::getLineInfo?
    std::tuple<
        unsigned int,
        unsigned int,
        int>
        LineStartsICU::getLineInfo(unsigned int index)
    {
        unsigned int start = 0;
        unsigned int end   = 0;
        int          err   = 0;

        if (index == 0)
        {
            err = 3;
            goto return_res;
        }

        index -= 1; // because line 1 saved at index 0

        if (index > starts.size() - 1)
        {
            err = 1;
            goto return_res;
        }

        if (index == 0)
        {
            start = 0;
        }
        else
        {
            start = starts[index];
        }

        if (index == starts.size() - 1)
        {
            end = text_size;
        }
        else
        {
            end = starts[index + 1];
        }

    return_res:
        return std::tuple(start, end, err);
    }

    std::string trim_right(std::string s)
    {
        bool found = false;
        for (;;)
        {
            found = false;
            // todo: is this portable?
            for (
                std::string i :
                std::vector<std::string>{
                    "\t",
                    "\n",
                    "\r",
                    " ",
                    std::string("\0", 1)
                }
            )
            {
                while (s.ends_with(i))
                {
                    found = true;
                    s     = s.substr(0, s.length() - i.length());
                }
            }
            if (!found)
            {
                break;
            }
        }
        return s;
    }

    icu::UnicodeString trim_right(icu::UnicodeString s)
    {
        bool found = false;
        for (;;)
        {
            found = false;
            // todo: is this portable?
            for (
                icu::UnicodeString i :
                std::vector<icu::UnicodeString>{
                    "\t",
                    "\n",
                    "\r",
                    " ",
                    icu::UnicodeString("\0", 1)
                }
            )
            {
                while (s.endsWith(i))
                {
                    found = true;
                    icu::UnicodeString substr("");
                    s.extract(0, s.length() - i.length(), substr);
                    s = substr;
                }
            }
            if (!found)
            {
                break;
            }
        }
        return s;
    }

} // namespace codeeditor
} // namespace wayround_i2p
