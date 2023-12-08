
#include <experimental/scope>
#include <format>
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

        setup_search_method_list();

        main_settings_frame.set_child(main_settings_box);
        main_settings_box.append(search_method);
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
        apply_search_method_visual();

        search_method.property_selected_item().signal_changed().connect(
            sigc::mem_fun(*this, &FindTextWidget::on_search_method_changed)
        );
    }

    FindTextWidget::~FindTextWidget()
    {
    }

    void FindTextWidget::setup_search_method_list()
    {

        auto method_list_model = Gio::ListStore<TextSearchMethodListItem>::create();

        for (auto i : {
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
             })
        {
            method_list_model->append(TextSearchMethodListItem::create(i));
        }

        search_method.set_model(method_list_model);

        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto w = Gtk::make_managed<TextSearchMethodListItemWidget>(
                    list_item
                );
                list_item->set_child(*w);
            }
        );
        factory->signal_bind().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto list_item_child = list_item->get_child();

                auto tic = dynamic_cast<TextSearchMethodListItemWidget *>(
                    list_item_child
                );
                if (!tic)
                {
                    return;
                }
                tic->bind(list_item);
            }
        );
        search_method.set_factory(factory);
    }

    void FindTextWidget::setMode(FindTextWidgetMode mode)
    {
        switch (mode)
        {
            case SEARCH:
                replace_frame.set_visible(false);
                stop_after_first_match.set_visible(true);
                break;
            case EDIT:
                replace_frame.set_visible(true);
                stop_after_first_match.set_visible(false);
                break;
        }
        this->mode = mode;
    }

    FindTextWidgetMode FindTextWidget::getMode()
    {
        return this->mode;
    }

    FindTextQuery FindTextWidget::getFindTextQuery()
    {
        FindTextQuery ret;

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

    int FindTextWidget::setFindTextQuery(FindTextQuery q)
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
        return 0;
    };

    int FindTextWidget::search_in_text(
        const std::string           in_text,
        std::shared_ptr<LineStarts> in_text_ls,

        std::function<bool()> check_stop_flag,

        std::function<
            int(
                unsigned int line,
                std::string  text,
                unsigned int start_offset,
                unsigned int end_offset
            )>
            here_s_new_occurance
    )
    {
        auto query = getFindTextQuery();

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

                    err = here_s_new_occurance(
                        line_no,
                        substr,
                        dist,
                        dist + subj.length()
                    );

                    i = ri;
                    i = std::next(i);
                }
                break;
            }
        }
        return 0;
    }

    FindTextSearchMethod FindTextWidget::getVisualSelectedFindTextSearchMethod()
    {
        auto item = std::dynamic_pointer_cast<TextSearchMethodListItem>(
            search_method.get_selected_item()
        );
        if (item != NULL)
        {
            return item->value;
        }
        else
        {
            return INVALID;
        }
    }

    void FindTextWidget::setVisualSelectedFindTextSearchMethod(
        FindTextSearchMethod m
    )
    {
        auto dd_model = std::dynamic_pointer_cast<Gio::ListStore<TextSearchMethodListItem>>(search_method.get_model());

        for (gint i = 0; i != dd_model->get_n_items(); i++)
        {
            auto itm = dd_model->get_item(i);
            if (itm->value == m)
            {
                search_method.set_selected(i);
                break;
            }
        }
    }

    void FindTextWidget::apply_search_method_visual()
    {
        auto mtd = getVisualSelectedFindTextSearchMethod();

        std_re_mod_frame.set_visible(false);
        boost_re_perl_mod_frame.set_visible(false);
        boost_re_posix_mod_frame.set_visible(false);
        boost_re_common_mod_frame.set_visible(false);

        bool boost = false;

        for (auto i : {
                 BOOST_RE_ECMAScript,
                 BOOST_RE_BASIC,
                 BOOST_RE_EXTENDED,
                 BOOST_RE_AWK,
                 BOOST_RE_GREP,
                 BOOST_RE_EGREP,
                 BOOST_RE_SED,
                 BOOST_RE_PERL,
                 BOOST_RE_LITERAL
             })
        {
            if (mtd == i)
            {
                boost_re_common_mod_frame.set_visible(true);
                break;
            }
        }

        for (auto i : {BOOST_RE_BASIC, BOOST_RE_SED, BOOST_RE_GREP})
        {
            if (mtd == i)
            {
                boost_re_posix_mod_frame.set_visible(true);
                break;
            }
        }

        if (mtd == BOOST_RE_PERL)
        {
            boost_re_perl_mod_frame.set_visible(true);
        }
    }

    void FindTextWidget::on_search_method_changed()
    {
        apply_search_method_visual();
    }

    // -----

    TextSearchMethodListItemWidget::TextSearchMethodListItemWidget(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
    }

    TextSearchMethodListItemWidget::~TextSearchMethodListItemWidget()
    {
    }

    void TextSearchMethodListItemWidget::bind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
        auto list_item_item = list_item->get_item();

        auto ti = std::dynamic_pointer_cast<TextSearchMethodListItem>(
            list_item_item
        );
        if (!ti)
        {
            return;
        }

        std::string text = "undefined";

        switch (ti->value)
        {
            default:
                text = std::format("TODO: {}", (unsigned char)ti->value);
                break;
            case PLAIN:
                text = "PLAIN (simple text search)";
                break;
            case STD_RE_ECMAScript:
                text = "std C++ ECMA Script";
                break;
            case STD_RE_BASIC:
                text = "std C++ Basic";
                break;
            case STD_RE_EXTENDED:
                text = "std C++ Extended";
                break;
            case STD_RE_AWK:
                text = "std C++ AWK";
                break;
            case STD_RE_GREP:
                text = "std C++ grep";
                break;
            case STD_RE_EGREP:
                text = "std C++ egrep";
                break;
            case BOOST_RE_ECMAScript:
                text = "boostlib ECMA Script";
                break;
            case BOOST_RE_BASIC:
                text = "boostlib Basic";
                break;
            case BOOST_RE_EXTENDED:
                text = "boostlib Extended";
                break;
            case BOOST_RE_AWK:
                text = "boostlib AWK";
                break;
            case BOOST_RE_GREP:
                text = "boostlib grep";
                break;
            case BOOST_RE_EGREP:
                text = "boostlib egrep";
                break;
            case BOOST_RE_SED:
                text = "boostlib sed";
                break;
            case BOOST_RE_PERL:
                text = "boostlib Perl";
                break;
            case BOOST_RE_LITERAL:
                text = "boostlib LITERAL";
                break;
        }

        set_text(text);
    }

    // ---------------

    std::shared_ptr<FindText> FindText::create(
        std::weak_ptr<CodeEditorAbstract> editor_window
    )
    {
        auto n     = std::shared_ptr<FindText>(new FindText(editor_window));
        n->own_ptr = n;
        return n;
    }

    FindText::FindText(std::weak_ptr<CodeEditorAbstract> editor_window) :
        main_box(Gtk::Orientation::VERTICAL, 5),
        search_box(Gtk::Orientation::VERTICAL, 5),
        text_search_btn_box(Gtk::Orientation::HORIZONTAL, 5),
        find_text_widget(EDIT)
    {
        this->editor_window = editor_window;

        set_child(main_box);

        main_box.set_margin(5);
        main_box.append(search_ex);
        main_box.append(result_frame);

        search_ex.set_label("Settings");
        search_ex.set_expanded(true);
        search_ex.set_child(search_box);

        search_box.append(find_text_widget);
        search_box.append(text_search_btn_box);

        text_search_btn_box.append(text_search_btn_box1);
        text_search_btn_box.append(text_search_btn_box2);
        text_search_btn_box1.set_halign(Gtk::Align::START);
        text_search_btn_box2.set_halign(Gtk::Align::END);
        text_search_btn_box1.append(start_btn);
        text_search_btn_box1.append(stop_btn);
        text_search_btn_box2.append(acquire_cursor_position_btn);
        text_search_btn_box2.append(restore_cursor_position_btn);

        start_btn.set_label("Find All");
        stop_btn.set_label("Stop");
        acquire_cursor_position_btn.set_label("Save Cursor Position");
        restore_cursor_position_btn.set_label("Back to Line");

        result_frame.set_vexpand(true);
        result_frame.set_child(result_sw);

        result_sw.set_child(result_list_view);

        setup_result_linelist();

        saveEditorLine();

        start_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindText::on_start_btn)
        );

        stop_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindText::on_stop_btn)
        );

        acquire_cursor_position_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindText::on_acquire_cursor_position)
        );

        restore_cursor_position_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindText::on_restore_cursor_position)
        );

        signal_destroy().connect(
            sigc::mem_fun(*this, &FindText::on_destroy_sig)
        );
    }

    void FindText::setup_result_linelist()
    {
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect(
            [this](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto w = Gtk::make_managed<FindFileResultTreeItemItemWidget>(
                    list_item,
                    [this](FindFileResultTreeItemItemP item)
                    {
                        if (auto p = editor_window.lock())
                        {
                            p->setCurrentLine(
                                item->line,
                                true
                            );
                            p->selectSlice(
                                item->start_offset,
                                item->end_offset
                            );
                        }
                    }
                );
                list_item->set_child(*w);
            }
        );
        factory->signal_bind().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto list_item_child = list_item->get_child();

                auto tic = dynamic_cast<FindFileResultTreeItemItemWidget *>(
                    list_item_child
                );
                if (!tic)
                {
                    return;
                }
                tic->bind(list_item);
            }
        );
        result_list_view.set_factory(factory);
    }

    FindText::~FindText()
    {
    }

    FindTextQuery FindText::getFindTextQuery()
    {
        return find_text_widget.getFindTextQuery();
    }

    int FindText::setFindTextQuery(FindTextQuery q)
    {
        return find_text_widget.setFindTextQuery(q);
    }

    void FindText::saveEditorLine()
    {
        if (auto p = editor_window.lock())
        {
            saved_cursor_line = p->getCurrentLine();
            restore_cursor_position_btn.set_label(
                std::format("Back to Line {}", saved_cursor_line)
            );
        }
        else
        {
            // todo: exception?
        }
    }

    void FindText::on_start_btn()
    {
        std::cout << "on_start_btn()" << std::endl;
        start_search_thread();
    }

    void FindText::on_stop_btn()
    {
        std::cout << "on_stop_btn()" << std::endl;
        stop_search_thread();
    }

    int FindText::start_search_thread()
    {
        if (search_working)
        {
            return 1;
        }
        search_stop_flag = false;

        work_time_query = getFindTextQuery();

        // todo: thread
        std::cout << "std::thread(search_thread);" << std::endl;
        std::thread thr1(&FindText::search_thread, this);
        thr1.detach();
        return 0;
    }

    void FindText::stop_search_thread()
    {
        search_stop_flag = true;
    }

    void FindText::search_thread()
    {
        if (search_working)
        {
            return;
        }
        search_working = true;
        auto sg04      = std::experimental::fundamentals_v3::scope_exit(
            [this]
            {
                search_working = false;
            }
        );

        auto m1     = std::promise<void>();
        auto m1_fut = m1.get_future();

        auto item_store = Gio::ListStore<FindFileResultTreeItemItem>::create();

        Glib::MainContext::get_default()->signal_idle().connect_once(
            [this, &m1, &item_store]()
            {
                auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                    [&m1]()
                    {
                        m1.set_value();
                    }
                );

                auto sel = Gtk::SingleSelection::create(item_store);
                result_list_view.set_model(sel);
            }
        );
        m1_fut.wait();

        std::cout << "FindText::search_thread()" << std::endl;

        std::string cont_txt;

        if (auto p = editor_window.lock())
        {
            cont_txt = p->getText();
        }
        else
        {
            // todo: report error
            return;
        }

        auto cont_txt_ls = std::make_shared<LineStarts>(cont_txt);

        // todo: error checking?
        find_text_widget.search_in_text(
            cont_txt,
            cont_txt_ls,

            [this]() -> bool
            {
                return search_stop_flag;
            },
            [this, &item_store](
                unsigned int line,
                std::string  line_text,
                unsigned int start_offset,
                unsigned int end_offset
            ) -> int
            {
                auto m1     = std::promise<void>();
                auto m1_fut = m1.get_future();

                Glib::MainContext::get_default()->signal_idle().connect_once(
                    [this,
                     &m1,
                     &item_store,
                     &line,
                     &line_text,
                     &start_offset,
                     &end_offset]()
                    {
                        auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                            [&m1]()
                            {
                                m1.set_value();
                            }
                        );

                        auto s1 = trim_right(line_text);

                        auto list_item = FindFileResultTreeItemItem::create(
                            std::filesystem::path(""), // not used
                            line,
                            s1,
                            start_offset,
                            end_offset
                        );
                        // todo: use insert_sorted()
                        item_store->append(list_item);
                    }
                );

                m1_fut.wait();

                return 0;
            }
        );
    }

    void FindText::on_acquire_cursor_position()
    {
        saveEditorLine();
    }

    void FindText::on_restore_cursor_position()
    {
        if (auto p = editor_window.lock())
        {
            p->setCurrentLine(saved_cursor_line, true);
        }
        else
        {
            // todo: exception?
        }
    }

    void FindText::on_destroy_sig()
    {
        own_ptr.reset();
    }

} // namespace codeeditor
} // namespace wayround_i2p
