#ifndef WAYROUND_I2P_20240311_144846_123889
#define WAYROUND_I2P_20240311_144846_123889

#include <format>
#include <memory>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace ccedit
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

        void updateTitle();

        void on_destroy_sig();
    };

} // namespace ccedit
} // namespace wayround_i2p

#endif
