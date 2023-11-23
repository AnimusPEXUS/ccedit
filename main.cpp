#include <gtkmm.h>

#include "Controller.hpp"

using namespace wayround_i2p::codeeditor;

int main(int argc, char *argv[])
{

    auto app = Gtk::Application::create();

    int ret = 0;

    {
        auto controller = std::shared_ptr<Controller>(
            new Controller(app)
        );

        // some subprogramms needs to use Controller.
        // so we copying it's shared_ptr here.
        controller->own_ptr = controller;

        ret = controller->run(argc, argv);

        controller->own_ptr.reset();
        controller.reset();
    }

    std::cout << "thx for using CodeEditor" << std::endl;

    return ret;
}
