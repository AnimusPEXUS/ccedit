#include "ProjectSelectorForFile.hpp"

namespace wayround_i2p::ccedit
{

ProjectSelectorForFile_shared ProjectSelectorForFile::create(
    std::deque<std::string> names,
    std::filesystem::path   pth
)
{
    auto ret     = ProjectSelectorForFile_shared();
    ret->own_ptr = ret;
    return ret;
}

ProjectSelectorForFile::ProjectSelectorForFile(
    std::deque<std::string> names,
    std::filesystem::path   pth
) :
    destroyer(
        [this]()
        {
            win.destroy();
            own_ptr.reset();
        }
    ),
    names(names),
    pth(pth)
{

    win.signal_destroy().connect(
        [this]()
        { on_destroy_sig(); }
    );

    win.signal_close_request().connect(
        [this]() -> bool
        { return on_signal_close_request(); },
        true
    );
}

ProjectSelectorForFile::~ProjectSelectorForFile()
{
}

void ProjectSelectorForFile::show()
{
    win.present();
}

void ProjectSelectorForFile::destroy()
{
    destroyer.run();
}

void ProjectSelectorForFile::on_destroy_sig()
{
    std::cout << "FindFile::on_destroy_sig()" << std::endl;
    destroyer.run();
}

bool ProjectSelectorForFile::on_signal_close_request()
{
    std::cout << "FindFile::on_signal_close_request()" << std::endl;
    destroyer.run();
    return false;
}

} // namespace wayround_i2p::ccedit
