#ifndef PROJECT_CTL_HPP
#define PROJECT_CTL_HPP

#include <gtkmm.h>

#include "Controller.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class Controller;

    class ProjectCtl : public Gtk::Window
    {
      public:
        ProjectCtl(std::shared_ptr<Controller> controller);

      private:
        std::shared_ptr<Controller> controller;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
