#ifndef FIND_TABLES_HPP
#define FIND_TABLES_HPP

#include <filesystem>

#include "FindTypes.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class FindFileResultTreeItem;
    class FindFileResultTreeItemItem;

    using FindFileResultTreeItemP     = Glib::RefPtr<FindFileResultTreeItem>;
    using FindFileResultTreeItemItemP = Glib::RefPtr<FindFileResultTreeItemItem>;

    class FindFileResultTreeItem : public Glib::Object
    {
      public:
        static Glib::RefPtr<FindFileResultTreeItem> create(
            std::filesystem::path subpath
        );

        ~FindFileResultTreeItem();

        const std::filesystem::path subpath;

      protected:
        FindFileResultTreeItem(
            std::filesystem::path subpath
        );

      private:
        Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>> items;

      public: // methods
        FindFileResultTreeItemItemP create_item(
            std::filesystem::path subpath,
            unsigned int          line,
            std::string           text,
            unsigned int          start_offset,
            unsigned int          end_offset
        );

        Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>>
            get_list_store();
    };

    class FindFileResultTreeItemItem : public Glib::Object
    {
      public:
        static FindFileResultTreeItemItemP create(
            std::filesystem::path subpath,
            unsigned int          line,
            std::string           text,
            unsigned int          start_offset,
            unsigned int          end_offset
        );

        ~FindFileResultTreeItemItem();

        std::filesystem::path subpath;
        const unsigned int    line;
        const std::string     text;
        unsigned int          start_offset;
        unsigned int          end_offset;

      protected:
        FindFileResultTreeItemItem(
            std::filesystem::path subpath,
            unsigned int          line,
            std::string           text,
            unsigned int          start_offset,
            unsigned int          end_offset
        );
    };

    class FindFileResultTreeItemWidget : public Gtk::Box
    {
      public:
        FindFileResultTreeItemWidget(
            const Glib::RefPtr<Gtk::ListItem>                &list_item,
            std::function<void(FindFileResultTreeItemP item)> go_action
        );
        ~FindFileResultTreeItemWidget();

        void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
        void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

      private:
        Gtk::Label  subpath;
        Gtk::Label  found_count;
        Gtk::Button go_btn;

        FindFileResultTreeItemP item;

        std::function<void(FindFileResultTreeItemP item)> go_action;

        void on_go_btn();
    };

    class FindFileResultTreeItemItemWidget : public Gtk::Box
    {
      public:
        FindFileResultTreeItemItemWidget(
            const Glib::RefPtr<Gtk::ListItem>                    &list_item,
            std::function<void(FindFileResultTreeItemItemP item)> go_action
        );
        ~FindFileResultTreeItemItemWidget();

        void bind(const Glib::RefPtr<Gtk::ListItem> &list_item);
        void unbind(const Glib::RefPtr<Gtk::ListItem> &list_item);

      private:
        Gtk::Label  line;
        Gtk::Label  text;
        Gtk::Button go_btn;

        FindFileResultTreeItemItemP item;

        std::function<void(FindFileResultTreeItemItemP item)> go_action;

        void on_go_btn();
    };

} // namespace codeeditor
} // namespace wayround_i2p
#endif
