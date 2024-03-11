#ifndef WAYROUND_I2P_20240311_144846_112236
#define WAYROUND_I2P_20240311_144846_112236

#include <filesystem>
#include <vector>

#include <gtkmm.h>
// #include <gtkmm/icontheme.h>

#include "ProjectListItem.hpp"
#include "ProjectMgr.hpp"

#include "modularity.hpp"

#include "tables.hpp"

namespace wayround_i2p
{
namespace codeeditor
{

    class ProjectMgr;
    class ProjectTableRow;

    struct CodeEditorMod;

    class Controller //: public Gtk::Window
    {
      public:
        Controller(Glib::RefPtr<Gtk::Application> app);
        ~Controller();

        std::shared_ptr<Controller> own_ptr;

        int run(int argc, char *argv[]);

        void showProjectMgr();
        void cleanupProjectMgr();

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

        std::tuple<
            std::string,
            int>
            getNameProject(std::shared_ptr<ProjectCtl> p_ctl);

        std::tuple<
            std::string,
            int>
            getNameProject(ProjectCtl *p_ctl);

        std::tuple<
            std::filesystem::path,
            int>
            getPathProject(std::string name);

        std::tuple<
            std::filesystem::path,
            int>
            getPathProject(std::shared_ptr<ProjectCtl> p_ctl);

        std::tuple<
            std::filesystem::path,
            int>
            getPathProject(ProjectCtl *p_ctl);

        Glib::RefPtr<Gtk::Application> getGtkApp();

        void registerWindow(std::shared_ptr<Gtk::Window> win);
        void registerWindow(Gtk::Window *win);

        int saveConfig();
        int loadConfig();

        Glib::RefPtr<Gio::ListStore<ProjectTableRow>> getProjectListStore();
        std::vector<CodeEditorMod *>                  getBuiltinMods();

        bool isGlobalProjCtl(std::shared_ptr<ProjectCtl> p_ctl);
        bool isGlobalProjCtl(ProjectCtl *p_ctl);

        std::shared_ptr<ProjectCtl> createGlobalProjCtl();
        std::shared_ptr<ProjectCtl> getGlobalProjCtl();   // resulting pointer is empty if function failed
        void                        closeGlobalProjCtl(); // this leads to closing global proj children windows

        void showGlobalProjCtlWin();                      // this will call createGlobalProjCtl() if not created yet
        void closeGlobalProjCtlWin();

        std::tuple<std::shared_ptr<ProjectCtl>, int> createProjCtl(std::string name);
        std::tuple<std::shared_ptr<ProjectCtl>, int> getProjCtl(std::string name); // resulting pointer is empty if function failed
        int                                          closeProjCtl(std::string name);

        int  showProjCtlWin(std::string name);                                     // error if not found
        void closeProjCtlWin(std::string name);

        // this also calls closeGlobalProjCtl if global ProjectCtl passed
        void closeProjCtl(std::shared_ptr<ProjectCtl> p_ctl);
        void closeProjCtl(ProjectCtl *);

        Glib::RefPtr<Gtk::IconTheme> icon_theme;

        // void emitProjectListStoreItemsChanged();

      private:
        Glib::RefPtr<Gtk::Application> app;

        std::shared_ptr<ProjectMgr> project_mgr;
        std::shared_ptr<ProjectCtl> global_proj_ctl;

        // todo: use mutex on all project_list methods?
        Glib::RefPtr<Gio::ListStore<ProjectTableRow>> project_list_store;

        void on_app_startup();

        int findProjectIndex(std::string name);
        int findProjectIndex(std::shared_ptr<ProjectCtl> p_ctl);
        int findProjectIndex(ProjectCtl *p_ctl);

        std::tuple<std::filesystem::path, int> getConfigDir();
        std::tuple<std::filesystem::path, int> getConfigFullPath();
        int                                    ensureConfigDirExists();

        std::vector<CodeEditorMod *> builtin_mods;

        int addBuiltinMods();
        int addBuiltinMod(CodeEditorMod *mod);
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
