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

    struct CodeEditorModule;

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
            std::string name,
            std::string path,
            bool        save_to_config
        );

        int editProject(
            std::string name,
            std::string new_name,
            std::string new_path
        );

        std::tuple<std::string, int>
            getPathProject(std::string name);

        Glib::RefPtr<Gtk::Application>
            getGtkApp();

        int saveConfig();
        int loadConfig();

        Glib::RefPtr<Gio::ListStore<ProjectTableRow>> getProjectListStore();
        std::vector<CodeEditorModule *>               getBuiltinModules();

        int addBuiltinModule(CodeEditorModule *module);

        void showGlobalProjCtl();
        void cleanupGlobalProjCtl();

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

        std::vector<CodeEditorModule *> builtin_modules;

        int addBuiltinModules();
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
