#include "FileExplorerWidget.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorerWidget::FileExplorerWidget(std::shared_ptr<ProjectCtl> proj_ctl)
{
    this->proj_ctl = proj_ctl;

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    path_box.set_spacing(5);

    go_home_btn.set_label("home");
    exit_folder_btn.set_label("..");

    path_box.append(go_home_btn);
    path_box.append(exit_folder_btn);
    path_box.append(path_entry);

    set_orientation(Gtk::Orientation::VERTICAL);
    set_spacing(5);

    append(path_box);
    append(file_list_sw);

    file_list_sw.set_child(file_list_view);

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
