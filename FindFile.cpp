
#include <algorithm>
#include <experimental/scope>
#include <format>
#include <future>
#include <queue>
#include <thread>

#include "CodeEditorAbstract.hpp"
#include "Controller.hpp"
#include "FindFile.hpp"
#include "ProjectCtl.hpp"

#include "utils.hpp"

#include "posix_interface.hpp"

namespace wayround_i2p::ccedit
{

FindFile_shared FindFile::create(ProjectCtl_shared p_ctl)
{
    auto ret     = FindFile_shared(new FindFile(p_ctl));
    ret->own_ptr = ret;
    p_ctl->registerFindFile(ret);
    return ret;
}

FindFile::FindFile(ProjectCtl_shared p_ctl) :
    destroyer(
        [this]()
        {
            if (auto l = this->p_ctl.lock(); l)
            {
                l->unregisterFindFile(own_ptr);
            }
            win.destroy();
            own_ptr.reset();
        }
    )
{
    this->p_ctl = p_ctl;

    // maximize();

    win.set_child(main_box);

    main_box.set_orientation(Gtk::Orientation::VERTICAL);
    main_box.set_spacing(5);
    main_box.set_margin(5);

    main_box.append(editors_frame);
    main_box.append(files_settings_frame);
    main_box.append(search_contents_frame);
    main_box.append(button_frame);
    main_box.append(results_pan);

    editors_frame.set_child(editors_grid);
    editors_grid.set_row_spacing(5);
    editors_grid.set_column_spacing(5);
    editors_grid.set_margin(5);
    editors_grid.set_row_homogeneous(true);

    button_box.set_margin(5);

    filemask_l.set_text("File Mask");
    subpath_l.set_text("Project's Subpath");

    filemask_l.set_halign(Gtk::Align::START);
    subpath_l.set_halign(Gtk::Align::START);

    filemask_w.set_hexpand(true);
    subpath_w.set_hexpand(true);

    editors_grid.attach(filemask_l, 0, 0);
    editors_grid.attach(filemask_w, 1, 0);

    editors_grid.attach(subpath_l, 0, 1);
    editors_grid.attach(subpath_w, 1, 1);

    use_fnmatch_on_path_part_cb.set_label("File Path Checked By Mask Too");
    use_fnmatch_on_path_part_cb.set_tooltip_text(
        "File Mask Checks not only filename, but entire path, "
        "starting from project root"
    );
    recurcive_cb.set_label("Recurcive");
    delve_hidden_cb.set_label("Delve into Hidden Dirs");
    delve_hidden_cb.set_tooltip_text("dirs, which names starting with dot");
    max_depth_cb.set_label("Max Depth");
    fnmatch_cs_cb.set_label("File Mask Case Sensitive");
    search_contents_cb.set_label("Use Content Matching");
    dont_show_files_with_0_contents_match_cb.set_label("Don't Display Files with 0 content matches");

    max_depth_sb.set_range(0, 1024);
    max_depth_sb.set_increments(1, 1);

    max_depth_box.set_spacing(5);
    max_depth_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    max_depth_box.append(max_depth_cb);
    max_depth_box.append(max_depth_sb);

    files_settings_frame.set_child(files_settings_box);
    files_settings_frame.set_label("File Search Settings");
    files_settings_box.set_selection_mode(Gtk::SelectionMode::NONE);
    files_settings_box.append(use_fnmatch_on_path_part_cb);
    files_settings_box.append(recurcive_cb);
    files_settings_box.append(delve_hidden_cb);
    files_settings_box.append(max_depth_box);
    files_settings_box.append(fnmatch_cs_cb);

    find_text_widget.set_margin(5);

    find_text_widget_frame.set_margin_start(5);
    find_text_widget_frame.set_margin_end(5);

    // if (find_text_widget have scrolled window) {
    // find_text_widget_sw.set_child(find_text_widget);
    // find_text_widget_frame.set_child(find_text_widget_sw);
    // } else {
    find_text_widget_frame.set_child(find_text_widget);
    // }

    search_contents_box.set_orientation(Gtk::Orientation::VERTICAL);
    search_contents_box.set_spacing(5);

    search_contents_frame.set_child(search_contents_box);
    search_contents_frame.set_label("Contents Search Settings");
    search_contents_box.append(search_contents_cb);
    search_contents_box.append(find_text_widget_frame);
    search_contents_box.append(dont_show_files_with_0_contents_match_cb);

    button_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    button_box.set_spacing(5);

    start_btn.set_label("Start");
    stop_btn.set_label("Stop");

    progress_bar.set_hexpand(true);
    progress_bar.set_show_text(true);

    button_frame.set_child(button_box);
    button_box.append(start_btn);
    button_box.append(progress_bar);
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
        [this](gint i)
        { on_filelist_activate(i); }
    );

    start_btn.signal_clicked().connect(
        [this]()
        { on_start_btn(); }
    );

    stop_btn.signal_clicked().connect(
        [this]()
        { on_stop_btn(); }
    );

    win.signal_destroy().connect(
        [this]()
        { on_destroy_sig(); }
    );

    win.signal_close_request().connect(
        [this]() -> bool
        { return on_signal_close_request(); },
        true
    );

    if (auto x = this->p_ctl.lock(); x)
    {
        x->getController()->registerWindow(&win);
    }
}

void FindFile::setup_result_filelist()
{
    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        [this](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            auto w = Gtk::make_managed<FindFileResultTreeItemWidget>(
                list_item,
                [this](FindFileResultTreeItemP item)
                {
                    auto x = p_ctl.lock();
                    if (!x)
                    {
                        return;
                    }

                    auto ed = x->workSubjectExistingOrNewEditor(
                        item->subpath
                    );
                    if (!ed)
                    {
                        return;
                    }

                    ed->show();
                }
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
        [this](const Glib::RefPtr<Gtk::ListItem> &list_item)
        {
            auto w = Gtk::make_managed<FindFileResultTreeItemItemWidget>(
                list_item,
                [this](FindFileResultTreeItemItemP item)
                {
                    auto x = p_ctl.lock();
                    if (!x)
                    {
                        return;
                    }

                    auto ed = x->workSubjectExistingOrNewEditor(
                        item->subpath
                    );
                    if (!ed)
                    {
                        return;
                    }

                    ed->show();

                    ed->setCurrentLine(
                        item->line,
                        true
                    );

                    ed->selectSlice(
                        item->start_offset,
                        item->end_offset
                    );
                }
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

void FindFile::destroy()
{
    destroyer.run();
}

void FindFile::on_destroy_sig()
{
    std::cout << "FindFile::on_destroy_sig()" << std::endl;
    destroyer.run();
}

bool FindFile::on_signal_close_request()
{
    std::cout << "FindFile::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

void FindFile::start()
{
    // todo: todo
}

void FindFile::stop()
{
    // todo: todo
}

void FindFile::show()
{
    win.present();
}

FindFileQuery FindFile::getFindFileQuery()
{
    FindFileQuery ret;
    ret.fnmatch_pattern = icu::UnicodeString::fromUTF8(
        icu::StringPiece(
            filemask_w.get_text()
        )
    );
    ret.fnmatch_cs                            = fnmatch_cs_cb.get_active();
    ret.use_fnmatch_on_path_part              = use_fnmatch_on_path_part_cb.get_active();
    ret.recurcive                             = recurcive_cb.get_active();
    ret.delve_into_hidden_dirs                = delve_hidden_cb.get_active();
    ret.subpath                               = subpath_w.get_text();
    ret.use_max_depth                         = max_depth_cb.get_active();
    ret.max_depth                             = max_depth_sb.get_value_as_int();
    ret.contents_search_enabled               = search_contents_cb.get_active();
    ret.dont_show_files_with_0_contents_match = dont_show_files_with_0_contents_match_cb.get_active();

    ret.find_text_query = find_text_widget.getFindTextQuery();

    return ret;
}

int FindFile::setFindFileQuery(FindFileQuery q)
{
    {
        auto x = std::string("");
        filemask_w.set_text(q.fnmatch_pattern.toUTF8String(x));
    }
    fnmatch_cs_cb.set_active(q.fnmatch_cs);
    use_fnmatch_on_path_part_cb.set_active(q.use_fnmatch_on_path_part);
    recurcive_cb.set_active(q.recurcive);
    delve_hidden_cb.set_active(q.delve_into_hidden_dirs);
    subpath_w.set_text(q.subpath.string());
    max_depth_cb.set_active(q.use_max_depth);
    max_depth_sb.set_value(q.max_depth);
    search_contents_cb.set_active(q.contents_search_enabled);
    dont_show_files_with_0_contents_match_cb.set_active(
        q.dont_show_files_with_0_contents_match
    );

    find_text_widget.setFindTextQuery(q.find_text_query);

    return 0;
}

void FindFile::updateProgressbar(
    unsigned int dirs_count,
    unsigned int dirs_done,
    unsigned int files_count,
    unsigned int files_done
)
{
    double fract = 0;

    fract = 1. / (((double)dirs_count + (double)files_count) / ((double)dirs_done + (double)files_done));

    std::string status_text("");

    if (search_working)
    {
        status_text = "searching";
    }
    else
    {
        status_text = "stopped";
        if (search_stop_flag)
        {
            status_text += " - by cancel command";
        }
        else
        {
            status_text += " - finished";
        }
    }

    std::string new_text = std::format(
        "dirs: {} / {}, files: {} / {}, {:.03}% - {}",
        dirs_done,
        dirs_count,
        files_done,
        files_count,
        100.0 * fract,
        status_text
    );

    std::promise<void> m1;
    auto               m1_fut = m1.get_future();

    Glib::MainContext::get_default()->signal_idle().connect_once(
        [this, &m1, &fract, &new_text]()
        {
            auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                [&m1]()
                {
                    m1.set_value();
                }
            );

            progress_bar.set_fraction(fract);
            progress_bar.set_text(new_text);
        }
    );

    m1_fut.wait();
}

void FindFile::on_filelist_activate(gint)
{
    // todo: add casting result checks

    std::cout << "FindFile::on_filelist_activate" << std::endl;

    auto files_sel_model = std::dynamic_pointer_cast<Gtk::SingleSelection>(
        result_files.get_model()
    );

    auto item = std::dynamic_pointer_cast<FindFileResultTreeItem>(
        files_sel_model->get_selected_item()
    );

    // auto item = files_model->get_item(position);

    auto list_store = item->get_list_store();

    auto lines_sel = Gtk::SingleSelection::create(list_store);

    result_lines.set_model(lines_sel);
}

void FindFile::on_start_btn()
{
    std::cout << "on_start_btn()" << std::endl;
    start_search_thread();
}

void FindFile::on_stop_btn()
{
    std::cout << "on_stop_btn()" << std::endl;
    stop_search_thread();
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

    auto p_ctl_locked = p_ctl.lock();

    if (!p_ctl_locked)
    {
        return;
    }

    int err = 0;
    if (search_working)
    {
        return;
    }
    search_working = true;

    unsigned int dirs_count  = 0;
    unsigned int dirs_done   = 0;
    unsigned int files_count = 0;
    unsigned int files_done  = 0;

    auto sg04 = std::experimental::fundamentals_v3::scope_exit(
        [this,
         &dirs_count,
         &dirs_done,
         &files_count,
         &files_done]
        {
            search_working = false;
            updateProgressbar(
                dirs_count,
                dirs_done,
                files_count,
                files_done
            );
        }
    );

    std::cout << "FindFile::search_thread()" << std::endl;

    auto matched_files = Gio::ListStore<FindFileResultTreeItem>::create();
    auto sel           = Gtk::SingleSelection::create(matched_files);

    auto m1     = std::promise<void>();
    auto m1_fut = m1.get_future();

    // todo: find better soultion to this than using signal_idle()
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

    auto proj_path = p_ctl_locked->getProjectPath();

    std::queue<std::filesystem::path> dir_subpaths_to_search_q;
    // std::queue<std::filesystem::path> files_to_grep_q;

    dir_subpaths_to_search_q.push(work_time_query.subpath);
    dirs_count++;
    updateProgressbar(
        dirs_count,
        dirs_done,
        files_count,
        files_done
    );

    while (!dir_subpaths_to_search_q.empty())
    {
        std::cout << "new iter" << std::endl;

        if (search_stop_flag)
        {
            return;
        }

        auto work_path = dir_subpaths_to_search_q.front();
        std::cout << "work_path 1: " << work_path << std::endl;
        dir_subpaths_to_search_q.pop();

        auto sg01 = std::experimental::fundamentals_v3::scope_exit(
            [this,
             &dirs_count,
             &dirs_done,
             &files_count,
             &files_done]()
            {
                dirs_done++;
                updateProgressbar(
                    dirs_count,
                    dirs_done,
                    files_count,
                    files_done
                );
            }
        );

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

            if (search_stop_flag)
            {
                return;
            }

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
                    dirs_count++;

                    updateProgressbar(
                        dirs_count,
                        dirs_done,
                        files_count,
                        files_done
                    );
                }
            }
            else
            {
                std::string x;

                if (auto [ok, err] = fnmatch_simple(
                        work_time_query.fnmatch_pattern.toUTF8String(x),
                        (
                            work_time_query.use_fnmatch_on_path_part ?
                                work_path_full_name_rel.string() :
                                name
                        )
                    );
                    ok && err == 0)
                {
                    matched_filenames.push_back(work_path_full_name_rel);
                    files_count++;

                    updateProgressbar(
                        dirs_count,
                        dirs_done,
                        files_count,
                        files_done
                    );
                }
            }
        }

        for (auto i = matched_filenames.begin();
             i != matched_filenames.end();
             i++)
        {

            if (search_stop_flag)
            {
                return;
            }

            auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                [this,
                 &dirs_count,
                 &dirs_done,
                 &files_count,
                 &files_done]()
                {
                    files_done++;
                    updateProgressbar(
                        dirs_count,
                        dirs_done,
                        files_count,
                        files_done
                    );
                }
            );

            auto x = FindFileResultTreeItem::create(*i);
            if (work_time_query.contents_search_enabled)
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

            m1     = std::promise<void>();
            m1_fut = m1.get_future();

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
    int                err = 0;
    icu::UnicodeString cont_txt;

    auto p_ctl_locked = p_ctl.lock();
    if (!p_ctl_locked)
    {
        return 5;
    }

    auto proj_path = p_ctl_locked->getProjectPath();

    auto item_subpath   = item->subpath;
    auto full_file_name = proj_path / item_subpath;

    std::cout << "grepping file: " << full_file_name << std::endl;

    std::tie(cont_txt, err) = loadStringFromFileICU(full_file_name);
    if (err != 0)
    {
        // todo: report
        return 1;
    }

    auto cont_txt_ls = std::make_shared<LineStartsICU>(cont_txt);

    // todo: error checking?
    find_text_widget.search_in_text(
        cont_txt,
        cont_txt_ls,

        [this]() -> bool
        {
            return search_stop_flag;
        },
        [this, &item](
            unsigned int       line,
            icu::UnicodeString line_text,
            unsigned int       start_offset,
            unsigned int       end_offset
        ) -> int
        {
            auto m1     = std::promise<void>();
            auto m1_fut = m1.get_future();

            Glib::MainContext::get_default()->signal_idle().connect_once(
                [this,
                 &m1,
                 &item,
                 &line,
                 &line_text,
                 &start_offset,
                 &end_offset]()
                {
                    auto sg01 = std::experimental::fundamentals_v3::scope_exit(
                        [&m1]()
                        {
                            m1.set_value();
                        }
                    );

                    auto s1 = trim_right(line_text);

                    {
                        std::string x;
                        item->create_item(
                            item->subpath,
                            line,
                            s1.toUTF8String(x),
                            start_offset,
                            end_offset
                        );
                    }
                }
            );

            m1_fut.wait();

            return 0;
        }
    );

    return 0;
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
    // ret->own_ptr = ret;
    return ret;
}

// ---------------------

} // namespace wayround_i2p::ccedit
