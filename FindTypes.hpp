#ifndef FIND_TYPES_HPP
#define FIND_TYPES_HPP

#include <filesystem>
#include <format>
#include <iostream>

#include <gtkmm.h>

#include <unicode/stringpiece.h>
#include <unicode/unistr.h>

namespace wayround_i2p
{
namespace codeeditor
{
    enum FindTextSearchMethod : unsigned char
    {
        INVALID, // this is for error reporting

        PLAIN,

        /* disabling, because as far as I know, std c++ re doesnt support unicode
STD_RE_ECMAScript,
STD_RE_BASIC,
STD_RE_EXTENDED,
STD_RE_AWK,
STD_RE_GREP,
STD_RE_EGREP,
*/

        BOOST_RE_ECMAScript,
        BOOST_RE_BASIC,
        BOOST_RE_EXTENDED,
        BOOST_RE_AWK,
        BOOST_RE_GREP,
        BOOST_RE_EGREP,
        BOOST_RE_SED,
        BOOST_RE_PERL,
        BOOST_RE_LITERAL
    };

    struct FindTextQuery
    {
        bool replace_visible = false;

        icu::UnicodeString query   = "";
        icu::UnicodeString replace = "";

        FindTextSearchMethod search_method          = PLAIN;
        bool                 stop_after_first_match = false;
        bool                 casesensitive          = false;

        // ----------
        bool std_re_mod_icase;
        bool std_re_mod_nosubs;
        bool std_re_mod_optimize;
        bool std_re_mod_collate;
        bool std_re_mod_multiline;

        // ----------
        bool boost_re_perl_mod_no_mod_m;
        bool boost_re_perl_mod_no_mod_s;
        bool boost_re_perl_mod_mod_s;
        bool boost_re_perl_mod_mod_x;

        // ----------
        bool boost_re_posix_mod_bk_plus_qm;
        bool boost_re_posix_mod_bk_vbar;
        bool boost_re_posix_mod_no_char_classes;
        bool boost_re_posix_mod_no_intervals;

        // ----------
        bool boost_re_common_mod_nosubs;
        bool boost_re_common_mod_optimize;
        bool boost_re_common_mod_collate;
        bool boost_re_common_mod_newline_alt;
        // bool boost_re_common_mod_no_except;
    };

    struct FindFileQuery
    {
        icu::UnicodeString    fnmatch_pattern          = "*";
        bool                  fnmatch_cs               = false;
        bool                  use_fnmatch_on_path_part = true;
        bool                  recurcive                = true;
        bool                  delve_into_hidden_dirs   = false;
        std::filesystem::path subpath                  = "/";
        bool                  use_max_depth            = false;
        unsigned short        max_depth                = 1;

        bool contents_search_enabled               = false;
        bool contents_search_cs                    = false;
        bool dont_show_files_with_0_contents_match = true;

        FindTextQuery find_text_query;
    };

} // namespace codeeditor
} // namespace wayround_i2p
#endif
