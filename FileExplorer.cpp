#include "FileExplorer.hpp"

using namespace wayround_i2p::codeeditor;

FileExplorer::FileExplorer(std::shared_ptr<ProjectCtl> proj_ctl)
{

    this->proj_ctl = proj_ctl;

    few = new FileExplorerWidget(proj_ctl);

    set_child(*few);

    few->set_margin_top(5);
    few->set_margin_start(5);
    few->set_margin_end(5);
    few->set_margin_bottom(5);

    signal_destroy().connect(
        sigc::mem_fun(*this, &FileExplorer::on_destroy_sig)
    );
}

FileExplorer::~FileExplorer()
{
    std::cout << "~FileExplorer()" << std::endl;
    delete few;
    few = nullptr;
    std::cout << "~FileExplorer() ok" << std::endl;
}

void FileExplorer::on_destroy_sig()
{
    std::cout << "FileExplorer sig destroy" << std::endl;
    delete this;
    std::cout << "FileExplorer sig destroy exit" << std::endl;
}
