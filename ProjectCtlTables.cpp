#include "ProjectCtlTables.hpp"

#include <iostream>

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
        [this]()
        { this->on_ws_changed(); }
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
    bond_ws->getProject()->workSubjectExistingOrNewEditor(bond_ws);
}

void WorkSubjectTableRowWidget::on_ws_changed()
{
    update_labels();
}

CodeEditorTableRowWidget::CodeEditorTableRowWidget()
{
    set_spacing(5);

    button_box1.add_css_class("linked");
    button_box2.add_css_class("linked");

    button_box1.append(btn_close);
    button_box1.append(btn_show);

    button_box2.append(btn_up);
    button_box2.append(btn_down);

    append(button_box1);
    append(button_box2);
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
}

void CodeEditorTableRowWidget::unbind(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
}

void CodeEditorTableRowWidget::on_btn_show()
{
}

void CodeEditorTableRowWidget::on_btn_close()
{
}

void CodeEditorTableRowWidget::on_btn_up()
{
}

void CodeEditorTableRowWidget::on_btn_down()
{
}

} // namespace wayround_i2p::ccedit
