#include "ProjectCtlTables.hpp"

namespace wayround_i2p::ccedit
{

/*
Glib::RefPtr<WorkSubjectTableRow> WorkSubjectTableRow::create()
{
    return Glib::make_refptr_for_instance<WorkSubjectTableRow>(
        new WorkSubjectTableRow()
    );
}

WorkSubjectTableRow::WorkSubjectTableRow() :
    Glib::ObjectBase(typeid(WorkSubjectTableRow))
{
}

WorkSubjectTableRow::~WorkSubjectTableRow()
{
}
*/

Glib::RefPtr<CodeEditorTableRow> CodeEditorTableRow::create()
{
    return Glib::make_refptr_for_instance<CodeEditorTableRow>(
        new CodeEditorTableRow()
    );
}

CodeEditorTableRow::CodeEditorTableRow() :
    Glib::ObjectBase(typeid(CodeEditorTableRow))
{
}

CodeEditorTableRow::~CodeEditorTableRow()
{
}

} // namespace wayround_i2p::ccedit
