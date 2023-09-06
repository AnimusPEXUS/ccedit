#ifndef CODEEDITORABSTRACT_HPP
#define CODEEDITORABSTRACT_HPP

#include <memory>

namespace wayround_i2p
{
namespace codeeditor
{
    class CodeEditorAbstract
    {
      public:
        virtual void saveOwnPtr(std::shared_ptr<CodeEditorAbstract>) = 0;
        virtual void show()                                          = 0;
        virtual void close()                                         = 0;
    };
} // namespace codeeditor
} // namespace wayround_i2p

#endif
