#include <gtkmm.h>

#include "Controller.hpp"

using namespace wayround_i2p::codeeditor;

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create("i2p.wayround.codeeditor");

    auto controller = new Controller(app);

    return controller->run(argc, argv);
}
