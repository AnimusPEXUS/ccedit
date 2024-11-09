#include <iostream>

#include <gtkmm.h>

#include "Controller.hpp"

int main(int argc, char *argv[])
{

    auto app = Gtk::Application::create();

    int ret = 0;

    {
        auto controller = wayround_i2p::ccedit::Controller::create(app);

        ret = controller->run(argc, argv);

        controller.reset();
    }

    std::cout << "thx for using CodeEditor" << std::endl;

    return ret;
}
