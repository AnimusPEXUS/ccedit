#ifndef WAYROUND_I2P_20241204_051018_595909
#define WAYROUND_I2P_20241204_051018_595909

#include <filesystem>

#include <gtkmm.h>

namespace wayround_i2p::ccedit
{

class FileExplorerDirTreeRow : public Glib::Object
{
  public:
    // std::string filename;

    // todo: retry finding better solution?
    // note: I have to store here full subpath,
    //       because Gtk::TreeListModel's SlotCreateModel
    //       doesn't gives way to traceback expanded row parents
    //       and build path by single filename().
    //       I don't like this, but I don't have other choice.

    std::filesystem::path pth;

    static Glib::RefPtr<FileExplorerDirTreeRow> create();

  protected:
    FileExplorerDirTreeRow();

  public:
    ~FileExplorerDirTreeRow();
};

class FileExplorerFileListRow : public Gtk::Box
{
  public:
    // std::string filename;

    // todo: I have to store here full path,
    //       because Gtk::TreeListModel's SlotCreateModel
    //       doesn't gives way to traceback expanded row parents
    //       and build path by single filename().
    //       I don't like this, but I don't have other choice.

    std::filesystem::path pth;

    static Glib::RefPtr<FileExplorerFileListRow> create();

  protected:
    FileExplorerFileListRow();

  public:
    ~FileExplorerFileListRow();
};

} // namespace wayround_i2p::ccedit

#endif
