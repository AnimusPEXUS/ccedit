#ifndef WAYROUND_I2P_20241106_133730_900611
#define WAYROUND_I2P_20241106_133730_900611

#include <memory>

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class CodeEditorAbstract
{
  public:
    virtual CodeEditorAbstract_shared getOwnPtr() = 0;

    // protected:
    //  virtual setOwnPtr(CodeEditorAbstract_shared) = 0;

  public:
    virtual WorkSubject_shared getWorkSubject()                  = 0;
    virtual bool               workSubjectIs(WorkSubject_shared) = 0;

    virtual Gtk::Window *getWindowPtr() = 0;
    virtual Gtk::Window &getWindowRef() = 0;

    virtual void show()    = 0;
    virtual void destroy() = 0;

    virtual std::size_t getCursorOffsetPosition() = 0;

    virtual void setCursorOffsetPosition(
        std::size_t new_pos,
        bool        scroll = true
    ) = 0;

    virtual std::size_t getCurrentLine()                                     = 0;
    virtual void        setCurrentLine(std::size_t line, bool scroll = true) = 0;
    virtual void        selectSlice(std::size_t start, std::size_t end)      = 0;
    virtual void        unselect()                                           = 0;
    virtual std::string getText()                                            = 0;
};

} // namespace wayround_i2p::ccedit

#endif
