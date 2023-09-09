#include <cstdio>
#include <experimental/scope>

#include "WorkSubject.hpp"

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

std::filesystem::path WorkSubject::getPath()
{
    return fpth;
}

std::filesystem::path WorkSubject::getFullPath()
{
    return fpth; // fixme
}

void WorkSubject::createNew()
{
    txt_buff = Gtk::TextBuffer::create();
}

int WorkSubject::reload()
{
    return reload(false);
}

const size_t size2mib = 1024 * 1024 * 2;

int WorkSubject::reload(bool allow_nonexist)
{
    auto fp = getFullPath();

    // todo: mode tests?
    if (!std::filesystem::exists(fp))
    {
        if (allow_nonexist)
        {
            txt_buff->set_text("");
            return 0;
        }
        else
        {
            return 1;
        }
    }

    int         err     = 0;
    int         err_eof = 0;
    std::string txt("");
    char       *buffer = new char[size2mib];

    if (sizeof(buffer[0]) != 1)
    {
        throw std::length_error("unsupported char size: not 1");
    }

    auto se01 = std::experimental::fundamentals_v3::scope_exit(
        [buffer]()
        {
            // todo: remove debug code
            std::cout << "scope_exit 1" << std::endl;
            delete buffer;
        }
    );

    auto f = std::fopen(fp.c_str(), "r");
    if (err = std::ferror(f); err != 0)
    {
        return err;
    }
    auto se02 = std::experimental::fundamentals_v3::scope_exit(
        [f]()
        {
            // todo: remove debug code
            std::cout << "scope_exit 2" << std::endl;
            std::fclose(f);
        }
    );

    std::cout << "while" << std::endl;
    while (true)
    {
        err     = std::ferror(f);
        err_eof = std::feof(f);

        // todo: remove debug code
        std::cout << "err: " << err << " err_eof: " << err_eof << std::endl;

        if (err != 0)
        {
            return err;
        }

        if (err_eof != 0)
        {
            txt_buff->set_text(txt);
            return 0;
        }
        auto readed = std::fread(buffer, sizeof(buffer[0]), size2mib, f);
        txt.append(std::string(buffer, readed));
        // std::cout << "while2" << std::endl;
    }
}

int WorkSubject::save()
{
    int  err = 0;
    auto fp  = getFullPath();

    // todo: ensure directory exists. create if needed.

    auto txt      = txt_buff->get_text();
    auto txt_c    = txt.c_str();
    auto txt_size = txt.size();

    if (sizeof(txt_c[0]) != 1)
    {
        throw std::length_error("unsupported char size: not 1");
    }

    auto f = std::fopen(fp.c_str(), "w");
    if (err = std::ferror(f); err != 0)
    {
        return err;
    }
    auto se02 = std::experimental::fundamentals_v3::scope_exit(
        [f]()
        {
            // todo: remove debug code
            std::cout << "scope_exit 2" << std::endl;
            std::fclose(f);
        }
    );

    auto offset = 0;

    while (true)
    {
        if (offset == txt_size)
        {
            break;
        }
        auto length_minus_offset  = txt_size - offset;
        auto count_to_write       = (size2mib <= length_minus_offset ? size2mib : length_minus_offset);
        auto writen_count         = std::fwrite(txt_c + offset, sizeof(txt_c[0]), count_to_write, f);
        offset                   += writen_count;
    }

    return 0;
}

bool WorkSubject::modified()
{
    return 1;
}

Glib::RefPtr<Gtk::TextBuffer> WorkSubject::getTextBuffer()
{
    return txt_buff;
}

void WorkSubject::getBinaryBuffer()
{
    // todo:
}
