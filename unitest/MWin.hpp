#ifndef MWIN_HPP
#define MWIN_HPP

#include <gtkmm.h>

namespace wayround_i2p
{
namespace unitest
{

    class MWin : public Gtk::Window
    {
      public:
        static std::shared_ptr<MWin> create(
            std::shared_ptr<Gtk::Application> app
        );

        ~MWin();

        void destroy();

      protected:
        MWin(std::shared_ptr<Gtk::Application> app);

      private:
        std::shared_ptr<Gtk::Application> app;
        std::shared_ptr<MWin>             own_ptr;

        Gtk::Box main_box;

        Gtk::Box    examples_box;
        Gtk::Button example_0_btn;
        Gtk::Button example_1_btn;

        Gtk::ScrolledWindow test_text_sw;
        Gtk::TextView       test_text;

        Gtk::Box    plain_test_box;
        Gtk::Entry  plain_test;
        Gtk::Button plain_test_btn;

        Gtk::Box    regex_test_box;
        Gtk::Entry  regex_test;
        Gtk::Button regex_test_btn;

        Gtk::Button calc_values_btn;

        Gtk::ScrolledWindow values_box_sw;
        Gtk::Box            values_box;
        Gtk::Frame          calc_values_std_res_frm;
        Gtk::Frame          calc_values_glib_res_frm;
        Gtk::Frame          calc_values_gtktexbuffer_res_frm;
        Gtk::Frame          calc_values_icu_res_frm;

        Gtk::Label calc_values_std_res_lbl;
        Gtk::Label calc_values_glib_res_lbl;
        Gtk::Label calc_values_gtktexbuffer_res_lbl;
        Gtk::Label calc_values_icu_res_lbl;

        void do_calculations();
        void do_calculations_std();
        void do_calculations_glib();
        void do_calculations_gtktexbuffer();
        void do_calculations_icu();
    };

} // namespace unitest
} // namespace wayround_i2p

#endif
