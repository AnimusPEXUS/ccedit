#ifndef WAYROUND_I2P_20241223_224139_473489
#define WAYROUND_I2P_20241223_224139_473489

#include <filesystem>
#include <memory>
#include <vector>

#include <gtkmm.h>

#include "build_num.hpp"

#include "forward_declarations.hpp"

#include "modularity.hpp"
#include "utils.hpp"
#include <wayround_i2p/ccutils/signal/signal_sigc_compat.hpp>

#include "CodeEditorAbstract.hpp"
#include "ProjectMgrTables.hpp"

namespace wayround_i2p::ccedit
{

const std::string CODEEDIT_CFG = "ccedit.cfg";

class Controller
{
  public:
    static Controller_shared create(Glib::RefPtr<Gtk::Application> app);

  protected:
    Controller(Glib::RefPtr<Gtk::Application> app);

  public:
    ~Controller();

    void destroy();

    int  run(int argc, char *argv[]);
    void quit();

    Glib::RefPtr<Gtk::Application> getGtkApp();

    void registerWindow(Gtk::Window *win);
    void unregisterWindow(Gtk::Window *win);

    int saveConfig();
    int loadConfig();

    void showProjectMgr();
    void destroyProjectMgr();

    // todo: use error type

    // function fails if
    //   project with such name already exists
    int createProject(
        std::string           name,
        std::filesystem::path path,
        bool                  save_to_config
    );

    // Project name will not be changed if
    //   (new_name == "" || new_name == name).
    // Can't be edited project which name not exists.
    // If trying to change name - function fails if
    //   project with such name already exists.
    // saveConfig called automatically on success
    int editProject(
        std::string           name,
        std::string           new_name,
        std::filesystem::path new_path
    );

    std::tuple<std::string, int>           getNameProject(ProjectCtl_shared p_ctl);
    std::tuple<std::filesystem::path, int> getPathProject(std::string name);
    std::tuple<std::filesystem::path, int> getPathProject(ProjectCtl_shared p_ctl);

    Glib::RefPtr<Gio::ListStore<ProjectTableRow>> getProjectListStore();

    std::vector<CodeEditorMod *> getBuiltinMods();

    bool isGlobalProjCtl(ProjectCtl_shared p_ctl);

    ProjectCtl_shared createGlobalProjCtl();
    // resulting pointer is empty if function failed
    ProjectCtl_shared getGlobalProjCtl();
    // this leads to closing global proj children windows
    void              destroyGlobalProjCtl();
    // this will call createGlobalProjCtl() if not created yet
    void              showGlobalProjCtlWin();
    void              destroyGlobalProjCtlWin();

    std::tuple<ProjectCtl_shared, int> createProjCtl(std::string name);
    // resulting pointer is empty if function failed
    std::tuple<ProjectCtl_shared, int> getProjCtl(std::string name);
    int                                destroyProjCtl(std::string name);

    // error if not found
    int  showProjCtlWin(std::string name);
    void destroyProjCtlWin(std::string name);

    // this also calls closeGlobalProjCtl if global ProjectCtl passed
    void destroyProjCtl(ProjectCtl_shared p_ctl);

    Glib::RefPtr<Gtk::IconTheme> icon_theme;

    std::deque<std::string> determineProjectsByFilePath(std::filesystem::path pth);

    int openFile(
        std::filesystem::path pth,
        bool                  allow_nonexisting = false,
        bool                  always_global     = false
    );

  private:
    RunOnce destroyer;

    Controller_shared own_ptr;

    std::vector<CodeEditorMod *> builtin_mods;

    Glib::RefPtr<Gtk::Application> app;

    ProjectMgr_shared project_mgr;
    ProjectCtl_shared global_proj_ctl;

    // todo: use mutex on all project_list methods?
    int findProjectIndex(std::string name);
    int findProjectIndex(ProjectCtl_shared p_ctl);

    Glib::RefPtr<Gio::ListStore<ProjectTableRow>> project_list_store;

    std::tuple<std::filesystem::path, int> getConfigDir();
    std::tuple<std::filesystem::path, int> getConfigFullPath();
    int                                    ensureConfigDirExists();

    int addBuiltinMods();
    int addBuiltinMod(CodeEditorMod *mod);

    wayround_i2p::ccutils::signal::SlotSigCCompat<void()> app_signal_startup_slot;

    void on_app_startup();
};
} // namespace wayround_i2p::ccedit

#endif
