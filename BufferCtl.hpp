#ifndef BUFFER_CTL_HPP
#define BUFFER_CTL_HPP

#include "Buffer.hpp"
#include "Controller.hpp"

#include "types.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    struct BufferItem
    {
        std::string filepath;
        ProjectCtl *proj_ctl;
    }

    class BufferCtl
    {
      public:
        // set project to null if BufferCtl not part of the project
        BufferCtl(
            std::shared_ptr<Controller> controller,
            std::shared_ptr<ProjectCtl> project
        );

        // if BufferCtl is under project, then filepath must be
        // relative to project dir. else filepath must be a full
        // filepath
        std::tuple<shared_ptr<Buffer>, std::exception> createBuffer(
            std::string filepath
        );

      private:
        Controller *controller;
        ProjectCtl *project;

        std::list<std::shared_ptr<Buffer>> buffer_list;
    }

} // namespace codeeditor
} // namespace wayround_i2p

#endif
