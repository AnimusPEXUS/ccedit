#include "CommonEditorWindow.hpp"

using namespace wayround_i2p::codeeditor;

CommonEditorWindow::CommonEditorWindow(
    std::shared_ptr<ProjectCtl>  project_ctl,
    std::shared_ptr<WorkSubject> subject
) :
    main_box(Gtk::Orientation::VERTICAL, 5)
{
    this->project_ctl = project_ctl;
    this->subject     = subject;

    set_child(main_box);

    main_box.append(paned);

    paned.set_start_child(text_view_sw);
    paned.set_end_child(outline_view_sw);

    paned.set_vexpand(true);

    text_view_sw.set_child(text_view);
    outline_view_sw.set_child(outline_view);

    text_view.set_buffer(subject->getTextBuffer());

    signal_destroy().connect(
        sigc::mem_fun(*this, &CommonEditorWindow::on_destroy_sig)
    );
}

CommonEditorWindow::~CommonEditorWindow()
{
    std::cout << "~CommonEditorWindow()" << std::endl;
}

void CommonEditorWindow::saveOwnPtr(std::shared_ptr<CodeEditorAbstract> val)
{
    own_ptr = val;
}

void CommonEditorWindow::show()
{
    auto x = (Gtk::Window *)this;
    x->show();
}

void CommonEditorWindow::close()
{
    auto x = (Gtk::Window *)this;
    x->close();
}

void CommonEditorWindow::on_destroy_sig()
{
    project_ctl->unregisterEditor(own_ptr);
    own_ptr.reset();
}
