
#include <algorithm>
#include <experimental/scope>
#include <format>
#include <future>
#include <queue>
#include <thread>

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

        editors_frame.set_child(editors_grid);
        files_settings_frame.set_child(files_settings_box);
        contents_settings_frame.set_child(contents_settings_box);
        button_frame.set_child(button_box);

        main_box.append(editors_frame);
        main_box.append(files_settings_frame);
        main_box.append(contents_settings_frame);
        main_box.append(button_frame);
        main_box.append(results_pan);

        editors_grid.set_row_spacing(5);
        editors_grid.set_column_spacing(5);
        editors_grid.set_margin(5);
        editors_grid.set_row_homogeneous(true);

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

        editors_grid.attach(filemask_l, 0, 0);
        editors_grid.attach(filemask_w, 1, 0);

        editors_grid.attach(subpath_l, 0, 1);
        editors_grid.attach(subpath_w, 1, 1);

        editors_grid.attach(grep_l, 0, 2);
        editors_grid.attach(query_w, 1, 2);

        use_fnmatch_on_path_part_cb.set_label("File Path Checked By Mask Too");
        use_fnmatch_on_path_part_cb.set_tooltip_text("File Mask Checks not only filename, but entire path, starting from project root");
        recurcive_cb.set_label("Recurcive");
        delve_hidden_cb.set_label("Delve into Hidden Dirs");
        delve_hidden_cb.set_tooltip_text("dirs, which names starting with dot");
        max_depth_cb.set_label("Max Depth");
        fnmatch_cs_cb.set_label("File Mask Case Sensitive");
        query_casesensitive_cb.set_label("Contents Grep Case Sensitive");
        search_contents_cb.set_label("Search File Contents");
        one_content_match_is_enough_cb.set_label("1 Content Match is Enough");
        one_content_match_is_enough_cb.set_tooltip_text("stop grepping current file and proceed to next");
        dont_show_files_with_0_contents_match_cb.set_label("Don't Display Files with 0 content matches");

        query_type_l.set_text("Contents Grep Mode");

        max_depth_sb.set_range(0, 1024);
        max_depth_sb.set_increments(1, 1);

        max_depth_box.set_spacing(5);
        max_depth_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        max_depth_box.append(max_depth_cb);
        max_depth_box.append(max_depth_sb);

        files_settings_box.append(use_fnmatch_on_path_part_cb);
        files_settings_box.append(recurcive_cb);
        files_settings_box.append(delve_hidden_cb);
        files_settings_box.append(max_depth_box);
        files_settings_box.append(fnmatch_cs_cb);

        query_type_box.set_spacing(5);
        query_type_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        query_type_box.append(query_type_l);
        query_type_box.append(query_type_lb);

        contents_settings_box.append(search_contents_cb);
        contents_settings_box.append(query_casesensitive_cb);
        contents_settings_box.append(query_type_box);
        contents_settings_box.append(one_content_match_is_enough_cb);
        contents_settings_box.append(dont_show_files_with_0_contents_match_cb);

        button_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        button_box.set_spacing(5);

        start_btn.set_label("Start");
        stop_btn.set_label("Stop");

        button_box.append(start_btn);
        button_box.append(stop_btn);

        results_pan.set_vexpand(true);

        results_pan.set_start_child(result_files_sw);
        results_pan.set_end_child(result_lines_sw);

        result_files_sw.set_child(result_files);
        result_lines_sw.set_child(result_lines);

        result_files.set_single_click_activate(true);

        setup_result_filelist();
        setup_result_linelist();

        setFindFileQuery(FindFileQuery());

        result_files.signal_activate().connect(
            sigc::mem_fun(*this, &FindFile::on_filelist_activate)
        );

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
                auto w = Gtk::make_managed<FindFileResultTreeItemWidget>(
                    list_item
                );
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
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto w = Gtk::make_managed<FindFileResultTreeItemItemWidget>(
                    list_item
                );
                list_item->set_child(*w);
            }
        );
        factory->signal_bind().connect(
            [](const Glib::RefPtr<Gtk::ListItem> &list_item)
            {
                auto list_item_child = list_item->get_child();

                auto tic = dynamic_cast<FindFileResultTreeItemItemWidget *>(
                    list_item_child
                );
                if (!tic)
                {
                    return;
                }
                tic->bind(list_item);
            }
        );
        result_lines.set_factory(factory);
    }

    FindFile::~FindFile()
    {
        std::cout << "~FindFile()" << std::endl;
    }

    FindFileQuery FindFile::getFindFileQuery()
    {
        FindFileQuery ret;
        ret.fnmatch_pattern                       = filemask_w.get_text();
        ret.fnmatch_cs                            = fnmatch_cs_cb.get_active();
        ret.use_fnmatch_on_path_part              = use_fnmatch_on_path_part_cb.get_active();
        ret.recurcive                             = recurcive_cb.get_active();
        ret.delve_into_hidden_dirs                = delve_hidden_cb.get_active();
        ret.subpath                               = subpath_w.get_text();
        ret.use_max_depth                         = max_depth_cb.get_active();
        ret.max_depth                             = max_depth_sb.get_value_as_int();
        ret.search_contents                       = search_contents_cb.get_active();
        ret.contents                              = query_w.get_text();
        ret.contents_search_method                = PLAIN; // todo: todo
        ret.contents_search_cs                    = query_casesensitive_cb.get_active();
        ret.one_content_match_is_enough           = one_content_match_is_enough_cb.get_active();
        ret.dont_show_files_with_0_contents_match = dont_show_files_with_0_contents_match_cb.get_active();
        return ret;
    }

    int FindFile::setFindFileQuery(FindFileQuery q)
    {
        filemask_w.set_text(q.fnmatch_pattern);
        fnmatch_cs_cb.set_active(q.fnmatch_cs);
        use_fnmatch_on_path_part_cb.set_active(q.use_fnmatch_on_path_part);
        recurcive_cb.set_active(q.recurcive);
        delve_hidden_cb.set_active(q.delve_into_hidden_dirs);
        subpath_w.set_text(q.subpath.string());
        max_depth_cb.set_active(q.use_max_depth);
        max_depth_sb.set_value(q.max_depth);
        search_contents_cb.set_active(q.search_contents);
        query_w.set_text(q.contents);
        // ret.contents_search_method                = PLAIN; // todo: todo
        query_casesensitive_cb.set_active(q.contents_search_cs);
        one_content_match_is_enough_cb.set_active(q.one_content_match_is_enough);
        dont_show_files_with_0_contents_match_cb.set_active(q.dont_show_files_with_0_contents_match);
        return 0;
    }

    void FindFile::on_filelist_activate(gint)
    {
        // todo: add casting result checks

        std::cout << "FindFile::on_filelist_activate" << std::endl;

        auto files_sel_model = std::dynamic_pointer_cast<Gtk::SingleSelection>(
            result_files.get_model()
        );

        auto item = std::dynamic_pointer_cast<FindFileResultTreeItem>(files_sel_model->get_selected_item());

        /*
            auto files_model = std::dynamic_pointer_cast<Gio::ListStore<FindFileResultTreeItem>>(
                files_sel_model->get_model()
            );
        */

        // auto item = files_model->get_item(position);

        auto list_store = item->get_list_store();

        auto lines_sel = Gtk::SingleSelection::create(list_store);

        result_lines.set_model(lines_sel);
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

        work_time_query = getFindFileQuery();

        // todo: thread
        std::cout << "std::thread(search_thread);" << std::endl;
        std::thread thr1(&FindFile::search_thread, this);
        thr1.detach();
        return 0;
    }

    void FindFile::stop_search_thread()
    {
        search_stop_flag = true;
    }

    void FindFile::search_thread()
    {
        int err = 0;

        std::cout << "search_thread()" << std::endl;

        auto matched_files = Gio::ListStore<FindFileResultTreeItem>::create();
        auto sel           = Gtk::SingleSelection::create(matched_files);

        {

            std::promise<void> m1;
            auto               m1_fut = m1.get_future();

            Glib::MainContext::get_default()->signal_idle().connect_once(
                [this, &m1, &sel]()
                {
                    auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                        [&m1]()
                        {
                            m1.set_value();
                        }
                    );

                    result_files.set_model(sel);
                }
            );

            m1_fut.wait();
        }

        std::filesystem::path proj_path;

        std::tie(proj_path, err) = getProjectPath();
        if (err != 0)
        {
            // todo: report
            return;
        }

        std::queue<std::filesystem::path> dir_subpaths_to_search_q;
        std::queue<std::filesystem::path> files_to_grep_q;

        dir_subpaths_to_search_q.push(work_time_query.subpath);

        while (!dir_subpaths_to_search_q.empty())
        {

            std::cout << "new iter" << std::endl;

            auto work_path = dir_subpaths_to_search_q.front();
            std::cout << "work_path 1: " << work_path << std::endl;
            dir_subpaths_to_search_q.pop();

            typeof(work_path) work_path_full;

            work_path = work_path.relative_path().lexically_normal();
            std::cout << "work_path 2: " << work_path << std::endl;

            work_path_full = proj_path / work_path;
            work_path_full = work_path_full.lexically_normal();
            std::cout << "work_path_full: " << work_path_full << std::endl;

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
            std::cout << "enumerating children at: " << work_path_full << std::endl;

            auto sg02 = std::experimental::fundamentals_v3::scope_exit(
                [&enumerator]()
                {
                    enumerator->close();
                }
            );

            std::vector<std::filesystem::path> matched_filenames;

            for (;;)
            {
                std::cout << "dir files new iter" << std::endl;

                auto fi = enumerator->next_file();
                if (fi == nullptr)
                {
                    break;
                }

                auto name = fi->get_name();
                auto type = fi->get_file_type();

                std::cout << "   name: " << name << std::endl;

                auto work_path_full_name = work_path_full / name;
                std::cout << "   work_path_full_name: " << work_path_full_name << std::endl;

                auto work_path_full_name_rel = std::filesystem::relative(
                    work_path_full_name,
                    proj_path
                );
                std::cout << "   work_path_full_name_rel: " << work_path_full_name_rel << std::endl;

                if (type == Gio::FileType::DIRECTORY)
                {
                    if (work_time_query.recurcive)
                    {
                        if (work_time_query.use_max_depth)
                        {
                            if (std::distance(
                                    work_path_full_name_rel.begin(),
                                    work_path_full_name_rel.end()
                                )
                                > work_time_query.max_depth)
                            {
                                continue;
                            }
                        }
                        if (!work_time_query.delve_into_hidden_dirs
                            && name.starts_with("."))
                        {
                            continue;
                        }
                        dir_subpaths_to_search_q.push(work_path_full_name_rel);
                    }
                }
                else
                {
                    if (
                        fnmatch(
                            work_time_query.fnmatch_pattern.c_str(),
                            (work_time_query.use_fnmatch_on_path_part ? work_path_full_name_rel.c_str() : name.c_str()),
                            0
                        )
                        == 0
                    )
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
                    if (work_time_query.dont_show_files_with_0_contents_match
                        && (x->get_list_store()->get_n_items() == 0))
                    {
                        continue;
                    }
                }

                std::promise<void> m1;
                auto               m1_fut = m1.get_future();

                Glib::MainContext::get_default()->signal_idle().connect_once(
                    [this, &m1, &matched_files, &x]()
                    {
                        auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                            [&m1]()
                            {
                                m1.set_value();
                            }
                        );

                        matched_files->insert_sorted(
                            x,
                            [](
                                const Glib::RefPtr<const FindFileResultTreeItem> &a,
                                const Glib::RefPtr<const FindFileResultTreeItem> &b
                            )
                            {
                                auto aa    = a->subpath.relative_path().lexically_normal().string();
                                bool aa_sl = aa.contains("/");

                                auto bb    = b->subpath.relative_path().lexically_normal().string();
                                bool bb_sl = bb.contains("/");

                                if (aa == bb)
                                {
                                    return 0;
                                }

                                if (aa_sl == bb_sl)
                                {
                                    if (aa < bb)
                                    {
                                        return -1;
                                    }
                                    if (aa > bb)
                                    {
                                        return 1;
                                    }
                                }
                                else
                                {
                                    if (aa_sl == true && bb_sl == false)
                                    {
                                        return 1;
                                    }
                                    if (aa_sl == false && bb_sl == true)
                                    {
                                        return -1;
                                    }
                                }

                                return 0;
                            }
                        );
                    }
                );

                m1_fut.wait();
            }
        }
    }

    int FindFile::search_thread_search_contents(
        FindFileResultTreeItemP item
    )
    {
        int         err = 0;
        std::string cont_txt;

        std::filesystem::path proj_path;

        std::tie(proj_path, err) = getProjectPath();
        if (err != 0)
        {
            // todo: report
            return 1;
        }

        auto item_subpath   = item->subpath;
        auto full_file_name = proj_path / item_subpath;

        std::cout << "grepping file: " << full_file_name << std::endl;

        std::tie(cont_txt, err) = loadStringFromFile(full_file_name);
        if (err != 0)
        {
            // todo: report
            return 1;
        }

        auto cont_txt_ls = LineStarts(cont_txt);
        // cont_txt_ls.printParsingResult(cont_txt);

        switch (work_time_query.contents_search_method)
        {
            default:
                return 1;
                break;
            case PLAIN:
            {
                std::string subj(work_time_query.contents);

                if (subj.length() == 0)
                {
                    return 2;
                }

                auto i = cont_txt.begin();

                for (;;)
                {
                    auto ri = std::search(
                        i, cont_txt.end(), subj.begin(), subj.end()
                    );

                    if (ri == cont_txt.end())
                    {
                        break;
                    }
                    std::cout << "search ok" << std::endl;

                    std::promise<void> m1;
                    auto               m1_fut = m1.get_future();

                    Glib::MainContext::get_default()->signal_idle().connect_once(
                        [this, &m1, &item, &cont_txt_ls, &cont_txt, &ri]()
                        {
                            auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                                [&m1]()
                                {
                                    m1.set_value();
                                }
                            );

                            auto dist = std::distance(cont_txt.begin(), ri);

                            auto line_no = cont_txt_ls.getLineByOffset(dist);

                            auto line_info = cont_txt_ls.getLineInfo(line_no);

                            auto r0  = std::get<0>(line_info);
                            auto r1  = std::get<1>(line_info);
                            auto err = std::get<2>(line_info);
                            if (err != 0)
                            {
                                return;
                            }

                            auto substr = trim_right(cont_txt.substr(r0, r1 - r0));

                            /*
                                        std::cout
                                            << std::format(
                                                   "    (dist: {}, line: {}) r0: {}, r1: {}, r1-r0: {}, substr: {}",
                                                   dist,
                                                   line_no,
                                                   r0,
                                                   r1,
                                                   r1 - r0,
                                                   substr
                                               )
                                            << std::endl;
                        */

                            item->create_item(line_no, substr);
                        }
                    );

                    m1_fut.wait();
                    i = ri;
                    i = std::next(i);
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
        std::cout << "~FindFileResultTreeItem()" << std::endl;
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

    Glib::RefPtr<Gio::ListStore<FindFileResultTreeItemItem>>
        FindFileResultTreeItem::get_list_store()
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
        // todo: something better is needed whan std::format
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
