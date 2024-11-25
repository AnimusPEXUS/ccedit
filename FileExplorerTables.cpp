#include "FileExplorerTables.hpp"

namespace wayround_i2p::ccedit
{

Glib::RefPtr<FileExplorerDirTreeRow> FileExplorerDirTreeRow::create()
{
    return Glib::make_refptr_for_instance<FileExplorerDirTreeRow>(
        new FileExplorerDirTreeRow()
    );
}

FileExplorerDirTreeRow::FileExplorerDirTreeRow() :
    Glib::ObjectBase(typeid(FileExplorerDirTreeRow))
{
}

FileExplorerDirTreeRow::~FileExplorerDirTreeRow()
{
}

Glib::RefPtr<FileExplorerFileListRow> FileExplorerFileListRow::create()
{
    return Glib::make_refptr_for_instance<FileExplorerFileListRow>(
        new FileExplorerFileListRow()
    );
}

FileExplorerFileListRow::FileExplorerFileListRow() :
    Glib::ObjectBase(typeid(FileExplorerFileListRow))
{
}

FileExplorerFileListRow::~FileExplorerFileListRow()
{
}

} // namespace wayround_i2p::ccedit
