#ifndef WAYROUND_I2P_20241210_171354_651218
#define WAYROUND_I2P_20241210_171354_651218

#include <gtkmm.h>

namespace wayround_i2p::ccedit
{

class OutlineTableRow : public Glib::Object
{
  public:
    unsigned int line;
    std::string  text;

    static Glib::RefPtr<OutlineTableRow> create();

  protected:
    OutlineTableRow();

  public:
    ~OutlineTableRow();
};

} // namespace wayround_i2p::ccedit

#endif
