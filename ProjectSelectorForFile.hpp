#ifndef WAYROUND_I2P_20241223_224139_474947
#define WAYROUND_I2P_20241223_224139_474947

/*
 TODO:
 this functionality is on halt. ccedit should first learn
 to work as server and accept new calls to it's executable and work with
 d-bus and other desktop integration tools.
*/

#include <deque>
#include <iostream>
#include <string>

#include <gtkmm.h>

#include "forward_declarations.hpp"
#include "utils.hpp"

namespace wayround_i2p::ccedit
{

class ProjectSelectorForFile
{
  public:
    static ProjectSelectorForFile_shared create(
        std::deque<std::string> names,
        std::filesystem::path   pth
    );

  protected:
    ProjectSelectorForFile(
        std::deque<std::string> names,
        std::filesystem::path   pth
    );

  public:
    ~ProjectSelectorForFile();

    void show();
    void destroy();

  private:
    RunOnce                       destroyer;
    ProjectSelectorForFile_shared own_ptr;

    std::deque<std::string> names;
    std::filesystem::path   pth;

    Gtk::ApplicationWindow win;

    Gtk::Box box_for_radios;

    Gtk::Box bb;

    Gtk::Button open_btn;
    Gtk::Button cancel_btn;

    void on_destroy_sig();
    bool on_signal_close_request();
};

} // namespace wayround_i2p::ccedit

#endif
