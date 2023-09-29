#ifndef COMMON_EDITOR_WINDOW_HPP
#define COMMON_EDITOR_WINDOW_HPP

#include <memory>

#include <gtkmm.h>
// #include <gtksourceview/gtksource.h>

#include "CodeEditorAbstract.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class CommonEditorWindow : public CodeEditorAbstract,
                               public Gtk::ApplicationWindow
    {
      public:
        CommonEditorWindow(
            std::shared_ptr<ProjectCtl>  project_ctl,
            std::shared_ptr<WorkSubject> subject
        );
        ~CommonEditorWindow();

        void saveOwnPtr(std::shared_ptr<CodeEditorAbstract>) override;

        void show() override;
        void close() override;

        void updateTitle();

        void setOutlineContents(
            std::vector<std::tuple<unsigned int, std::string>> val
        );
        void setOutlineCurrentLine(unsigned int val);

        virtual std::vector<std::tuple<unsigned int, std::string>>
            genOutlineContents();

        void setTextPreservingView(std::string txt);

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;
        CodeEditorMod               *mod;

        std::shared_ptr<CodeEditorAbstract> own_ptr;

        Gtk::Box                                      main_box;
        Gtk::Paned                                    paned;
        Gtk::ScrolledWindow                           text_view_sw;
        Gtk::TextView                                 text_view;
        Gtk::Box                                      outline_box;
        Gtk::ScrolledWindow                           outline_view_sw;
        Gtk::ColumnView                               outline_view;
        Gtk::Button                                   outline_view_refresh_btn;
        Glib::RefPtr<Gtk::SingleSelection>            outline_view_selection;
        Glib::RefPtr<Gio::ListStore<OutlineTableRow>> outline_list_store;

        void setup_outline_columns();

        void make_menubar();
        void make_actions();
        void make_hotkeys();

        Gtk::PopoverMenuBar menu_bar;

        Glib::RefPtr<Gio::Menu> menu_model;

        Glib::RefPtr<Gio::Menu>     mm_buffer;
        Glib::RefPtr<Gio::MenuItem> mm_buffer_reload;
        Glib::RefPtr<Gio::MenuItem> mm_buffer_save;
        Glib::RefPtr<Gio::MenuItem> mm_buffer_save_as;

        void action_buffer_reload();
        void action_buffer_save();
        void action_buffer_save_as();

        void on_outline_refresh_btn();
        void on_outline_activate(guint val);

        void on_destroy_sig();

      protected:
        // std::shared_ptr<ProjectCtl> getProjectCtl();
        // std::shared_ptr<WorkSubject> getSubject();
        Glib::RefPtr<Gio::Menu> getMenuModel();

      private:
        virtual void make_special_menu();
        virtual void make_special_actions();
        virtual void make_special_hotkeys();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
