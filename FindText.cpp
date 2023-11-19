
#include <iostream>

#include "FindText.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    FindTextWidget::FindTextWidget(FindTextWidgetMode mode) :
        Box(Gtk::Orientation::VERTICAL, 5)
    {

        query.set_margin(5);
        replace.set_margin(5);

        query_frame.set_child(query);
        replace_frame.set_child(replace);

        query_frame.set_label("Query");
        replace_frame.set_label("Replace");

        append(query_frame);
        append(replace_frame);

        main_settings_frame.set_child(main_settings_box);
        main_settings_box.append(search_mode);
        main_settings_box.append(casesensitive);
        main_settings_box.append(stop_after_first_match);
        append(main_settings_frame);

        stop_after_first_match.set_label("1 Content Match is Enough");
        stop_after_first_match.set_tooltip_text(
            "stop grepping current file and proceed to next"
        );
        casesensitive.set_label("Case Sensitive");

        std_re_mod_icase.set_label("icase");
        std_re_mod_nosubs.set_label("nosubs");
        std_re_mod_optimize.set_label("optimize");
        std_re_mod_collate.set_label("collate");
        std_re_mod_multiline.set_label("multiline");

        std_re_mod_frame.set_label("std C++ lib regexp settings");
        std_re_mod_frame.set_child(std_re_mod_box);
        std_re_mod_box.set_selection_mode(Gtk::SelectionMode::NONE);
        std_re_mod_box.append(std_re_mod_icase);
        std_re_mod_box.append(std_re_mod_nosubs);
        std_re_mod_box.append(std_re_mod_optimize);
        std_re_mod_box.append(std_re_mod_collate);
        std_re_mod_box.append(std_re_mod_multiline);
        append(std_re_mod_frame);

        // ----------

        boost_re_common_mod_nosubs.set_label("nosubs");
        boost_re_common_mod_optimize.set_label("optimize");
        boost_re_common_mod_collate.set_label("collate");
        boost_re_common_mod_newline_alt.set_label("newline_alt");

        boost_re_common_mod_frame.set_label("boost lib Common regexp settings");
        boost_re_common_mod_frame.set_child(boost_re_common_mod_box);
        boost_re_common_mod_box.set_selection_mode(Gtk::SelectionMode::NONE);
        boost_re_common_mod_box.append(boost_re_common_mod_nosubs);
        boost_re_common_mod_box.append(boost_re_common_mod_optimize);
        boost_re_common_mod_box.append(boost_re_common_mod_collate);
        boost_re_common_mod_box.append(boost_re_common_mod_newline_alt);
        append(boost_re_common_mod_frame);

        // ----------

        boost_re_perl_mod_no_mod_m.set_label("no_mod_m");
        boost_re_perl_mod_no_mod_s.set_label("no_mod_s");
        boost_re_perl_mod_mod_s.set_label("mod_s");
        boost_re_perl_mod_mod_x.set_label("mod_x");

        boost_re_perl_mod_frame.set_label("boost lib Perl regexp settings");
        boost_re_perl_mod_frame.set_child(boost_re_perl_mod_box);
        boost_re_perl_mod_box.set_selection_mode(Gtk::SelectionMode::NONE);
        boost_re_perl_mod_box.append(boost_re_perl_mod_no_mod_m);
        boost_re_perl_mod_box.append(boost_re_perl_mod_no_mod_s);
        boost_re_perl_mod_box.append(boost_re_perl_mod_mod_s);
        boost_re_perl_mod_box.append(boost_re_perl_mod_mod_x);
        append(boost_re_perl_mod_frame);

        // ----------
        boost_re_posix_mod_bk_plus_qm.set_label("bk_plus_qm");
        boost_re_posix_mod_bk_vbar.set_label("bk_vbar");
        boost_re_posix_mod_no_char_classes.set_label("no_char_classes");
        boost_re_posix_mod_no_intervals.set_label("no_intervals");

        boost_re_posix_mod_frame.set_label("boost lib POSIX regexp settings");
        boost_re_posix_mod_frame.set_child(boost_re_posix_mod_box);
        boost_re_posix_mod_box.set_selection_mode(Gtk::SelectionMode::NONE);
        boost_re_posix_mod_box.append(boost_re_posix_mod_bk_plus_qm);
        boost_re_posix_mod_box.append(boost_re_posix_mod_bk_vbar);
        boost_re_posix_mod_box.append(boost_re_posix_mod_no_char_classes);
        boost_re_posix_mod_box.append(boost_re_posix_mod_no_intervals);
        append(boost_re_posix_mod_frame);

        setMode(mode);
    }

    FindTextWidget::~FindTextWidget()
    {
    }

    void FindTextWidget::setMode(FindTextWidgetMode mode)
    {
        switch (this->mode)
        {
            case SEARCH:
                replace_frame.set_visible(false);
                break;
            case EDIT:
                replace_frame.set_visible(true);
                break;
        }
        this->mode = mode;
    }

    FindTextWidgetMode FindTextWidget::getMode()
    {
        return this->mode;
    }

    FindTextSettings FindTextWidget::getSettings()
    {
        FindTextSettings ret;

        // ret.replace_active = replace_active.get_active();

        ret.query   = query.get_text();
        ret.replace = replace.get_text();

        // ----------
        ret.std_re_mod_icase     = std_re_mod_icase.get_active();
        ret.std_re_mod_nosubs    = std_re_mod_nosubs.get_active();
        ret.std_re_mod_optimize  = std_re_mod_optimize.get_active();
        ret.std_re_mod_collate   = std_re_mod_collate.get_active();
        ret.std_re_mod_multiline = std_re_mod_multiline.get_active();

        // ----------
        ret.boost_re_perl_mod_no_mod_m = boost_re_perl_mod_no_mod_m.get_active();
        ret.boost_re_perl_mod_no_mod_s = boost_re_perl_mod_no_mod_s.get_active();
        ret.boost_re_perl_mod_mod_s    = boost_re_perl_mod_mod_s.get_active();
        ret.boost_re_perl_mod_mod_x    = boost_re_perl_mod_mod_x.get_active();

        // ----------
        ret.boost_re_posix_mod_bk_plus_qm      = boost_re_posix_mod_bk_plus_qm.get_active();
        ret.boost_re_posix_mod_bk_vbar         = boost_re_posix_mod_bk_vbar.get_active();
        ret.boost_re_posix_mod_no_char_classes = boost_re_posix_mod_no_char_classes.get_active();
        ret.boost_re_posix_mod_no_intervals    = boost_re_posix_mod_no_intervals.get_active();

        // ----------
        ret.boost_re_common_mod_nosubs      = boost_re_common_mod_nosubs.get_active();
        ret.boost_re_common_mod_optimize    = boost_re_common_mod_optimize.get_active();
        ret.boost_re_common_mod_collate     = boost_re_common_mod_collate.get_active();
        ret.boost_re_common_mod_newline_alt = boost_re_common_mod_newline_alt.get_active();

        return ret;
    }

    void FindTextWidget::setSettings(FindTextSettings q)
    {
        query.set_text(q.query);
        replace.set_text(q.replace);

        std_re_mod_icase.set_active(q.std_re_mod_icase);
        std_re_mod_nosubs.set_active(q.std_re_mod_nosubs);
        std_re_mod_optimize.set_active(q.std_re_mod_optimize);
        std_re_mod_collate.set_active(q.std_re_mod_collate);
        std_re_mod_multiline.set_active(q.std_re_mod_multiline);

        boost_re_perl_mod_no_mod_m.set_active(q.boost_re_perl_mod_no_mod_m);
        boost_re_perl_mod_no_mod_s.set_active(q.boost_re_perl_mod_no_mod_s);
        boost_re_perl_mod_mod_s.set_active(q.boost_re_perl_mod_mod_s);
        boost_re_perl_mod_mod_x.set_active(q.boost_re_perl_mod_mod_x);

        boost_re_posix_mod_bk_plus_qm.set_active(q.boost_re_posix_mod_bk_plus_qm);
        boost_re_posix_mod_bk_vbar.set_active(q.boost_re_posix_mod_bk_vbar);
        boost_re_posix_mod_no_char_classes.set_active(q.boost_re_posix_mod_no_char_classes);
        boost_re_posix_mod_no_intervals.set_active(q.boost_re_posix_mod_no_intervals);

        boost_re_common_mod_nosubs.set_active(q.boost_re_common_mod_nosubs);
        boost_re_common_mod_optimize.set_active(q.boost_re_common_mod_optimize);
        boost_re_common_mod_collate.set_active(q.boost_re_common_mod_collate);
        boost_re_common_mod_newline_alt.set_active(q.boost_re_common_mod_newline_alt);
    };

    int FindTextWidget::search_in_text(
        const std::string           in_text,
        std::shared_ptr<LineStarts> in_text_ls,

        std::function<bool()> check_stop_flag,

        std::function<int(unsigned int line, std::string text)>
            here_s_new_occurance
    )
    {
        auto query = getSettings();

        int err = 0;

        auto m1     = std::promise<void>();
        auto m1_fut = m1.get_future();

        switch (query.search_method)
        {
            default:
                return 1;
                break;
            case PLAIN:
            {
                std::string subj(query.query);

                if (subj.length() == 0)
                {
                    return 2;
                }

                auto i = in_text.begin();

                for (;;)
                {

                    if (check_stop_flag())
                    {
                        return 4;
                    }

                    auto ri = std::search(
                        i, in_text.end(), subj.begin(), subj.end()
                    );

                    if (ri == in_text.end())
                    {
                        break;
                    }
                    std::cout << "search ok" << std::endl;

                    auto dist      = std::distance(in_text.begin(), ri);
                    auto line_no   = in_text_ls->getLineByOffset(dist);
                    auto line_info = in_text_ls->getLineInfo(line_no);
                    auto r0        = std::get<0>(line_info);
                    auto r1        = std::get<1>(line_info);
                    auto err       = std::get<2>(line_info);
                    if (err != 0)
                    {
                        return 5;
                    }

                    auto substr = in_text.substr(r0, r1 - r0);

                    err = here_s_new_occurance(line_no, substr);

                    i = ri;
                    i = std::next(i);
                }
                break;
            }
        }
        return 0;
    }

} // namespace codeeditor
} // namespace wayround_i2p
