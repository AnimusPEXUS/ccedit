#ifndef TABLES_HPP
#define TABLES_HPP

#include <memory>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    // class ProjectCtl;

    class ProjectTableRow : public Glib::Object
    {
      public:
        std::string                 proj_name;
        std::string                 proj_path;
        std::shared_ptr<ProjectCtl> proj_ctl;

        static Glib::RefPtr<ProjectTableRow> create()
        {
            return Glib::make_refptr_for_instance<ProjectTableRow>(
                new ProjectTableRow()
            );
        }

      protected:
        ProjectTableRow() :
            Glib::ObjectBase(typeid(ProjectTableRow))
        {
        }
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
