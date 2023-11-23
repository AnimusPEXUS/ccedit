#ifndef FIND_FILE_HPP
#define FIND_FILE_HPP

#include <filesystem>
#include <memory>

#include <gtkmm.h>

#include "CodeEditorAbstract.hpp"
#include "FindText.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    struct FindFileQuery
    {
        std::string           fnmatch_pattern          = "*";
        bool                  fnmatch_cs               = false;
        bool                  use_fnmatch_on_path_part = true;
        bool                  recurcive                = true;
        bool                  delve_into_hidden_dirs   = false;
        std::filesystem::path subpath                  = "/";
        bool                  use_max_depth            = false;
        unsigned short        max_depth                = 1;

        bool contents_search_enabled               = false;
        bool contents_search_cs                    = false;
        bool dont_show_files_with_0_contents_match = true;

        FindTextSettings find_text_settings;
    };

    class FindFileResultTreeItem;
    class FindFileResultTreeItemItem;

    using FindFileResultTreeItemP     = Glib::RefPtr<FindFileResultTreeItem>;
    using FindFileResultTreeItemItemP = Glib::RefPtr<FindFileResultTreeItemItem>;

    class FindFile : public Gtk::Window
    {
      public:
        static std::shared_ptr<FindFile> create(
            std::shared_ptr<ProjectCtl> p_ctl
        );

        ~FindFile();

        void setTargetEditor(
            std::shared_ptr<CodeEditorAbstract> editor
        );
        void                                unsetTargetEditor();
        std::shared_ptr<CodeEditorAbstract> getTargetEditor();

        void start();
        void stop();

        FindFileQuery getFindFileQuery();
        int           setFindFileQuery(FindFileQuery q);

      protected:
        FindFile(std::shared_ptr<ProjectCtl> p_ctl);

      private:
        std::shared_ptr<ProjectCtl>       p_ctl;
        std::shared_ptr<FindFile>         own_ptr;
        std::weak_ptr<CodeEditorAbstract> target_editor;

        // -----------

        Gtk::Box main_box;

        // -----------

        Gtk::Frame editors_frame;
        Gtk::Grid  editors_grid;

        // -----------

        Gtk::Label filemask_l;
        Gtk::Entry filemask_w;

        Gtk::Label subpath_l;
        Gtk::Entry subpath_w;

        // -----------

        Gtk::Frame   files_settings_frame;
        Gtk::FlowBox files_settings_box;

        // -----------

        Gtk::CheckButton use_fnmatch_on_path_part_cb;
        Gtk::CheckButton recurcive_cb;
        Gtk::CheckButton delve_hidden_cb;

        Gtk::Box         max_depth_box;
        Gtk::CheckButton max_depth_cb;
        Gtk::SpinButton  max_depth_sb;

        Gtk::CheckButton fnmatch_cs_cb;

        // -----------

        Gtk::Frame          search_contents_frame;
        Gtk::Box            search_contents_box;
        Gtk::CheckButton    search_contents_cb;
        Gtk::Frame          find_text_widget_frame;
        // Gtk::ScrolledWindow find_text_widget_sw;
        FindTextWidget      find_text_widget;
        Gtk::CheckButton    dont_show_files_with_0_contents_match_cb;

        // -----------

        Gtk::Frame button_frame;
        Gtk::Box   button_box;

        Gtk::Button      start_btn;
        Gtk::ProgressBar progress_bar;
        Gtk::Button      stop_btn;

        Gtk::Paned results_pan;

        Gtk::ScrolledWindow result_files_sw;
        Gtk::ListView       result_files;

        Gtk::ScrolledWindow result_lines_sw;
        Gtk::ListView       result_lines;

        void setup_result_filelist();
        void setup_result_linelist();

        std::tuple<std::filesystem::path, int> getProjectPath();

        FindFileQuery work_time_query;
        bool          search_stop_flag = false;
        bool          search_working   = false;
        int           start_search_thread();
        void          stop_search_thread();
        void          search_thread();
        int           search_thread_search_contents(FindFileResultTreeItemP item);

        void updateProgressbar(
            unsigned int dirs_count,
            unsigned int dirs_done,
            unsigned int files_count,
            unsigned int files_done
        );

        void on_filelist_activate(gint);

        void on_start_btn();
        void on_stop_btn();
        void on_destroy_sig();
    };

    // todo: this class requires rename
    class FindFileResultTreeItem : public Glib::Object
    {
      public:
        static Glib::RefPtr<FindFileResultTreeItem> create(
            std::filesystem::path subpath
        );

        ~FindFileResultTreeItem();

        const std::filesystem::path subpath;

      protected:
        FindFileResultTreeItem(
            std::filesystem::path subpath
        );

      private:
        // FindFileResultTreeItemP own_ptr;

        Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>> items;

      public: // methods
        FindFileResultTreeItemItemP create_item(
            unsigned int line,
            std::string  text
        );

        Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>>
            get_list_store();
    };

    // todo: this class requires rename
    class FindFileResultTreeItemItem : public Glib::Object
    {
      public:
        static FindFileResultTreeItemItemP create(
            // FindFileResultTreeItemP tree_item,
            unsigned int line,
            std::string  text
        );

        ~FindFileResultTreeItemItem();

        const unsigned int line;
        const std::string  text;

      protected:
        FindFileResultTreeItemItem(
            // FindFileResultTreeItemP tree_item,
            unsigned int line,
            std::string  text
        );

      private:
        // FindFileResultTreeItemP tree_item;

      public: // methods
    };

    class FindFileResultTreeItemWidget : public Gtk::Box
    {
      public:
        FindFileResultTreeItemWidget(
            const Glib::RefPtr<Gtk::ListItem> &list_item
        );
        ~FindFileResultTreeItemWidget();

        void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
        void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

      private:
        Gtk::Label subpath;
        Gtk::Label found_count;
    };

    class FindFileResultTreeItemItemWidget : public Gtk::Box
    {
      public:
        FindFileResultTreeItemItemWidget(
            const Glib::RefPtr<Gtk::ListItem> &list_item
        );
        ~FindFileResultTreeItemItemWidget();

        void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
        void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

      private:
        Gtk::Label line;
        Gtk::Label text;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
