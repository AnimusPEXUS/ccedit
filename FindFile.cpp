
#include <algorithm>
#include <experimental/scope>
#include <queue>

#include <fnmatch.h>

#include "FindFile.hpp"
#include "utils.hpp"

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

        maximize();

        set_child(main_box);

        main_box.set_orientation(Gtk::Orientation::VERTICAL);
        main_box.set_spacing(5);
        main_box.set_margin(5);

        results_pan.set_vexpand(true);

        editors_frame.set_child(editors_grid);
        settings_frame.set_child(settings_grid);
        button_frame.set_child(button_box);

        main_box.append(editors_frame);
        main_box.append(settings_frame);
        main_box.append(button_frame);
        main_box.append(results_pan);

        editors_grid.set_row_spacing(5);
        editors_grid.set_column_spacing(5);
        editors_grid.set_margin(5);
        editors_grid.set_row_homogeneous(true);

        settings_grid.set_row_spacing(5);
        settings_grid.set_column_spacing(5);
        settings_grid.set_margin(5);
        settings_grid.set_row_homogeneous(true);

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

        results_pan.set_start_child(result_files_sw);
        results_pan.set_end_child(result_lines_sw);

        result_files_sw.set_child(result_files);
        result_lines_sw.set_child(result_lines);

        setup_result_filelist();
        setup_result_linelist();

        start_btn.signal_clicked().connect(
            sigc::mem_fun(*this, &FindFile::on_start_btn)
        );

        signal_destroy().connect(
            sigc::mem_fun(*this, &FindFile::on_destroy_sig)
        );
    }

    void FindFile::setup_result_filelist()
    {
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto w = Gtk::make_managed<FindFileResultTreeItemWidget>(list_item);
                list_item->set_child(*w);
            }
        );
        factory->signal_bind().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto list_item_child = list_item->get_child();

                auto tic = dynamic_cast<FindFileResultTreeItemWidget *>(
                    list_item_child
                );
                if (!tic)
                {
                    return;
                }
                tic->bind(list_item);
            }
        );
        result_files.set_factory(factory);
    }

    void FindFile::setup_result_linelist()
    {
    }

    FindFile::~FindFile()
    {
        std::cout << "~FindFile()" << std::endl;
    }

    std::tuple<std::filesystem::path, int> FindFile::getProjectPath()
    {
        return p_ctl->getProjectPath();
    }

    void FindFile::on_start_btn()
    {
        std::cout << "on_start_btn()" << std::endl;
        start_search_thread();
    }

    int FindFile::start_search_thread()
    {
        if (search_working)
        {
            return 1;
        }
        search_stop_flag = false;

        // todo: thread
        search_thread();
        return 0;
    }

    void FindFile::stop_search_thread()
    {
        search_stop_flag = true;
    }

    void FindFile::search_thread()
    {
        int err = 0;

        auto matched_files = Gio::ListStore<FindFileResultTreeItem>::create();
        auto sel           = Gtk::SingleSelection::create();
        sel->set_model(matched_files);
        result_files.set_model(sel);

        std::filesystem::path proj_path;

        std::tie(proj_path, err) = getProjectPath();
        if (err != 0)
        {
            // todo: report
            return;
        }

        std::queue<std::filesystem::path> dir_subpaths_to_search_q;
        std::queue<std::filesystem::path> files_to_grep_q;

        dir_subpaths_to_search_q.push("/");

        while (!dir_subpaths_to_search_q.empty())
        {
            auto              work_path = dir_subpaths_to_search_q.front();
            typeof(work_path) work_path_full;

            work_path      = work_path.relative_path().lexically_normal();
            work_path_full = proj_path / work_path;

            auto dir        = Gio::File::create_for_path(work_path_full);
            auto dir_exists = dir->query_exists();
            if (!dir_exists)
            {
                // todo: report
                continue;
            }

            auto dir_type = dir->query_file_type();
            if (dir_type != Gio::FileType::DIRECTORY)
            {
                // todo: report
                continue;
            }

            auto enumerator = dir->enumerate_children();
            std::cout << "auto enumerator = dir->enumerate_children();" << std::endl;

            auto sg02 = std::experimental::fundamentals_v3::scope_exit(
                [&enumerator]()
                {
                    enumerator->close();
                }
            );

            std::vector<std::filesystem::path> matched_filenames;

            for (;;)
            {
                auto fi = enumerator->next_file();
                if (fi == nullptr)
                {
                    break;
                }
                auto name = fi->get_name();
                auto type = fi->get_file_type();

                auto work_path_full_name     = work_path_full / name;
                auto work_path_full_name_rel = std::filesystem::relative(
                    work_path_full_name,
                    work_path_full
                );

                if (type == Gio::FileType::DIRECTORY)
                {
                    dir_subpaths_to_search_q.push(work_path_full_name_rel);
                }
                else
                {
                    if (fnmatch(
                            "*",
                            (work_time_query.use_fnmatch_on_path_part ? work_path_full_name_rel.c_str() : name.c_str()),
                            0
                        )
                        == 0)
                    {
                        matched_filenames.push_back(work_path_full_name_rel);
                    }
                }
            }

            for (auto i = matched_filenames.begin();
                 i != matched_filenames.end();
                 i++)
            {
                auto x = FindFileResultTreeItem::create(*i);
                if (work_time_query.search_contents)
                {
                    if (search_thread_search_contents(x) != 0)
                    {
                        // todo: report error
                        continue;
                    }
                    if (x->get_list_store()->get_n_items() == 0)
                    {
                        continue;
                    }
                }

                // Glib::RefPtr<FindFileResultTreeItem>

                matched_files->append(x);
            }
        }
    }

    int FindFile::search_thread_search_contents(
        FindFileResultTreeItemP item
    )
    {
        int err = 0;

        std::filesystem::path proj_path;

        std::tie(proj_path, err) = getProjectPath();
        if (err != 0)
        {
            // todo: report
            return 1;
        }

        std::string cont_txt;

        auto item_subpath = item->subpath;

        std::tie(cont_txt, err) = loadStringFromFile(
            proj_path
            / item_subpath
        );
        if (err != 0)
        {
            // todo: report
            return 1;
        }

        auto cont_txt_ls = LineStarts(cont_txt);

        switch (work_time_query.contents_search_method)
        {
        default:
            return 1;
            break;
        case PLAIN:
        {
            std::string testing_text(cont_txt);
            std::string subj(work_time_query.contents);

            auto i = testing_text.begin();

            for (;;)
            {
                i = std::search(i + 1, testing_text.end(), subj.begin(), subj.end());

                if (i == testing_text.end())
                {
                    break;
                }
                item->create_item(
                    cont_txt_ls.getLineByOffset(
                        std::distance(testing_text.begin(), i)
                    ),
                    "???"
                );
            }
            break;
        }
        }
        return 0;
    }

    void FindFile::on_destroy_sig()
    {
        own_ptr.reset();
    }

    FindFileResultTreeItemP FindFileResultTreeItem::create(
        std::filesystem::path subpath
    )
    {
        auto ret = Glib::make_refptr_for_instance<FindFileResultTreeItem>(
            new FindFileResultTreeItem(
                subpath
            )
        );
        ret->own_ptr = ret;
        return ret;
    }

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
    }

    FindFileResultTreeItemItemP FindFileResultTreeItem::create_item(
        unsigned int line,
        std::string  text
    )
    {
        auto x = FindFileResultTreeItemItem::create(
            own_ptr,
            line,
            text
        );
        items->append(x);
        return x;
    }

    Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>> FindFileResultTreeItem::get_list_store()
    {
        return items;
    }

    FindFileResultTreeItemItemP FindFileResultTreeItemItem::create(
        FindFileResultTreeItemP tree_item,
        unsigned int            line,
        std::string             text
    )
    {
        auto ret = Glib::make_refptr_for_instance<FindFileResultTreeItemItem>(
            new FindFileResultTreeItemItem(
                tree_item,
                line,
                text
            )
        );
        // ret->own_ptr = ret;
        return ret;
    }

    FindFileResultTreeItemItem::FindFileResultTreeItemItem(
        FindFileResultTreeItemP tree_item,
        unsigned int            line,
        std::string             text
    ) :
        Glib::ObjectBase(typeid(FindFileResultTreeItemItem)),
        line(line),
        text(text)

    {
        this->tree_item = tree_item;
    }

    FindFileResultTreeItemItem::~FindFileResultTreeItemItem()
    {
    }

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

        auto ti = std::dynamic_pointer_cast<FindFileResultTreeItem>(list_item_item);
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

} // namespace codeeditor
} // namespace wayround_i2p
