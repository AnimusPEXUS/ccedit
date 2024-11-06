

#include <cassert>
#include <format>
#include <iostream>

#include "ProjectMgrEditor.hpp"

namespace wayround_i2p::ccedit
{

ProjectMgrEditor_shared create(
    Controller_shared controller,
    std::string                 proj_name,
    std::filesystem::path       proj_path
)
{
    auto ret = ProjectMgrEditor_shared(
        new ProjectMgrEditor(
            controller,
            proj_name,
            proj_path
        )
    );
    ret->own_ptr = ret;
    return ret;
}

ProjectMgrEditor::ProjectMgrEditor(
    Controller_shared controller,
    std::string                 proj_name,
    std::filesystem::path       proj_path
)
{
    if (!controller)
    {
        throw "controller is required";
    }

    // TODO: use set_titlebar and put buttons to title

    this->controller = controller;

    {
        auto wt = "adding new project - Code Editor";
        if (proj_name.length() != 0)
        {
            wt = std::format("rename project {} - Code Editor", proj_name);
        }
        win.set_title(wt);
    }

    win.set_child(main_box);

    main_box.set_vexpand(true);
    main_box.set_hexpand(true);
    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    main_box.set_spacing(5);

    main_box.set_margin_top(5);
    main_box.set_margin_start(5);
    main_box.set_margin_end(5);
    main_box.set_margin_bottom(5);

    main_box.append(main_grid);
    main_box.append(button_box);

    proj_name_orig = proj_name;
    proj_path_orig = proj_path;

    main_grid.set_vexpand(true);
    main_grid.set_hexpand(true);
    main_grid.set_row_spacing(5);
    main_grid.set_column_spacing(5);

    main_grid.attach(name_label, 0, 0, 1, 1);
    main_grid.attach(path_label, 0, 1, 1, 1);
    main_grid.attach(project_name, 1, 0, 2, 1);
    main_grid.attach(project_path, 1, 1, 1, 1);
    main_grid.attach(btn_browse, 2, 1, 1, 1);

    name_label.set_text("name");
    path_label.set_text("path");

    btn_browse.set_label("browse..");

    project_name.set_text(proj_name);
    project_path.set_text(proj_path.string());

    project_name.set_hexpand(true);
    project_path.set_hexpand(true);

    btn_ok.set_label("ok");
    btn_cancel.set_label("cancel");

    button_box.set_spacing(5);

    button_box.append(btn_ok);
    button_box.append(btn_cancel);

    btn_ok.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgrEditor::on_ok_click)
    );

    btn_cancel.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgrEditor::on_cancel_click)
    );

    btn_browse.signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectMgrEditor::on_browse_click)
    );

    signal_destroy().connect(
        sigc::mem_fun(*this, &ProjectMgrEditor::on_destroy_sig)
    );
}

ProjectMgrEditor::~ProjectMgrEditor()
{
    std::cout << "~ProjectMgrEditor()" << std::endl;
}

void ProjectMgrEditor::on_ok_click()
{
    assert(controller != NULL);
    auto new_name = project_name.get_text();
    auto new_path = project_path.get_text();

    if (proj_name_orig == "")
    {
        auto err = controller->createProject(
            new_name,
            std::filesystem::path(new_path),
            false
        );
    }
    else
    {
        // todo: force ColumnView redraw
        auto err = controller->editProject(
            proj_name_orig,
            new_name,
            std::filesystem::path(new_path)
        );
    }

    close();
}

void ProjectMgrEditor::on_cancel_click()
{
    close();
}

void ProjectMgrEditor::on_browse_click()
{
    select_dir_dialog = Gtk::FileDialog::create();
    select_dir_dialog->set_title("select a project directory");
    select_dir_dialog->select_folder(
        *this,
        sigc::mem_fun(*this, &ProjectMgrEditor::on_browse_click_finish)
    );
}

void ProjectMgrEditor::on_browse_click_finish(
    std::shared_ptr<Gio::AsyncResult> res
)
{
    #error "don't use shared_ptr for Gio::AsyncResult"
    if (res == NULL)
    {
        // std::cout << "res == NULL\n";
        return;
    }

    // std::cout << "on_browse_click_finish\n";
    auto result = select_dir_dialog->select_folder_finish(res);

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

    project_path.set_text(result->get_path());

    select_dir_dialog.reset();
}

void ProjectMgrEditor::on_destroy_sig()
{
    delete (this);
}

} // namespace wayround_i2p::ccedit
