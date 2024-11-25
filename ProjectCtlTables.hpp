#ifndef WAYROUND_I2P_20241125_061830_595300
#define WAYROUND_I2P_20241125_061830_595300

#include <gtkmm.h>

#include "forward_declarations.hpp"

namespace wayround_i2p::ccedit
{

class WorkSubjectTableRow : public Glib::Object
{
  public:
    WorkSubject_shared work_subj;

    static Glib::RefPtr<WorkSubjectTableRow> create();

  protected:
    WorkSubjectTableRow();
};

class CodeEditorTableRow : public Glib::Object
{
  public:
    CodeEditorAbstract_shared editor;

    static Glib::RefPtr<CodeEditorTableRow> create();

  protected:
    CodeEditorTableRow();
};

} // namespace wayround_i2p::ccedit

#endif
