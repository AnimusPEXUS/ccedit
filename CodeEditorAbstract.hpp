#ifndef CODE_EDITOR_ABSTRACT_HPP
#define CODE_EDITOR_ABSTRACT_HPP

#include <memory>

#include <gtkmm.h>

#include "WorkSubject.hpp"

namespace wayround_i2p
{
namespace codeeditor
{
    class WorkSubject;

    class CodeEditorAbstract
    {
      public:
        virtual std::shared_ptr<CodeEditorAbstract> getOwnPtr()   = 0;
        virtual void                                resetOwnPtr() = 0;

        virtual std::shared_ptr<WorkSubject> getWorkSubject()                            = 0;
        virtual bool                         workSubjectIs(std::shared_ptr<WorkSubject>) = 0;

        virtual void show()                                               = 0;
        virtual void present()                                            = 0;
        virtual void close()                                              = 0;
        virtual void setTransientWindow(Gtk::Window *win)                 = 0;
        virtual void setTransientWindow(std::shared_ptr<Gtk::Window> win) = 0;

        virtual unsigned int getCursorOffsetPosition() = 0;
        virtual void         setCursorOffsetPosition(
                    unsigned int new_pos,
                    bool         scroll = true
                )                                                                  = 0;
        virtual unsigned int getCurrentLine()                                      = 0;
        virtual void         setCurrentLine(unsigned int line, bool scroll = true) = 0;
        virtual void         selectSlice(unsigned int start, unsigned int end)     = 0;
        virtual void         unselect()                                            = 0;
        virtual std::string  getText()                                             = 0;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
