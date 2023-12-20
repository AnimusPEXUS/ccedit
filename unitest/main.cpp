#include <iostream>

#include <gtkmm.h>

#include "MWin.hpp"

using namespace wayround_i2p::unitest;

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create();
    int  ret = 0;
    {
        auto win = MWin::create(app);
        app->signal_startup().connect(
            [&app, &win]()
            {
                app->add_window(*win);
                win->present();
            }
        );
        ret = app->run(argc, argv);
        win->destroy();
    }
    std::cout << "thx for using unitest" << std::endl;
    return ret;
}
