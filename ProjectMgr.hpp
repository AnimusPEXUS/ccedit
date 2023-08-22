#ifndef PROJECT_MGR_HPP
#define PROJECT_MGR_HPP

#include <gtkmm.h>

#include "Controller.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class Controller;

    class ProjectMgr : public Gtk::Window
    {
      public:
        ProjectMgr(std::shared_ptr<Controller> controller);

      private:
        std::shared_ptr<Controller> controller;

        // ProjectMgrTableRow ProjectMgrTableRow;

        Gtk::ScrolledWindow                project_list_sw;
        Gtk::ColumnView                    project_list_view;
        Glib::RefPtr<Gtk::MultiSelection> project_list_view_selection;

        Gtk::Box main_box;
        Gtk::Box button_box;
        Gtk::Box button_box_sec;

        Gtk::Button    add_proj;
        Gtk::Button    rm_proj;
        Gtk::Button    edit_proj;
        Gtk::Separator separ;
        Gtk::Button    open_proj;
        Gtk::Button    save_cfg;

        void on_add_click();
        void on_rm_click();
        void on_edit_click();
        void on_open_click();
        

        void on_save_cfg();
        void on_modules_info_print();

        void add_columns();

        void table_cell_setup(
            const Glib::RefPtr<Gtk::ListItem> &list_item,
            Gtk::Align                         halign
        );
        void table_name_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
        void table_path_cell_bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
