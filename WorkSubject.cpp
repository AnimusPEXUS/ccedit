#include <cstdio>

#include "WorkSubject.hpp"
#include "utils.hpp"

using namespace wayround_i2p::codeeditor;

WorkSubject::WorkSubject(
    std::shared_ptr<Controller> controller,
    std::shared_ptr<ProjectCtl> project_ctl,
    std::filesystem::path       fpth
)
{
    this->controller  = controller;
    this->project_ctl = project_ctl;
    this->fpth        = fpth;

    priv_signal_modified_changed = std::shared_ptr<sigc::signal<void()>>(
        new sigc::signal<void()>()
    );

    createNew();
}

WorkSubject::~WorkSubject()
{
    std::cout << "~WorkSubject()" << std::endl;
}

std::shared_ptr<Controller> WorkSubject::getController()
{
    return controller;
}

std::shared_ptr<ProjectCtl> WorkSubject::getProject()
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
    return fpth; // fixme
}

void WorkSubject::createNew()
{
    txt_buff = Gtk::TextBuffer::create();

    txt_buff->signal_modified_changed().connect(
        sigc::mem_fun(
            *this,
            &WorkSubject::emit_signal_modified_changed
        )
    );
}

void WorkSubject::emit_signal_modified_changed()
{
    std::cout << "WorkSubject::emit_signal_modified_changed(): " << txt_buff->get_modified() << std ::endl;
    priv_signal_modified_changed->emit();
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
    txt_buff->set_text(txt);
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
    txt_buff->set_text(txt);
    return;
}

std::shared_ptr<sigc::signal<void()>> WorkSubject::signal_modified_changed()
{
    return priv_signal_modified_changed;
}
