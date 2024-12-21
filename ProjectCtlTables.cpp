#include "ProjectCtlTables.hpp"

#include <iostream>

#include "CodeEditorAbstract.hpp"
#include "ProjectCtl.hpp"
#include "WorkSubject.hpp"

namespace wayround_i2p::ccedit
{

WorkSubjectTableRowWidget::WorkSubjectTableRowWidget()
{
    set_spacing(5);

    button_box.add_css_class("linked");

    edited_indicator.set_text("*");

    button_box.append(btn_close);
    button_box.append(btn_open_editor);

    btn_close.set_image_from_icon_name("window-close");
    btn_close.set_tooltip_text("Close");

    btn_open_editor.set_image_from_icon_name("window-new");
    btn_open_editor.set_tooltip_text("Show Existing or Open New Window");

    append(button_box);
    append(edited_indicator);
    append(path);

    ws_changed_slot->setFun(
        [this]()
        { this->on_ws_changed(); }
    );

    btn_close.signal_clicked().connect(
        [this]()
        {
            on_btn_close();
        }
    );

    btn_open_editor.signal_clicked().connect(
        [this]()
        {
            on_btn_open_editor();
        }
    );
}

WorkSubjectTableRowWidget::~WorkSubjectTableRowWidget()
{
}

void WorkSubjectTableRowWidget::bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    {
        auto list_item_item = list_item->get_item();
        if (!list_item_item)
        {
            std::cout << "WorkSubjectTableRowWidget::bind !list_item_item" << std::endl;
            return;
        }

        auto li = std::dynamic_pointer_cast<TableItemTpl<WorkSubject_shared>>(
            list_item_item
        );
        if (!li)
        {
            std::cout << "WorkSubjectTableRowWidget::bind !li" << std::endl;
            return;
        }

        std::cout << "WorkSubjectTableRowWidget::bind is ok" << std::endl;

        bond_ws = li->value;
    }

    // todo: track path change in WorkSubject
    path.set_text(bond_ws->getPath().string());

    bond_ws->signal_modified_changed().connect(
        ws_changed_slot
    );

    update_labels();
}

void WorkSubjectTableRowWidget::unbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    bond_ws.reset();
}

void WorkSubjectTableRowWidget::update_labels()
{
    edited_indicator.set_visible(bond_ws->modified());
}

void WorkSubjectTableRowWidget::on_btn_close()
{
    bond_ws->destroy();
}

void WorkSubjectTableRowWidget::on_btn_open_editor()
{
    auto ed = bond_ws->getProject()->workSubjectExistingOrNewEditor(bond_ws);
    ed->show();
}

void WorkSubjectTableRowWidget::on_ws_changed()
{
    update_labels();
}

CodeEditorTableRowWidget::CodeEditorTableRowWidget()
{
    set_spacing(5);

    // btn_grid.add_css_class("linked");

    btn_close.set_image_from_icon_name("window-close");
    btn_close.set_tooltip_text("Close");

    btn_show.set_image_from_icon_name("window-new");
    btn_show.set_tooltip_text("Show Window");

    btn_up.set_image_from_icon_name("go-up");
    btn_down.set_image_from_icon_name("go-down");

    btn_grid.attach(btn_close, 1, 0);
    btn_grid.attach(btn_show, 1, 1);
    btn_grid.attach(btn_up, 0, 0);
    btn_grid.attach(btn_down, 0, 1);

    append(btn_grid);
    append(path);

    btn_close.signal_clicked().connect(
        [this]()
        { on_btn_close(); }
    );

    btn_show.signal_clicked().connect(
        [this]()
        { on_btn_show(); }
    );

    btn_up.signal_clicked().connect(
        [this]()
        { on_btn_up(); }
    );

    btn_down.signal_clicked().connect(
        [this]()
        { on_btn_down(); }
    );
}

CodeEditorTableRowWidget::~CodeEditorTableRowWidget()
{
}

void CodeEditorTableRowWidget::bind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    {
        auto list_item_item = list_item->get_item();
        if (!list_item_item)
        {
            std::cout << "CodeEditorTableRowWidget::bind !list_item_item" << std::endl;
            return;
        }

        auto li = std::dynamic_pointer_cast<TableItemTpl<CodeEditorAbstract_shared>>(
            list_item_item
        );
        if (!li)
        {
            std::cout << "CodeEditorTableRowWidget::bind !li" << std::endl;
            return;
        }

        std::cout << "CodeEditorTableRowWidget::bind is ok" << std::endl;

        bond_ed = li->value;
    }

    // todo: track path change in WorkSubject
    path.set_text(bond_ed->getWorkSubject()->getPath().string());

    /*
    bond_ed->signal_modified_changed().connect(
        [this]()
        { this->on_ws_changed(); }
    );*/

    // update_labels();
}

void CodeEditorTableRowWidget::unbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    bond_ed.reset();
}

void CodeEditorTableRowWidget::on_btn_show()
{
    bond_ed->show();
}

void CodeEditorTableRowWidget::on_btn_close()
{
    bond_ed->destroy();
}

void CodeEditorTableRowWidget::on_btn_up()
{
    bond_ed->getProjectCtl()->editorShiftUp(bond_ed);
}

void CodeEditorTableRowWidget::on_btn_down()
{
    bond_ed->getProjectCtl()->editorShiftDown(bond_ed);
}

} // namespace wayround_i2p::ccedit
