#include "FindTables.hpp"

namespace wayround_i2p
{
namespace ccedit
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
        std::filesystem::path subpath,
        unsigned int          line,
        std::string           text,
        unsigned int          start_offset,
        unsigned int          end_offset
    )
    {
        auto x = FindFileResultTreeItemItem::create(
            subpath,
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
        std::filesystem::path subpath,
        unsigned int          line,
        std::string           text,
        unsigned int          start_offset,
        unsigned int          end_offset
    )
    {
        auto ret = Glib::make_refptr_for_instance<FindFileResultTreeItemItem>(
            new FindFileResultTreeItemItem(
                subpath,
                line,
                text,
                start_offset,
                end_offset
            )
        );
        return ret;
    }

    FindFileResultTreeItemItem::FindFileResultTreeItemItem(
        std::filesystem::path subpath,
        unsigned int          line,
        std::string           text,
        unsigned int          start_offset,
        unsigned int          end_offset
    ) :
        Glib::ObjectBase(typeid(FindFileResultTreeItemItem)),
        subpath(subpath),
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
        const Glib::RefPtr<Gtk::ListItem>                &list_item,
        std::function<void(FindFileResultTreeItemP item)> go_action
    )
    {
        this->go_action = go_action;

        set_orientation(Gtk::Orientation::HORIZONTAL);
        set_spacing(5);
        go_btn.set_label("go");

        append(go_btn);
        append(subpath);
        append(found_count);

        go_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindFileResultTreeItemWidget::on_go_btn)
        );
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
        this->item = ti;
        subpath.set_text(ti->subpath.string());
    }

    void FindFileResultTreeItemWidget::unbind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
    }

    void FindFileResultTreeItemWidget::on_go_btn()
    {
        if (go_action)
        {
            go_action(this->item);
        }
    }

    // ---------------------

    FindFileResultTreeItemItemWidget::FindFileResultTreeItemItemWidget(
        const Glib::RefPtr<Gtk::ListItem>                    &list_item,
        std::function<void(FindFileResultTreeItemItemP item)> go_action
    )
    {
        this->go_action = go_action;

        set_orientation(Gtk::Orientation::HORIZONTAL);
        set_spacing(5);
        go_btn.set_label("go");

        append(line);
        append(go_btn);
        append(text);

        go_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindFileResultTreeItemItemWidget::on_go_btn)
        );
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
        this->item = ti;

        // todo: something better is needed than std::format
        line.set_text(std::format("{}", ti->line));
        text.set_text(ti->text);
    }

    void FindFileResultTreeItemItemWidget::unbind(
        const Glib::RefPtr<Gtk::ListItem> &list_item
    )
    {
    }

    void FindFileResultTreeItemItemWidget::on_go_btn()
    {
        if (go_action)
        {
            go_action(this->item);
        }
    }
} // namespace ccedit
} // namespace wayround_i2p
