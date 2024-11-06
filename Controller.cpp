

#include <fstream>
#include <iostream>
#include <type_traits>

#include <boost/json.hpp>

#include "ProjectMgr.hpp"

#include "Controller.hpp"


namespace wayround_i2p::ccedit
{

Controller::Controller(Glib::RefPtr<Gtk::Application> app)
{
    this->app          = app;
    project_list_store = Gio::ListStore<ProjectTableRow>::create();
}

Controller::~Controller()
{
    std::cout << "~Controller()" << std::endl;
};

int Controller::run(int argc, char *argv[])
{

    ensureConfigDirExists();
    // if (ensureConfigDirExists()!= 0) {
    //     // std::cout << format("couldn't ensure directory existance: {}")
    // };

    loadConfig();

    addBuiltinMods();

    icon_theme = Gtk::IconTheme::create();

    app->signal_startup()
        .connect(
            sigc::mem_fun(*this, &Controller::on_app_startup)
        );

    return app->run(argc, argv);
}

void Controller::on_app_startup()
{
    showProjectMgr();
}

void Controller::showProjectMgr()
{
    if (!project_mgr)
    {
        project_mgr = ProjectMgr_shared(
            new ProjectMgr(own_ptr)
        );
        registerWindow(project_mgr);
    }

    project_mgr->show();
}

void Controller::cleanupProjectMgr()
{
    project_mgr.reset();
}

Glib::RefPtr<Gtk::Application>
    Controller::getGtkApp()
{
    return app;
}

void Controller::registerWindow(Gtk::Window *win)
{
    app->add_window(*win);
}


int Controller::findProjectIndex(std::string proj_name)
{
    for (
        int i = 0;
        i != project_list_store->get_n_items();
        i++
    )
    {
        if (project_list_store->get_item(i)->proj_name() == proj_name)
        {
            return i;
        }
    }
    return -1;
}

int Controller::findProjectIndex(ProjectCtl *p_ctl)
{
    for (
        int i = 0;
        i != project_list_store->get_n_items();
        i++
    )
    {
        if (project_list_store->get_item(i)->proj_ctl.get() == p_ctl)
        {
            return i;
        }
    }
    return -1;
}

int Controller::findProjectIndex(ProjectCtl_shared p_ctl)
{
    return findProjectIndex(p_ctl.get());
}

int Controller::saveConfig()
{
    // todo: optimizations? use pointers and scope_exits?
    auto [pth, err] = getConfigFullPath();
    if (err != 0)
    {
        return err;
    }

    boost::json::object root;

    boost::json::array projects = boost::json::array();

    for (
        int i = 0;
        i != project_list_store->get_n_items();
        i++
    )
    {
        projects.push_back(
            boost::json::object(
                {
                    std::pair(
                        "name",
                        project_list_store->get_item(i)->proj_name()
                    ),
                    std::pair(
                        "path",
                        project_list_store->get_item(i)->proj_path().string()
                    ),
                }
            )
        );
    }

    root["projects"] = projects;

    // todo: error checks for file operations
    auto f = std::ofstream(pth);

    f << root;

    f.close();

    return 0;
}

int Controller::loadConfig()
{
    // todo: optimizations? use pointers and scope_exits?
    auto [pth, err] = getConfigFullPath();
    if (err != 0)
    {
        return err;
    }

    // todo: error checks
    auto f = std::ifstream(pth);

    std::string root_str("");
    f >> root_str;

    f.close();

    auto root = boost::json::parse(root_str).as_object();

    auto projects = root["projects"].as_array();

    // todo: ensure project_list_store is empty

    for (std::size_t i = 0; i != projects.size(); i++)
    {
        auto x = projects[i].as_object();
        createProject(
            std::string(x["name"].as_string().c_str()),
            std::filesystem::path(std::string(x["path"].as_string().c_str())),
            false
        );
    }

    return 0;
}

int Controller::createProject(
    std::string           proj_name,
    std::filesystem::path proj_path,
    bool                  save_to_config
)
{
    auto p_ind = findProjectIndex(proj_name);
    if (p_ind != -1)
    {
        return 1;
    }

    auto x = ProjectTableRow::create();

    // x->controller = own_ptr;
    x->proj_name(proj_name);
    x->proj_path(proj_path);

    project_list_store->append(x);

    if (save_to_config)
    {
        saveConfig();
    }

    return 0;
}

int Controller::editProject(
    std::string           name,
    std::string           new_name,
    std::filesystem::path new_path
)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind == -1)
    {
        return 0;
    }

    if (new_name != "" && new_name != name)
    {
        auto index_2 = findProjectIndex(new_name);
        if (index_2 != -1)
        {
            return 3;
        }
    }

    auto x = project_list_store->get_item(p_ind);
    if (x == NULL)
    {
        return 1;
    }

    if (new_name != "" && new_name != name)
    {
        x->proj_name(new_name);
    }
    x->proj_path(new_path);

    saveConfig();

    return 0;
}

std::tuple<    std::string,    int>
    Controller::getNameProject(ProjectCtl_shared p_ctl)
{
    return getNameProject(p_ctl.get());
}

std::tuple<
    std::string,
    int>
    Controller::getNameProject(ProjectCtl *p_ctl)
{
    auto p_ind = findProjectIndex(p_ctl);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);

        return std::tuple(x->proj_name(), 0);
    }

    return std::tuple("", 1);
}

std::tuple<std::filesystem::path, int> Controller::getPathProject(std::string name)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        return std::tuple("", 1);
    }

    auto x = project_list_store->get_item(p_ind);
    if (x == NULL)
    {
        return std::tuple("", 2);
    }

    return std::tuple(x->proj_path(), 0);
}

std::tuple<
    std::filesystem::path,
    int>
    Controller::getPathProject(ProjectCtl_shared p_ctl)
{
    return getPathProject(p_ctl.get());
}

std::tuple<
    std::filesystem::path,
    int>
    Controller::getPathProject(ProjectCtl *p_ctl)
{

    std::filesystem::path root("/");

    if (global_proj_ctl)
    {
        if (p_ctl == global_proj_ctl.get())
        {
            return std::tuple(root, 0);
        }
    }

    auto p_ind = findProjectIndex(p_ctl);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);

        return std::tuple(x->proj_path(), 0);
    }

    return std::tuple(root, 1);
}

Glib::RefPtr<Gio::ListStore<ProjectTableRow>> Controller::getProjectListStore()
{
    return project_list_store;
}

std::vector<CodeEditorMod *> Controller::getBuiltinMods()
{
    return builtin_mods;
}

int Controller::addBuiltinMod(CodeEditorMod *mod)
{
    builtin_mods.push_back(mod);
    return 0;
}

int Controller::addBuiltinMods()
{
    addBuiltinMod(get_mod_info_ccpp());
    addBuiltinMod(get_mod_info_go());
    return 0;
}

bool Controller::isGlobalProjCtl(ProjectCtl_shared p_ctl)
{
    return isGlobalProjCtl(p_ctl.get());
}

bool Controller::isGlobalProjCtl(ProjectCtl *p_ctl)
{
    return global_proj_ctl && p_ctl == global_proj_ctl.get();
}

ProjectCtl_shared Controller::createGlobalProjCtl()
{
    if (!global_proj_ctl)
    {
        global_proj_ctl = ProjectCtl_shared(
            new ProjectCtl(own_ptr)
        );
        global_proj_ctl->own_ptr = global_proj_ctl;
        global_proj_ctl->projectControllerRegisteredInController();
    }

    return global_proj_ctl;
}

ProjectCtl_shared Controller::getGlobalProjCtl()
{
    return global_proj_ctl;
}

void Controller::closeGlobalProjCtl()
{
    if (global_proj_ctl)
    {
        // closeGlobalProjCtlWin()();
        global_proj_ctl->close();
        global_proj_ctl->own_ptr.reset();
        global_proj_ctl.reset();
    }
}

void Controller::showGlobalProjCtlWin()
{
    createGlobalProjCtl();
    if (global_proj_ctl)
    {
        global_proj_ctl->showWindow();
    }
}

void Controller::closeGlobalProjCtlWin()
{
    if (global_proj_ctl)
    {
        global_proj_ctl->closeWindow();
    }
}

std::tuple<ProjectCtl_shared, int> Controller::createProjCtl(std::string name)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);
        if (!(x->proj_ctl))
        {
            auto new_ctl = ProjectCtl_shared(
                new ProjectCtl(own_ptr)
            );
            x->proj_ctl      = new_ctl;
            new_ctl->own_ptr = new_ctl;
            x->proj_ctl->projectControllerRegisteredInController();
        }
        return std::tuple(x->proj_ctl, 0);
    }
    return std::tuple(nullptr, 1);
}

std::tuple<ProjectCtl_shared, int> Controller::getProjCtl(std::string name)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);
        if (x->proj_name() == name)
        {
            return std::tuple(x->proj_ctl, 0);
        }
    }
    return std::tuple(nullptr, 1);
}

int Controller::closeProjCtl(std::string name)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);

        closeProjCtl(x->proj_ctl);
        return 0;
    }
    return -1;
}

int Controller::showProjCtlWin(std::string name)
{
    createProjCtl(name);

    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);
        x->proj_ctl->showWindow();
        return 0;
    }
    return -1;
}

void Controller::closeProjCtlWin(std::string name)
{
    auto p_ind = findProjectIndex(name);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);
        x->proj_ctl->closeWindow();
    }
}

void Controller::closeProjCtl(ProjectCtl_shared p_ctl)
{
    return closeProjCtl(p_ctl.get());
}

void Controller::closeProjCtl(ProjectCtl *p_ctl)
{
    if (isGlobalProjCtl(p_ctl))
    {
        closeGlobalProjCtl();
        return;
    }

    auto p_ind = findProjectIndex(p_ctl);
    if (p_ind != -1)
    {
        auto x = project_list_store->get_item(p_ind);

        auto z = x->proj_ctl;
        x->proj_ctl.reset();
        z->own_ptr.reset();
    }
}

/*
void Controller::emitProjectListStoreItemsChanged()
{
    project_list_store->signal_items_changed().emit(0, 0, 0);
}
*/

const std::string CODEEDIT_CFG = "ccedit.cfg";

// ----------------------------------

#ifdef __unix__

    #include <wordexp.h>

std::tuple<std::filesystem::path, int> Controller::getConfigDir()
{
    wordexp_t wordexp_res;

    auto ret = wordexp("~", &wordexp_res, 0);

    if (wordexp_res.we_wordc != 1)
    {
        return std::tuple("", 1);
    }

    auto dir = std::filesystem::path(std::string(wordexp_res.we_wordv[0]));

    dir = dir / ".config" / "ccedit";

    return std::tuple(dir, 0);
}

std::tuple<std::filesystem::path, int> Controller::getConfigFullPath()
{
    auto [dir, err] = getConfigDir();
    if (err != 0)
    {
        return std::tuple("", err);
    }
    // todo: use const for file name
    dir = dir / CODEEDIT_CFG;
    return std::tuple(dir, 0);
}

int Controller::ensureConfigDirExists()
{
    auto [dir, err] = getConfigDir();
    if (err != 0)
    {
        return err;
    }

    std::filesystem::create_directories(dir);
    if (!std::filesystem::exists(dir))
    {
        return 1;
    }
    return 0;
}

#else
    #error "only __unix__ target supported now"
#endif

} // namespace wayround_i2p::ccedit
