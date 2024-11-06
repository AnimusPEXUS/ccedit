#ifndef WAYROUND_I2P_20241106_133730_894109
#define WAYROUND_I2P_20241106_133730_894109

#include <future>
#include <memory>

#include <gtkmm.h>

#include <unicode/stringpiece.h>
#include <unicode/unistr.h>

#include "forward_declarations.hpp"

#include "utils.hpp"

namespace wayround_i2p::ccedit
{

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

    FindTextQuery        getFindTextQuery();
    int                  setFindTextQuery(FindTextQuery q);
    FindTextSearchMethod getSearchMethod();
    int                  setSearchMethod(FindTextSearchMethod meth);

    int search_in_text(
        const icu::UnicodeString       in_text,
        std::shared_ptr<LineStartsICU> in_text_ls,

        std::function<bool()> check_stop_flag,

        std::function<
            int(
                unsigned int       line,
                icu::UnicodeString text,
                unsigned int       start_offset,
                unsigned int       end_offset
            )>
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

class FindText : public Gtk::Window
{
  public:
    static FindText_shared create(
        CodeEditorAbstract_weak editor_window
    );

    ~FindText();

    FindTextQuery getFindTextQuery();
    int           setFindTextQuery(FindTextQuery q);

  protected:
    FindText(
        CodeEditorAbstract_weak editor_window
    );

  private:
    FindText_shared own_ptr;

    CodeEditorAbstract_weak editor_window;
    unsigned int                      saved_cursor_line;

    Gtk::Box       main_box;
    Gtk::Expander  search_ex;
    Gtk::Box       search_box;
    FindTextWidget find_text_widget;

    Gtk::Box text_search_btn_box;
    Gtk::Box text_search_btn_box1;
    Gtk::Box text_search_btn_box2;

    Gtk::Button start_btn;
    Gtk::Button stop_btn;
    Gtk::Button acquire_cursor_position_btn;
    Gtk::Button restore_cursor_position_btn;

    Gtk::Frame          result_frame;
    Gtk::ScrolledWindow result_sw;
    Gtk::ListView       result_list_view;

    void setup_result_linelist();

    void saveEditorLine();

    FindTextQuery work_time_query;
    bool          search_stop_flag = false;
    bool          search_working   = false;
    int           start_search_thread();
    void          stop_search_thread();
    void          search_thread();

    void on_start_btn();
    void on_stop_btn();
    void on_acquire_cursor_position();
    void on_restore_cursor_position();

    void on_destroy_sig();
};

} // namespace wayround_i2p::ccedit

#endif
