#ifndef WAYROUND_I2P_20241223_224139_522147
#define WAYROUND_I2P_20241223_224139_522147

#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class CodeEditorAbstract
{
  public:
    virtual CodeEditorAbstract_shared getOwnPtr() const = 0;

    virtual ProjectCtl_shared getProjectCtl() const = 0;

    virtual WorkSubject_shared getWorkSubject() const                  = 0;
    virtual bool               workSubjectIs(WorkSubject_shared) const = 0;

    virtual Gtk::Window *getWindowPtr() = 0;
    virtual Gtk::Window &getWindowRef() = 0;

    virtual void show()    = 0;
    virtual void destroy() = 0;

    virtual std::size_t getCursorOffsetPosition() const = 0;

    virtual void setCursorOffsetPosition(
        std::size_t new_pos,
        bool        scroll = true
    ) = 0;

    virtual std::size_t getCurrentLine() const                               = 0;
    virtual void        setCurrentLine(std::size_t line, bool scroll = true) = 0;
    virtual void        selectSlice(std::size_t start, std::size_t end)      = 0;
    virtual void        unselect()                                           = 0;
    virtual std::string getText() const                                      = 0;
};

} // namespace wayround_i2p::ccedit

#endif
