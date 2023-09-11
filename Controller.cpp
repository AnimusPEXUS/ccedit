

#include <fstream>
#include <iostream>
#include <type_traits>

#include <boost/json.hpp>

#include "ProjectMgr.hpp"

#include "Controller.hpp"

#include "builtin_mods/ccpp/mod_ccpp.hpp"

using namespace wayround_i2p::codeeditor;

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

    addBuiltinMods();

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
        project_mgr = std::shared_ptr<ProjectMgr>(
            new ProjectMgr(own_ptr)
        );
        app->add_window(*project_mgr);
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

int Controller::findProjectIndex(std::string proj_name)
{
    // todo: know how to use std::remove_const
    for (
        int i = 0;
        i != project_list_store->get_n_items();
        i++
    )
    {
        if (project_list_store->get_item(i)->proj_name == proj_name)
        {
            return i;
        }
    }
    return -1;
}

int Controller::saveConfig()
{

    auto [pth, err] = getConfigFullPath();
    if (err != 0)
    {
        return err;
    }

    boost::json::object root;
    root["test"] = 123;

    auto f = std::ofstream(pth);

    f << root;

    f.close();

    return 1;
}

int Controller::loadConfig()
{
    return 1;
}

int Controller::createProject(
    std::string proj_name,
    std::string proj_path,
    bool        save_to_config
)
{
    auto index = findProjectIndex(proj_name);
    if (index != -1)
    {
        return 0;
    }

    auto x       = ProjectTableRow::create();
    x->proj_name = proj_name;
    x->proj_path = proj_path;

    project_list_store->append(x);

    if (save_to_config)
    {
        saveConfig();
    }

    return 0;
}

int Controller::editProject(
    std::string name,
    std::string new_name,
    std::string new_path
)
{
    auto index = findProjectIndex(name);
    if (index == -1)
    {
        return 0;
    }

    if (name != new_name)
    {
        auto index_2 = findProjectIndex(new_name);
        if (index_2 != -1)
        {
            return 3;
        }
    }

    auto x = project_list_store->get_item(index);
    if (x == NULL)
    {
        return 1;
    }

    x->proj_name = new_name;
    x->proj_path = new_path;

    saveConfig();

    return 0;
}

std::tuple<std::string, int> Controller::getPathProject(std::string name)
{
    auto index = findProjectIndex(name);
    if (index != -1)
    {
        return std::tuple("", 1);
    }

    auto x = project_list_store->get_item(index);
    if (x == NULL)
    {
        return std::tuple("", 2);
    }

    return std::tuple(x->proj_path, 0);
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
    return 0;
}

void Controller::showGlobalProjCtl()
{
    if (!global_proj_ctl)
    {
        global_proj_ctl = std::shared_ptr<ProjectCtl>(
            new ProjectCtl(own_ptr)
        );
        global_proj_ctl->own_ptr = global_proj_ctl;
        app->add_window(*global_proj_ctl);
    }

    global_proj_ctl->show();
}

void Controller::cleanupGlobalProjCtl()
{
    if (global_proj_ctl)
    {
        global_proj_ctl->own_ptr.reset();
        global_proj_ctl.reset();
    }
}

const std::string CODEEDIT_CFG = "codeedit.cfg";

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

    dir = dir / ".config" / "codeeditor";

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
