#include <format>
#include <iostream>
#include <memory>

#include <unicode/stringpiece.h>
#include <unicode/unistr.h>

#include "MWin.hpp"
#include "utils.hpp"

namespace wayround_i2p
{
namespace unitest
{

    const std::string TESTING_TEXT_0 = R"-(

    // path_box.set_spacing(5);
    // абв
    reset_view_btn.set_label("⏹️");
    reset_view_btn.set_tooltip_text("Reset View");
    go_root_btn.set_label("🏠");
    go_root_btn.set_tooltip_text("Nav to Project Root");
    refresh_btn.set_label("⟲");
    refresh_btn.set_tooltip_text("Refresh");

    filelauncher_dir_btn.set_label("🗁");
    filelauncher_dir_btn.set_tooltip_text("Open current Folder in System's App");
    find_file_btn.set_label("🔍");
    find_file_btn.set_tooltip_text("Find File..");

    make_file_or_directory_btn.set_label("🆕");
    make_file_or_directory_btn.set_tooltip_text("mk dir/file..");
    rename_file_or_directory_btn.set_label("✒️");
    rename_file_or_directory_btn.set_tooltip_text("Rename..");
    remove_file_or_directory_btn.set_label("␡");
    remove_file_or_directory_btn.set_tooltip_text("Delete File or Dir..");
)-";

    const std::string TESTING_TEXT_1 = R"-(
абв
⏹️
🏠
⟲
🗁
🔍
🆕
✒️
␡
)-";

    std::shared_ptr<MWin> MWin::create(std::shared_ptr<Gtk::Application> app)
    {
        auto ret     = std::shared_ptr<MWin>(new MWin(app));
        ret->own_ptr = ret;
        return ret;
    }

    MWin::MWin(std::shared_ptr<Gtk::Application> app) :
        app(app),
        main_box(Gtk::Orientation::VERTICAL, 5),
        examples_box(Gtk::Orientation::HORIZONTAL, 5),
        plain_test_box(Gtk::Orientation::HORIZONTAL, 5),
        regex_test_box(Gtk::Orientation::HORIZONTAL, 5),
        values_box(Gtk::Orientation::HORIZONTAL, 5)
    {
        set_title("Unicode Tests");
        set_child(main_box);

        main_box.set_margin(5);

        main_box.append(examples_box);
        main_box.append(test_text_sw);
        main_box.append(plain_test_box);
        main_box.append(regex_test_box);
        main_box.append(calc_values_btn);
        main_box.append(values_box_sw);

        examples_box.append(example_0_btn);
        examples_box.append(example_1_btn);

        test_text_sw.set_size_request(-1, 400);
        values_box_sw.set_vexpand(true);

        values_box_sw.set_child(values_box);

        values_box.set_homogeneous(true);
        values_box.append(calc_values_std_res_frm);
        values_box.append(calc_values_glib_res_frm);
        values_box.append(calc_values_gtktexbuffer_res_frm);
        values_box.append(calc_values_icu_res_frm);

        calc_values_std_res_frm.set_label("std C++");
        calc_values_glib_res_frm.set_label("glib::ustring");
        calc_values_gtktexbuffer_res_frm.set_label("Gtk::TextBuffer");
        calc_values_icu_res_frm.set_label("icu::UnicodeString");

        calc_values_std_res_frm.set_child(calc_values_std_res_lbl);
        calc_values_glib_res_frm.set_child(calc_values_glib_res_lbl);
        calc_values_gtktexbuffer_res_frm.set_child(calc_values_gtktexbuffer_res_lbl);
        calc_values_icu_res_frm.set_child(calc_values_icu_res_lbl);

        for (auto i : {
                 &calc_values_std_res_lbl,
                 &calc_values_glib_res_lbl,
                 &calc_values_gtktexbuffer_res_lbl,
                 &calc_values_icu_res_lbl
             })
        {
            i->set_halign(Gtk::Align::START);
            i->set_valign(Gtk::Align::START);
        }

        test_text_sw.set_child(test_text);
        test_text.get_buffer()->set_text(TESTING_TEXT_0);

        plain_test.set_hexpand(true);
        plain_test_box.append(plain_test);
        plain_test_box.append(plain_test_btn);

        regex_test.set_hexpand(true);
        regex_test_box.append(regex_test);
        regex_test_box.append(regex_test_btn);

        plain_test_btn.set_label("Search");
        regex_test_btn.set_label("Search");

        example_0_btn.set_label("Example 0");
        example_1_btn.set_label("Example 1");

        calc_values_btn.set_label("Calculate Values");

        calc_values_btn.signal_clicked().connect(
            [this]()
            {
                do_calculations();
            }
        );

        example_0_btn.signal_clicked().connect(
            [this]()
            {
                test_text.get_buffer()->set_text(TESTING_TEXT_0);
            }
        );

        example_1_btn.signal_clicked().connect(
            [this]()
            {
                test_text.get_buffer()->set_text(TESTING_TEXT_1);
            }
        );
    }

    MWin::~MWin()
    {
        std::cout << "~MWin()" << std::endl;
    }

    void MWin::destroy()
    {
        own_ptr.reset();
        app.reset();
    }

    void MWin::do_calculations()
    {
        do_calculations_std();
        do_calculations_glib();
        do_calculations_gtktexbuffer();
        do_calculations_icu();
    }

    void MWin::do_calculations_std()
    {
        auto buff = test_text.get_buffer();
        auto text = buff->get_text();

        auto ls = std::unique_ptr<LineStarts>(new LineStarts(text));

        std::string msg = std::format(
            "text length: {}\n", text.length()
        );

        msg += "parsing diagnostics: " + ls->getParsingDiagnostics(text);

        calc_values_std_res_lbl.set_label(msg);
    }

    void MWin::do_calculations_glib()
    {
        auto buff = test_text.get_buffer();
        auto text = buff->get_text();

        auto ls = std::unique_ptr<LineStarts>(new LineStarts(text));

        std::string msg = std::format(
            "text length: {}\n", text.length()
        );

        msg += "parsing diagnostics: " + ls->getParsingDiagnostics(text);

        calc_values_glib_res_lbl.set_label(msg);
    }

    void MWin::do_calculations_gtktexbuffer()
    {
        auto buff = test_text.get_buffer();
        auto text = buff->get_text();

        std::string msg = std::format(
            "text length: {}\n", buff->get_char_count()
        );

        msg += "parsing diagnostics:\n";

        for (int i = 0; i != buff->get_line_count(); i++)
        {
            auto j          = i + 1;
            auto line_iter0 = buff->get_iter_at_line(i);
            auto line_iter1 = buff->get_iter_at_line(i + 1);
            auto r0         = line_iter0.get_offset();
            auto r1         = line_iter1.get_offset();
            auto line_len   = r1 - r0;
            auto substr     = buff->get_text(line_iter0, line_iter1);
            msg.append(
                std::format(
                    "{} ({}, {}, len:{}): `{}`\n",
                    j,
                    r0,
                    r1,
                    line_len,
                    trim_right(substr)
                )
            );
        }

        calc_values_gtktexbuffer_res_lbl.set_label(msg);
    }

    void MWin::do_calculations_icu()
    {
        auto buff     = test_text.get_buffer();
        auto text     = buff->get_text();
        auto text_icu = icu::UnicodeString::fromUTF8(icu::StringPiece(text));

        auto ls = std::unique_ptr<LineStartsICU>(new LineStartsICU(text_icu));

        std::string msg = std::format(
            "text length: {}\n", text_icu.length()
        );

        msg += "parsing diagnostics: " + ls->getParsingDiagnostics(text_icu);

        calc_values_icu_res_lbl.set_label(msg);
    }

} // namespace unitest
} // namespace wayround_i2p
