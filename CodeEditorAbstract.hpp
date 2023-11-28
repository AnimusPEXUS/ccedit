#ifndef CODE_EDITOR_ABSTRACT_HPP
#define CODE_EDITOR_ABSTRACT_HPP

#include <memory>

namespace wayround_i2p
{
namespace codeeditor
{
    class CodeEditorAbstract
    {
      public:
        virtual void                                show()        = 0;
        virtual void                                close()       = 0;
        virtual std::shared_ptr<CodeEditorAbstract> getOwnPtr()   = 0;
        virtual void                                resetOwnPtr() = 0;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
