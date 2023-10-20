
#include "FindFile.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    std::shared_ptr<FindFile> FindFile::create(std::shared_ptr<ProjectCtl> p_ctl)
    {
        auto ret     = std::shared_ptr<FindFile>(new FindFile(p_ctl));
        ret->own_ptr = ret;
        return ret;
    }

    FindFile::FindFile(std::shared_ptr<ProjectCtl> p_ctl)
    {
        this->p_ctl = p_ctl;

        set_child(main_box);

        main_box.set_orientation(Gtk::Orientation::VERTICAL);
        main_box.set_spacing(5);
        main_box.set_margin(5);

        results_sw.set_vexpand(true);

        editors_frame.set_child(editors_grid);
        settings_frame.set_child(settings_grid);
        button_frame.set_child(button_box);

        results_sw.set_has_frame(true);

        main_box.append(editors_frame);
        main_box.append(settings_frame);
        main_box.append(button_frame);
        main_box.append(results_sw);

        editors_grid.set_row_spacing(5);
        editors_grid.set_column_spacing(5);
        editors_grid.set_margin(5);

        settings_grid.set_row_spacing(5);
        settings_grid.set_column_spacing(5);
        settings_grid.set_margin(5);

        button_box.set_margin(5);

        filemask_l.set_text("File Mask");
        subpath_l.set_text("Project's Subpath");
        grep_l.set_text("Match File Contents");

        filemask_l.set_halign(Gtk::Align::START);
        subpath_l.set_halign(Gtk::Align::START);
        grep_l.set_halign(Gtk::Align::START);

        filemask_w.set_hexpand(true);
        subpath_w.set_hexpand(true);
        query_w.set_hexpand(true);

        editors_grid.attach(filemask_cb, 0, 0);
        editors_grid.attach(filemask_l, 1, 0);
        editors_grid.attach(filemask_w, 2, 0);

        editors_grid.attach(subpath_l, 0, 1, 2, 1);
        editors_grid.attach(subpath_w, 2, 1);

        editors_grid.attach(grep_cb, 0, 2);
        editors_grid.attach(grep_l, 1, 2);
        editors_grid.attach(query_w, 2, 2);

        recurcive_l.set_text("Recurcive");
        max_depth_l.set_text("Max Depth");
        filename_casesensitive_l.set_text("File Mask Case Sensetive");
        query_casesensitive_l.set_text("Contents Grep Case Sensetive");
        query_type_l.set_text("Contents Grep Mode");

        recurcive_l.set_halign(Gtk::Align::START);
        max_depth_l.set_halign(Gtk::Align::START);
        filename_casesensitive_l.set_halign(Gtk::Align::START);
        query_casesensitive_l.set_halign(Gtk::Align::START);

        settings_grid.attach(recurcive_cb, 0, 0);
        settings_grid.attach(recurcive_l, 1, 0, 2, 1);

        settings_grid.attach(max_depth_cb, 0, 1);
        settings_grid.attach(max_depth_l, 1, 1);
        settings_grid.attach(max_depth_w, 2, 1);

        settings_grid.attach(filename_casesensitive_cb, 0, 2);
        settings_grid.attach(filename_casesensitive_l, 1, 2, 2, 1);

        settings_grid.attach(query_casesensitive_cb, 0, 3);
        settings_grid.attach(query_casesensitive_l, 1, 3, 2, 1);

        settings_grid.attach(query_type_l, 1, 4);
        settings_grid.attach(query_type_w, 2, 4, 1, 1);

        recurcive_l.set_hexpand(true);
        max_depth_w.set_hexpand(true);
        filename_casesensitive_l.set_hexpand(true);
        query_casesensitive_l.set_hexpand(true);
        query_type_w.set_hexpand(true);

        button_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        button_box.set_spacing(5);

        start_btn.set_label("Start");
        stop_btn.set_label("Stop");

        button_box.append(start_btn);
        button_box.append(stop_btn);
    }

    FindFile::~FindFile()
    {
    }

} // namespace codeeditor
} // namespace wayround_i2p
