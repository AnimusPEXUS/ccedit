#ifndef WORK_SUBJECT_LIST_VIEW_HPP
#define WORK_SUBJECT_LIST_VIEW_HPP

#include <memory>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class WorkSubjectListView : public Gtk::Window
    {
      public:
        WorkSubjectListView(std::shared_ptr<ProjectCtl> project_ctl);
        ~WorkSubjectListView();

      private:
        std::shared_ptr<ProjectCtl> project_ctl;

        Gtk::Box main_box;

        Gtk::Box tools_box;

        Gtk::ScrolledWindow               ws_view_sw;
        Gtk::ColumnView                   ws_view;
        Glib::RefPtr<Gtk::MultiSelection> ws_view_sel;

        void add_columns();

        void table_cell_setup(
            const Glib::RefPtr<Gtk::ListItem> &list_item,
            Gtk::Align                         halign
        );
        void table_subject_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);

        void on_destroy_sig();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
