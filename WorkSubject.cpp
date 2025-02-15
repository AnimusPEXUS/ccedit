#include <cstdio>

#include "WorkSubject.hpp"

#include "ProjectCtl.hpp"

#include "utils.hpp"

namespace wayround_i2p::ccedit
{

std::tuple<WorkSubject_shared, int>
    WorkSubject::create(
        ProjectCtl_shared     project_ctl,
        std::filesystem::path fpth
    )
{

    auto [norm_path, err]
        = project_ctl->pathNormalizeAndTrimPrefix(fpth);

    if (err != 0)
    {
        return {nullptr, err};
    }

    auto ret = WorkSubject_shared(
        new WorkSubject(
            project_ctl,
            fpth
        )
    );
    ret->own_ptr = ret;
    project_ctl->registerWorkSubject(ret);
    return {ret, 0};
}

WorkSubject::WorkSubject(
    ProjectCtl_shared     project_ctl,
    std::filesystem::path fpth
) :
    destroyer(
        [this]()
        {
            this->project_ctl->destroyWorkSubjectEditors(own_ptr);
            this->project_ctl->unregisterWorkSubject(own_ptr);
            own_ptr.reset();
        }
    )
{

    this->project_ctl = project_ctl;
    this->fpth        = fpth.lexically_normal();

    this->controller = project_ctl->getController();

    createNew();
    std::cout << "project path: " << project_ctl->getProjectPath() << std::endl;
    std::cout << "   fpth        : " << fpth << std::endl;
    std::cout << "   this->fpth  : " << this->fpth << std::endl;
    std::cout << "   ws rel  path: " << getPath() << std::endl;
    std::cout << "   ws full path: " << getFullPath() << std::endl;
}

WorkSubject::~WorkSubject()
{
    std::cout << "WorkSubject::~WorkSubject()" << std::endl;
    destroyer.run();
}

void WorkSubject::destroy()
{
    std::cout << "WorkSubject::destroy()" << std::endl;
    destroyer.run();
}

Controller_shared WorkSubject::getController()
{
    return controller;
}

ProjectCtl_shared WorkSubject::getProject()
{
    return project_ctl;
}

// relative to project root
std::filesystem::path WorkSubject::getPath()
{
    return fpth;
}

// complete system filepath
std::filesystem::path WorkSubject::getFullPath()
{
    // todo: sanity check
    return project_ctl->getProjectPath() / fpth;
}

void WorkSubject::createNew()
{
    txt_buff = Gtk::TextBuffer::create();

    on_buffer_changed_slot->setFun(
        [this]()
        { emit_signal_modified_changed(); }
    );

    txt_buff->signal_modified_changed().connect(on_buffer_changed_slot);
}

void WorkSubject::emit_signal_modified_changed()
{
    std::cout << "WorkSubject::emit_signal_modified_changed(): "
              << txt_buff->get_modified() << std ::endl;
    priv_signal_modified_changed.emit();
}

// shortcut to reload() with allow_nonexist=false
int WorkSubject::reload()
{
    return reload(false);
}

int WorkSubject::reload(bool allow_nonexist)
{
    auto [txt, err] = loadStringFromFile(
        getFullPath(),
        allow_nonexist
    );
    if (err != 0)
    {
        return err;
    }
    setText(txt);
    txt_buff->set_modified(false);
    return 0;
}

int WorkSubject::save()
{
    auto err = saveStringToFile(
        getFullPath(),
        txt_buff->get_text()
    );
    txt_buff->set_modified(false);
    return err;
}

bool WorkSubject::modified()
{
    return txt_buff->get_modified();
}

void WorkSubject::modified(bool val)
{
    txt_buff->set_modified(val);
}

Glib::RefPtr<Gtk::TextBuffer> WorkSubject::getTextBuffer()
{
    return txt_buff;
}

void WorkSubject::getBinaryBuffer()
{
    // todo:
}

std::string WorkSubject::getText()
{
    return txt_buff->get_text();
}

void WorkSubject::setText(std::string txt)
{
    priv_signal_editors_save_state.emit();
    txt_buff->set_text(txt);
    priv_signal_editors_restore_state.emit();
    return;
}

wayround_i2p::ccutils::signal::Signal<void()> &WorkSubject::signal_editors_save_state()
{
    return priv_signal_editors_save_state;
}

wayround_i2p::ccutils::signal::Signal<void()> &WorkSubject::signal_editors_restore_state()
{
    return priv_signal_editors_restore_state;
}

wayround_i2p::ccutils::signal::Signal<void()> &WorkSubject::signal_modified_changed()
{
    return priv_signal_modified_changed;
}

} // namespace wayround_i2p::ccedit
