#ifndef TABLES_HPP
#define TABLES_HPP

#include <memory>

#include <gtkmm.h>

// #include "ProjectCtl.hpp"
// #include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class ProjectCtl;
    class WorkSubject;
    class CodeEditorAbstract;

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

    class WorkSubjectTableRow : public Glib::Object
    {
      public:
        std::shared_ptr<WorkSubject> work_subj;

        static Glib::RefPtr<WorkSubjectTableRow> create()
        {
            return Glib::make_refptr_for_instance<WorkSubjectTableRow>(
                new WorkSubjectTableRow()
            );
        }

      protected:
        WorkSubjectTableRow() :
            Glib::ObjectBase(typeid(WorkSubjectTableRow))
        {
        }
    };

    class CodeEditorTableRow : public Glib::Object
    {
      public:
        std::shared_ptr<CodeEditorAbstract> editor;

        static Glib::RefPtr<CodeEditorTableRow> create()
        {
            return Glib::make_refptr_for_instance<CodeEditorTableRow>(
                new CodeEditorTableRow()
            );
        }

      protected:
        CodeEditorTableRow() :
            Glib::ObjectBase(typeid(CodeEditorTableRow))
        {
        }
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
