#ifndef WAYROUND_I2P_20241106_003315_537102
#define WAYROUND_I2P_20241106_003315_537102

#include <filesystem>
#include <memory>
#include <vector>

#include <gtkmm.h>

#include "forward_declarations.hpp"

#include "modularity.hpp"

#include "tables.hpp"

namespace wayround_i2p::ccedit
{

class Controller
{
  public:
    Controller_shared create(Glib::RefPtr<Gtk::Application> app);

  protected:
    Controller(Glib::RefPtr<Gtk::Application> app);

  private:
    Controller_shared own_ptr;

  public:
    ~Controller();

    int run(int argc, char *argv[]);

    void showProjectMgr();
    void closeProjectMgr();

    // todo: use error type

    // function fails if
    //   project with such name already exists
    int createProject(
        std::string           name,
        std::filesystem::path path,
        bool                  save_to_config
    );

    // project name will not be changed if
    //   (new_name == "" || new_name == name)
    // can't be edited project which name not exists
    // if trying to change name - function fails if
    //   project with such name already exists
    int editProject(
        std::string           name,
        std::string           new_name,
        std::filesystem::path new_path
    );

    std::tuple<std::string, int>
        getNameProject(ProjectCtl_shared p_ctl);

    // std::tuple<std::string, int>
    //   getNameProject(ProjectCtl *p_ctl);

    std::tuple<std::filesystem::path, int>
        getPathProject(std::string name);

    std::tuple<std::filesystem::path, int>
        getPathProject(ProjectCtl_shared p_ctl);

    // std::tuple<std::filesystem::path, int>
    //   getPathProject(ProjectCtl *p_ctl);

    Glib::RefPtr<Gtk::Application> getGtkApp();

    void registerWindow(Gtk::Window *win);
    void unregisterWindow(Gtk::Window *win);

    int saveConfig();
    int loadConfig();

    Glib::RefPtr<Gio::ListStore<ProjectTableRow>>
        getProjectListStore();

    std::vector<CodeEditorMod *> getBuiltinMods();

    bool isGlobalProjCtl(ProjectCtl_shared p_ctl);
    // bool isGlobalProjCtl(ProjectCtl *p_ctl);

    ProjectCtl_shared createGlobalProjCtl();

    // resulting pointer is empty if function failed
    ProjectCtl_shared getGlobalProjCtl();

    // this leads to closing global proj children windows
    void closeGlobalProjCtl();

    // this will call createGlobalProjCtl() if not created yet
    void showGlobalProjCtlWin();
    void closeGlobalProjCtlWin();

    std::tuple<ProjectCtl_shared, int> createProjCtl(std::string name);

    // resulting pointer is empty if function failed
    std::tuple<ProjectCtl_shared, int> getProjCtl(std::string name);

    int closeProjCtl(std::string name);

    // error if not found
    int  showProjCtlWin(std::string name);
    void closeProjCtlWin(std::string name);

    // this also calls closeGlobalProjCtl if global ProjectCtl passed
    void closeProjCtl(ProjectCtl_shared p_ctl);
    // void closeProjCtl(ProjectCtl *);

    Glib::RefPtr<Gtk::IconTheme> icon_theme;

    // void emitProjectListStoreItemsChanged();

  private:
    Glib::RefPtr<Gtk::Application> app;

    ProjectMgr_weak project_mgr;
    ProjectCtl_weak global_proj_ctl;

    // todo: use mutex on all project_list methods?
    Glib::RefPtr<Gio::ListStore<ProjectTableRow>> project_list_store;

    void on_app_startup();

    int findProjectIndex(std::string name);
    int findProjectIndex(ProjectCtl_shared p_ctl);
    // int findProjectIndex(ProjectCtl *p_ctl);

    std::tuple<std::filesystem::path, int> getConfigDir();
    std::tuple<std::filesystem::path, int> getConfigFullPath();
    int                                    ensureConfigDirExists();

    std::vector<CodeEditorMod *> builtin_mods;

    int addBuiltinMods();
    int addBuiltinMod(CodeEditorMod *mod);
};
} // namespace wayround_i2p::ccedit

#endif
