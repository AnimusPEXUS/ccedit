#include "FileExplorer.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorer::FileExplorer(std::shared_ptr<ProjectCtl> proj_ctl) :
    main_box(Gtk::Orientation::VERTICAL, 5)
{
    this->proj_ctl = proj_ctl;

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    selected_file_lbl.set_text("<filename>");
    selected_file_lbl.set_hexpand(true);

    path_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    // path_box.set_spacing(5);

    go_home_btn.set_label("home");
    exit_folder_btn.set_label("..");

    path_box.append(go_home_btn);
    path_box.append(exit_folder_btn);
    path_box.append(refresh_btn);
    path_entry.set_margin_start(5);
    path_box.append(path_entry);

    lists_box.set_start_child(dir_tree_sw);
    lists_box.set_end_child(file_list_sw);

    lists_box.set_wide_handle(true);

    lists_box.set_vexpand(true);
    lists_box.set_hexpand(true);

    dir_tree_sw.set_has_frame(true);
    file_list_sw.set_has_frame(true);

    dir_tree_sw.set_child(dir_tree_view);
    file_list_sw.set_child(file_list_view);

    temp_file_selector_btn.set_label("Browse..");
    temp_file_open_btn.set_label("Open");

    temp_file_selector_box.append(temp_file_selector_btn);
    temp_file_selector_box.append(selected_file_lbl);
    temp_file_selector_box.append(temp_file_open_btn);

    main_box.append(path_box);
    main_box.append(lists_box);
    main_box.append(temp_file_selector_box);

    set_child(main_box);

    signal_destroy().connect(
        sigc::mem_fun(*this, &FileExplorer::on_destroy_sig)
    );

    temp_file_selector_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_temp_file_selector_btn)
    );

    temp_file_open_btn.signal_clicked().connect(
        sigc::mem_fun(*this, &FileExplorer::on_temp_file_open_btn)
    );
}

FileExplorer::~FileExplorer()
{
    std::cout << "~FileExplorer()" << std::endl;
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    delete this;
    std::cout << "FileExplorer sig destroy exit" << std::endl;
}

void FileExplorer::on_temp_file_selector_btn()
{
    select_file_dialog = Gtk::FileDialog::create();
    select_file_dialog->set_title("select a file to edit");
    select_file_dialog->open(
        *this,
        sigc::mem_fun(
            *this,
            &FileExplorer::on_temp_file_selector_btn_finish
        )
    );
}

void FileExplorer::on_temp_file_selector_btn_finish(
    std::shared_ptr<Gio::AsyncResult> res
)
{
    if (res == NULL)
    {
        // std::cout << "res == NULL\n";
        return;
    }

    // std::cout << "on_browse_click_finish\n";
    auto result = select_file_dialog->open_finish(res);

    if (result == NULL)
    {
        // std::cout << "result == NULL\n";
        return;
    }

    //  std::cout << format("path: {}\n", result->get_path());
    //  // cout << format("path: {}",result->get_path());
    //  std::cout.flush();
    //  //
    //

    selected_file_lbl.set_text(result->get_path());

    select_file_dialog.reset();
}

void FileExplorer::on_temp_file_open_btn()
{
    std::filesystem::path x(selected_file_lbl.get_text());
    proj_ctl->workSubjectEnsureExistance(x);
    proj_ctl->workSubjectNewEditor(x);
}
