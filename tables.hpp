#ifndef TABLES_HPP
#define TABLES_HPP

#include <filesystem>
#include <memory>

#include <gtkmm.h>

#include "ProjectCtl.hpp"
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
        std::string proj_name();
        void        proj_name(std::string);

        std::filesystem::path proj_path();
        void                  proj_path(std::filesystem::path);

        // std::shared_ptr<Controller> controller;
        std::shared_ptr<ProjectCtl> proj_ctl;

        static Glib::RefPtr<ProjectTableRow> create()
        {
            return Glib::make_refptr_for_instance<ProjectTableRow>(
                new ProjectTableRow()
            );
        }

        std::shared_ptr<sigc::signal<void()>> signal_proj_name_changed();
        std::shared_ptr<sigc::signal<void()>> signal_proj_path_changed();

      protected:
        ProjectTableRow();

      private:
        std::string           priv_proj_name;
        std::filesystem::path priv_proj_path;

        std::shared_ptr<sigc::signal<void()>> priv_signal_proj_name_changed;
        std::shared_ptr<sigc::signal<void()>> priv_signal_proj_path_changed;
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

    // todo: move this to better place?
    class OutlineTableRow : public Glib::Object
    {
      public:
        unsigned int line;
        std::string  text;

        static Glib::RefPtr<OutlineTableRow> create()
        {
            return Glib::make_refptr_for_instance<OutlineTableRow>(
                new OutlineTableRow()
            );
        }

      protected:
        OutlineTableRow() :
            Glib::ObjectBase(typeid(CodeEditorTableRow))
        {
        }
    };

    class FileExplorerDirTreeRow : public Glib::Object
    {
      public:
        // std::string filename;

        // todo: retry finding better solution?
        // note: I have to store here full subpath,
        //       because Gtk::TreeListModel's SlotCreateModel
        //       doesn't gives way to traceback expanded row parents
        //       and build path by single filename().
        //       I don't like this, but I don't have other choice.

        std::filesystem::path pth;

        static Glib::RefPtr<FileExplorerDirTreeRow> create()
        {
            return Glib::make_refptr_for_instance<FileExplorerDirTreeRow>(
                new FileExplorerDirTreeRow()
            );
        }

      protected:
        FileExplorerDirTreeRow() :
            Glib::ObjectBase(typeid(FileExplorerDirTreeRow))
        {
        }
    };

    class FileExplorerFileListRow : public Gtk::Box
    {
      public:
        // std::string filename;

        // todo: I have to store here full path,
        //       because Gtk::TreeListModel's SlotCreateModel
        //       doesn't gives way to traceback expanded row parents
        //       and build path by single filename().
        //       I don't like this, but I don't have other choice.

        std::filesystem::path pth;

        static Glib::RefPtr<FileExplorerFileListRow> create()
        {
            return Glib::make_refptr_for_instance<FileExplorerFileListRow>(
                new FileExplorerFileListRow()
            );
        }

      protected:
        FileExplorerFileListRow() :
            Glib::ObjectBase(typeid(FileExplorerFileListRow))
        {
        }
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
