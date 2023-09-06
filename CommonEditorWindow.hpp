#ifndef COMMON_EDITOR_WINDOW_HPP
#define COMMON_EDITOR_WINDOW_HPP

#include <memory>

#include <gtkmm.h>

#include "CodeEditorAbstract.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class CommonEditorWindow : public CodeEditorAbstract, public Gtk::Window
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

      private:
        std::shared_ptr<ProjectCtl>  project_ctl;
        std::shared_ptr<WorkSubject> subject;

        std::shared_ptr<CodeEditorAbstract> own_ptr;

        Gtk::Box            main_box;
        Gtk::Paned          paned;
        Gtk::ScrolledWindow text_view_sw;
        Gtk::TextView       text_view;
        Gtk::ScrolledWindow outline_view_sw;
        Gtk::ColumnView     outline_view;

        void on_destroy_sig();
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
