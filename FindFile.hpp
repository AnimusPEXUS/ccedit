#ifndef FIND_FILE_HPP
#define FIND_FILE_HPP

#include <filesystem>
#include <memory>

#include <gtkmm.h>

#include "FindTables.hpp"
#include "FindTypes.hpp"

#include "CodeEditorAbstract.hpp"
#include "FindText.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class FindTextWidget;

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

        Gtk::Frame       search_contents_frame;
        Gtk::Box         search_contents_box;
        Gtk::CheckButton search_contents_cb;
        Gtk::Frame       find_text_widget_frame;
        // Gtk::ScrolledWindow find_text_widget_sw;
        FindTextWidget   find_text_widget;
        Gtk::CheckButton dont_show_files_with_0_contents_match_cb;

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

} // namespace codeeditor
} // namespace wayround_i2p

#endif
