#ifndef COMMON_EDITOR_WINDOW_HPP
#define COMMON_EDITOR_WINDOW_HPP

#include <memory>

#include <gtkmm.h>
// #include <gtksourceview/gtksource.h>

#include "CodeEditorAbstract.hpp"
#include "FindText.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    struct CommonEditorWindowStateStorage
    {
        unsigned int cur_pos_iter_offset = 0;
        double       scroll_adj          = 0;
    };

    class CommonEditorWindow : public CodeEditorAbstract,
                               public Gtk::ApplicationWindow
    {
      public:
        CommonEditorWindow(
            std::shared_ptr<ProjectCtl>  project_ctl,
            std::shared_ptr<WorkSubject> subject
        );
        ~CommonEditorWindow();

        // void saveOwnPtr(std::shared_ptr<CodeEditorAbstract>) override;

        // ⬇️ {CodeEditorAbstract overrides} ⬇️

        void         show() override;
        void         close() override;
        unsigned int getCursorOffsetPosition() override;
        void         setCursorOffsetPosition(unsigned int new_pos, bool scroll = true) override;
        unsigned int getCurrentLine() override;
        void         setCurrentLine(unsigned int line, bool scroll = true) override;
        void         selectSlice(unsigned int start, unsigned int end) override;
        void         unselect() override;
        std::string  getText() override;

        // ⬆️ {CodeEditorAbstract overrides} ⬆️

        void updateTitle(); // todo: make it private?

        void setOutlineContents(
            std::vector<std::tuple<unsigned int, std::string>> val
        );
        void setOutlineCurrentLine(unsigned int val);

        virtual std::vector<std::tuple<unsigned int, std::string>>
            genOutlineContents();

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;
        CodeEditorMod               *mod;

        // std::shared_ptr<CodeEditorAbstract> own_ptr;

        Gtk::Box                                      main_box;
        Gtk::Paned                                    paned;
        Gtk::Box                                      text_view_box_upper;
        Gtk::Box                                      text_view_box;
        Gtk::DrawingArea                              linum_area;
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

        Glib::RefPtr<Gio::Menu>     mm_search;
        Glib::RefPtr<Gio::MenuItem> mm_search_search_window;

        void action_buffer_reload();
        void action_buffer_save();
        void action_buffer_save_as();

        void action_search_show_window();

        CommonEditorWindowStateStorage saved_editor_state;
        void                           saveState();
        void                           restoreState();

        void redraw_linum(
            const Cairo::RefPtr<Cairo::Context> &cont,
            int                                  width,
            int                                  height
        );

        void force_redraw_linum();

        void on_outline_refresh_btn();
        void on_outline_activate(guint val);

        void on_destroy_sig();

      protected:
        Glib::RefPtr<Gio::Menu> getMenuModel();

      private:
        virtual void make_special_menu();
        virtual void make_special_actions();
        virtual void make_special_hotkeys();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
