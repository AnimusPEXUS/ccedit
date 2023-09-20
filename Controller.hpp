#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <filesystem>
#include <vector>

#include <gtkmm.h>

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

        int createProject(
            std::string           name,
            std::filesystem::path path,
            bool                  save_to_config
        );

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

        Glib::RefPtr<Gtk::Application>
            getGtkApp();

        int saveConfig();
        int loadConfig();

        Glib::RefPtr<Gio::ListStore<ProjectTableRow>> getProjectListStore();
        std::vector<CodeEditorMod *>                  getBuiltinMods();

        void showGlobalProjCtl();
        void cleanupGlobalProjCtl();
        bool isGlobalProjCtl(std::shared_ptr<ProjectCtl> p_ctl);
        bool isGlobalProjCtl(ProjectCtl *p_ctl);

        void showProjCtl(std::string name);
        void cleanupProjCtl(std::string name);
        void cleanupProjCtl(std::shared_ptr<ProjectCtl> p_ctl);
        void cleanupProjCtl(ProjectCtl *p_ctl);

      private:
        Glib::RefPtr<Gtk::Application> app;

        std::shared_ptr<ProjectMgr> project_mgr;
        std::shared_ptr<ProjectCtl> global_proj_ctl;

        // todo: use mutex on all project_list methods?
        Glib::RefPtr<Gio::ListStore<ProjectTableRow>> project_list_store;

        void on_app_startup();

        int findProjectIndex(std::string name);

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
