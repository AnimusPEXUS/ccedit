#ifndef FIND_TEXT_HPP
#define FIND_TEXT_HPP

#include <future>

#include <gtkmm.h>

#include "utils.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    enum FindTextSearchMethod : unsigned char
    {
        INVALID, // this is for error reporting

        PLAIN,

        STD_RE_ECMAScript,
        STD_RE_BASIC,
        STD_RE_EXTENDED,
        STD_RE_AWK,
        STD_RE_GREP,
        STD_RE_EGREP,

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

    struct FindTextSettings
    {
        bool replace_visible = false;

        std::string query   = "";
        std::string replace = "";

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

    enum FindTextWidgetMode : unsigned char
    {
        SEARCH,
        EDIT
    };

    class FindTextWidget : public Gtk::Box
    {
      public:
        FindTextWidget(FindTextWidgetMode mode = SEARCH);
        ~FindTextWidget();

        FindTextSettings getSettings();
        void             setSettings(FindTextSettings settings);

        int search_in_text(
            const std::string           in_text,
            std::shared_ptr<LineStarts> in_text_ls,

            std::function<bool()> check_stop_flag,

            std::function<int(unsigned int line, std::string text)>
                here_s_new_occurance
        );

        void               setMode(FindTextWidgetMode mode = SEARCH);
        FindTextWidgetMode getMode();

      private:
        FindTextWidgetMode mode = SEARCH;

        Gtk::Frame query_frame;
        Gtk::Frame replace_frame;

        Gtk::Entry query;
        Gtk::Entry replace;

        // ----------
        // Gtk::Frame   plain_settings_frame;
        // Gtk::FlowBox plain_settings_box;

        Gtk::Frame       main_settings_frame;
        Gtk::FlowBox     main_settings_box;
        Gtk::DropDown    search_method;
        Gtk::CheckButton casesensitive;
        Gtk::CheckButton stop_after_first_match;

        // ----------
        Gtk::Frame       std_re_mod_frame;
        Gtk::FlowBox     std_re_mod_box;
        Gtk::CheckButton std_re_mod_icase;
        Gtk::CheckButton std_re_mod_nosubs;
        Gtk::CheckButton std_re_mod_optimize;
        Gtk::CheckButton std_re_mod_collate;
        Gtk::CheckButton std_re_mod_multiline;

        // ----------
        Gtk::Frame       boost_re_perl_mod_frame;
        Gtk::FlowBox     boost_re_perl_mod_box;
        Gtk::CheckButton boost_re_perl_mod_no_mod_m;
        Gtk::CheckButton boost_re_perl_mod_no_mod_s;
        Gtk::CheckButton boost_re_perl_mod_mod_s;
        Gtk::CheckButton boost_re_perl_mod_mod_x;

        // ----------
        Gtk::Frame       boost_re_posix_mod_frame;
        Gtk::FlowBox     boost_re_posix_mod_box;
        Gtk::CheckButton boost_re_posix_mod_bk_plus_qm;
        Gtk::CheckButton boost_re_posix_mod_bk_vbar;
        Gtk::CheckButton boost_re_posix_mod_no_char_classes;
        Gtk::CheckButton boost_re_posix_mod_no_intervals;

        // ----------
        Gtk::Frame       boost_re_common_mod_frame;
        Gtk::FlowBox     boost_re_common_mod_box;
        Gtk::CheckButton boost_re_common_mod_nosubs;
        Gtk::CheckButton boost_re_common_mod_optimize;
        Gtk::CheckButton boost_re_common_mod_collate;
        Gtk::CheckButton boost_re_common_mod_newline_alt;
        // Gtk::CheckButton boost_re_common_mod_no_except;

        FindTextSearchMethod getVisualSelectedFindTextSearchMethod();

        void setVisualSelectedFindTextSearchMethod(
            FindTextSearchMethod m
        );

        void setup_search_method_list();

        void apply_search_method_visual();

        void on_search_method_changed();
        void on_search_method_btn_clicked();
    };

    class TextSearchMethodListItem : public Glib::Object
    {
      public:
        static Glib::RefPtr<TextSearchMethodListItem> create(
            FindTextSearchMethod value
        )
        {
            auto ret = Glib::make_refptr_for_instance<TextSearchMethodListItem>(
                new TextSearchMethodListItem(value)
            );
            return ret;
        }

        const FindTextSearchMethod value;

      protected:
        TextSearchMethodListItem(FindTextSearchMethod value) :
            Glib::ObjectBase(typeid(TextSearchMethodListItem)),
            value(value)
        {
        }
    };

    class TextSearchMethodListItemWidget : public Gtk::Label
    {
      public:
        TextSearchMethodListItemWidget(
            const Glib::RefPtr<Gtk::ListItem> &list_item
        );
        ~TextSearchMethodListItemWidget();

        void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
