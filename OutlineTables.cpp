#include "OutlineTables.hpp"

namespace wayround_i2p::ccedit
{

Glib::RefPtr<OutlineTableRow> OutlineTableRow::create()
{
    return Glib::make_refptr_for_instance<OutlineTableRow>(
        new OutlineTableRow()
    );
}

OutlineTableRow::OutlineTableRow() :
    Glib::ObjectBase(typeid(OutlineTableRow))
{
}

OutlineTableRow::~OutlineTableRow()
{
}

} // namespace wayround_i2p::ccedit
