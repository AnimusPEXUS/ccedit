#ifndef WAYROUND_I2P_20240311_144846_114859
#define WAYROUND_I2P_20240311_144846_114859

#include <format>
#include <memory>

#include <gtkmm.h>

#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace ccedit
{

    class EditorListView : public Gtk::Window
    {
      public:
        EditorListView(std::shared_ptr<ProjectCtl> project_ctl);
        ~EditorListView();

      private:
        std::shared_ptr<ProjectCtl> project_ctl;

        Gtk::Box main_box;

        Gtk::Box tools_box;

        Gtk::ScrolledWindow               eds_view_sw;
        Gtk::ColumnView                   eds_view;
        Glib::RefPtr<Gtk::MultiSelection> eds_view_sel;

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
