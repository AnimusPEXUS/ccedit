#include "FileExplorer.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorer::FileExplorer(std::shared_ptr<ProjectCtl> proj_ctl)
{

    this->proj_ctl = proj_ctl;

    few = std::shared_ptr<FileExplorerWidget>(
        new FileExplorerWidget(proj_ctl)
    );

    set_child(*few);

    signal_destroy().connect(
        sigc::mem_fun(*this, &FileExplorer::on_destroy_sig)
    );
}

FileExplorer::~FileExplorer()
{
    std::cout << "~FileExplorer()" << std::endl;
    cleanup_few();
    std::cout << "  proj_ctl->unregisterFileExplorer(this)" << std::endl;
    proj_ctl->unregisterFileExplorer(this);
    // std::cout << "~FileExplorer() few->destroy()" << std::endl;
    // few->destroy();
    // std::cout << "~FileExplorer() delete few" << std::endl;
    // delete few;
    // std::cout << "~FileExplorer() few.reset()" << std::endl;
    // few.reset();
    std::cout << "~FileExplorer() ok" << std::endl;
}

void FileExplorer::cleanup_few()
{
    if (few)
    {
        std::cout << "FileExplorer::cleanup_few()" << std::endl;
        // std::cout << "FileExplorer::cleanup_few() few->unparent()" << std::endl;
        // few->unparent();
        // std::cout << "FileExplorer::cleanup_few() unset_child" << std::endl;
        // unset_child();
        few.reset();
    }
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    // cleanup_few();
    // std::cout << "  delete few" << std::endl;
    // delete few;
    // std::cout << "  rm window" << std::endl;
    // proj_ctl->getController()->getGtkApp()->remove_window(*this);
    // std::cout << "  this->set_application(nullptr)" << std::endl;
    // this->set_application(nullptr);
    std::cout << "  proj_ctl->unregisterFileExplorer(this)" << std::endl;
    proj_ctl->unregisterFileExplorer(this);
    std::cout << "FileExplorer sig destroy exit" << std::endl;
}
