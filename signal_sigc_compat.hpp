#ifndef WAYROUND_I2P_20241210_171354_6621950
#define WAYROUND_I2P_20241210_171354_6621950

#include <sigc++/sigc++.h>

#include "signal.hpp"

namespace wayround_i2p::ccedit
{

template <class>
class SlotSigCCompat;

template <class RetT, class... Args>
class SlotSigCCompat<RetT(Args...)>;

template <class RetT, class... Args>
class SlotSigCCompat<RetT(Args...)>
    : public SlotC<RetT(Args...)>,
      public sigc::trackable
{
  public:
    SlotSigCCompat(std::function<RetT(Args...)> fun = nullptr) :
        SlotC<RetT(Args...)>(fun)
    {
    }

    ~SlotSigCCompat()
    {
    }

    sigc::slot<RetT(Args...)> make_sigc_slot()
    {
        return sigc::mem_fun(this, &Slot<RetT(Args...)>::on_emission);
    }
};
} // namespace wayround_i2p::ccedit

#endif
