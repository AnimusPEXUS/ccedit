#ifndef WORK_SUBJECT_HPP
#define WORK_SUBJECT_HPP

#include <string>
#include <memory>

#include <gtkmm.h>

#include "Controller.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class WorkSubject
    {
      public:
        WorkSubject(
            std::shared_ptr<Controller> controller,
            std::shared_ptr<ProjectCtl> project_ctl,
            std::string                 filepath
        );

      private:
        Controller *controller;

        std::string     filepath;
        Gtk::TextBuffer buff;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
