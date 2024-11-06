#include <gtkmm.h>

#include "Controller.hpp"

// using namespace wayround_i2p::ccedit;

int main(int argc, char *argv[])
{

    auto app = Gtk::Application::create();

    int ret = 0;

    {
        auto controller = std::shared_ptr<wayround_i2p::ccedit::Controller>(
            new wayround_i2p::ccedit::Controller(app)
        );

        controller->own_ptr = controller;

        ret = controller->run(argc, argv);

        controller->own_ptr.reset();
        controller.reset();
    }

    std::cout << "thx for using CodeEditor" << std::endl;

    return ret;
}
