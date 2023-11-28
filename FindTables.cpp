#include "FindTables.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    FindFileResultTreeItem::FindFileResultTreeItem(
        std::filesystem::path subpath
    ) :
        Glib::ObjectBase(typeid(FindFileResultTreeItem)),
        subpath(subpath)
    {
        items = Gio::ListStore<FindFileResultTreeItemItem>::create();
    }

    FindFileResultTreeItem::~FindFileResultTreeItem()
    {
        std::cout << "~FindFileResultTreeItem()" << std::endl;
    }

    FindFileResultTreeItemItemP FindFileResultTreeItem::create_item(
        unsigned int line,
        std::string  text,
        unsigned int start_offset,
        unsigned int end_offset
    )
    {
        auto x = FindFileResultTreeItemItem::create(
            line,
            text,
            start_offset,
            end_offset
        );
        items->append(x);
        return x;
    }

    Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>>
        FindFileResultTreeItem::get_list_store()
    {
        return items;
    }

    // ---------------------

    FindFileResultTreeItemItemP FindFileResultTreeItemItem::create(
        // FindFileResultTreeItemP tree_item,
        unsigned int line,
        std::string  text,
        unsigned int start_offset,
        unsigned int end_offset
    )
    {
        auto ret = Glib::make_refptr_for_instance<FindFileResultTreeItemItem>(
            new FindFileResultTreeItemItem(
                line,
                text,
                start_offset,
                end_offset
            )
        );
        return ret;
    }

    FindFileResultTreeItemItem::FindFileResultTreeItemItem(
        unsigned int line,
        std::string  text,
        unsigned int start_offset,
        unsigned int end_offset
    ) :
        Glib::ObjectBase(typeid(FindFileResultTreeItemItem)),
        line(line),
        text(text),
        start_offset(start_offset),
        end_offset(end_offset)
    {
    }

    FindFileResultTreeItemItem::~FindFileResultTreeItemItem()
    {
        std::cout << "~FindFileResultTreeItemItem()" << std::endl;
    }

    // ---------------------

    FindFileResultTreeItemWidget::FindFileResultTreeItemWidget(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
        set_orientation(Gtk::Orientation::HORIZONTAL);
        set_spacing(5);

        append(subpath);
        append(found_count);
    }

    FindFileResultTreeItemWidget::~FindFileResultTreeItemWidget()
    {
    }

    void FindFileResultTreeItemWidget::bind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
        auto list_item_item = list_item->get_item();

        auto ti = std::dynamic_pointer_cast<FindFileResultTreeItem>(
            list_item_item
        );
        if (!ti)
        {
            return;
        }
        subpath.set_text(ti->subpath.string());
    }

    void FindFileResultTreeItemWidget::unbind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
    }

    // ---------------------

    FindFileResultTreeItemItemWidget::FindFileResultTreeItemItemWidget(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
        set_orientation(Gtk::Orientation::HORIZONTAL);
        set_spacing(5);

        append(line);
        append(text);
    }

    FindFileResultTreeItemItemWidget::~FindFileResultTreeItemItemWidget()
    {
    }

    void FindFileResultTreeItemItemWidget::bind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
        auto list_item_item = list_item->get_item();

        auto ti = std::dynamic_pointer_cast<FindFileResultTreeItemItem>(
            list_item_item
        );
        if (!ti)
        {
            return;
        }
        // todo: something better is needed than std::format
        line.set_text(std::format("{}", ti->line));
        text.set_text(ti->text);
    }

    void FindFileResultTreeItemItemWidget::unbind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
    }
} // namespace codeeditor
} // namespace wayround_i2p
