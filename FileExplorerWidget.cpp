#include "FileExplorerWidget.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorerWidget::FileExplorerWidget(std::shared_ptr<ProjectCtl> proj_ctl)
{
    this->proj_ctl = proj_ctl;

    selected_file_lbl.set_text("<filename>");

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);

    go_home_btn.set_label("home");
    exit_folder_btn.set_label("..");

    path_box.append(go_home_btn);
    path_box.append(exit_folder_btn);
    path_box.append(refresh_btn);
    path_entry.set_margin_start(5);
    path_box.append(path_entry);

    set_orientation(Gtk::Orientation::VERTICAL);
    set_spacing(5);

    lists_box.set_start_child(dir_tree_sw);
    lists_box.set_end_child(file_list_sw);

    lists_box.set_wide_handle(true);

    lists_box.set_vexpand(true);
    lists_box.set_hexpand(true);

    dir_tree_sw.set_has_frame(true);
    file_list_sw.set_has_frame(true);

    dir_tree_sw.set_child(dir_tree_view);
    file_list_sw.set_child(file_list_view);

    append(path_box);
    append(lists_box);
    append(selected_file_lbl);

    signal_destroy().connect(
        sigc::mem_fun(*this, &FileExplorerWidget::on_destroy_sig)
    );
}

FileExplorerWidget::~FileExplorerWidget()
{
    std::cout << "~FileExplorerWidget()" << std::endl;
}

void FileExplorerWidget::on_destroy_sig()
{
    std::cout << "FileExplorerWidget sig destroy" << std::endl;
}

// Gtk::Widget *FileExplorerWidget::getRootWidget()
// {
//     return dynamic_cast<Gtk::Widget *>(main_box);
// }
