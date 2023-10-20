#ifndef FIND_FILE_HPP
#define FIND_FILE_HPP

#include <filesystem>
#include <memory>

#include <gtkmm.h>

#include "CodeEditorAbstract.hpp"
#include "ProjectCtl.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    enum FindFileQueryType : unsigned char
    {
        PLAIN,
        REGEXP_BASIX,
        REGEXP_EXTENDED,
        REGEXP_SED,
        REGEXP_PERL,
        REGEXP_ECMA
    };

    struct FindFileQuery
    {
        bool                              use_filemask           = false;
        std::string                       filemask               = "*";
        bool                              filename_casesensitive = false;
        std::weak_ptr<CodeEditorAbstract> target_editor;
        bool                              recurcive     = true;
        bool                              use_subpath   = false;
        std::filesystem::path             subpath       = "/";
        bool                              use_max_depth = false;
        unsigned short                    max_depth     = 1;
        bool                              grep          = false;
        std::string                       query         = "";
        FindFileQueryType                 query_type    = PLAIN;
        bool                              use_edit      = false;
        std::string                       edit          = "";
    };

    class FindFile : public Gtk::Window
    {
      public:
        static std::shared_ptr<FindFile> create(std::shared_ptr<ProjectCtl> p_ctl);

        ~FindFile();

        void                                setTargetEditor(std::shared_ptr<CodeEditorAbstract> editor);
        void                                unsetTargetEditor();
        std::shared_ptr<CodeEditorAbstract> getTargetEditor();

        void setQuery(std::shared_ptr<FindFileQuery> q);
        void start();
        void stop();

      protected:
        FindFile(std::shared_ptr<ProjectCtl> p_ctl);

      private:
        std::shared_ptr<ProjectCtl> p_ctl;
        std::shared_ptr<FindFile>   own_ptr;

        Gtk::Box main_box;

        Gtk::Frame editors_frame;
        Gtk::Grid  editors_grid;

        Gtk::CheckButton filemask_cb;
        Gtk::Label       filemask_l;
        Gtk::Entry       filemask_w;

        Gtk::Label subpath_l;
        Gtk::Entry subpath_w;

        Gtk::CheckButton grep_cb;
        Gtk::Label       grep_l;
        Gtk::Entry       query_w;

        Gtk::Frame settings_frame;
        Gtk::Grid  settings_grid;

        Gtk::CheckButton recurcive_cb;
        Gtk::Label       recurcive_l;

        Gtk::CheckButton max_depth_cb;
        Gtk::Label       max_depth_l;
        Gtk::SpinButton  max_depth_w;

        Gtk::CheckButton filename_casesensitive_cb;
        Gtk::Label       filename_casesensitive_l;

        Gtk::CheckButton query_casesensitive_cb;
        Gtk::Label       query_casesensitive_l;

        Gtk::Label   query_type_l;
        Gtk::ListBox query_type_w;

        Gtk::Frame button_frame;
        Gtk::Box   button_box;

        Gtk::Button start_btn;
        Gtk::Button stop_btn;

        Gtk::ScrolledWindow results_sw;
        Gtk::ColumnView     results;
    };

} // namespace codeeditor
} // namespace wayround_i2p

#endif
