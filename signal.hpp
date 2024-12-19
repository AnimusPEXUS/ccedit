#ifndef WAYROUND_I2P_20241210_171354_662195
#define WAYROUND_I2P_20241210_171354_662195

// todo: move this signal tools to ccutils

#include <deque>
#include <functional>
#include <memory>

namespace wayround_i2p::ccedit
{

template <class>
class Slot;

template <class... Args>
class Slot<void(Args...)>;

template <class... Args>
using Slot_shared = std::shared_ptr<Slot<Args...>>;

template <class... Args>
using Slot_weak = std::weak_ptr<Slot<Args...>>;

template <class>
class SlotC;

template <class... Args>
class SlotC<void(Args...)>;

template <class>
class Signal;

template <class... Args>
class Signal<void(Args...)>;

template <class... Args>
using Signal_shared = std::shared_ptr<Signal<Args...>>;

template <class... Args>
using Signal_weak = std::weak_ptr<Signal<Args...>>;

template <class... Args>
class Slot<void(Args...)>
{
  public:
    static Slot_shared<void(Args...)> create(std::function<void(Args...)> fun = nullptr)
    {
        auto ret     = Slot_shared<void(Args...)>(new Slot(fun));
        ret->own_ptr = ret;
        return ret;
    }

  protected:
    Slot(std::function<void(Args...)> fun = nullptr) :
        fun(fun)
    {
    }

  public:
    ~Slot()
    {
    }

    void setFun(std::function<void(Args...)> fun)
    {
        this->fun = fun;
    }

    void on_emission(Args... args)
    {
        if (!fun)
        {
            throw "fun not set";
        }
        fun(args...);
    }

    Slot_shared<void(Args...)> getOwnPtr()
    {
        return own_ptr.lock();
    }

  private:
    Slot_weak<void(Args...)>     own_ptr;
    std::function<void(Args...)> fun;
};

template <class... Args>
class SlotC<void(Args...)> : public Slot_shared<void(Args...)>
{
  public:
    SlotC(std::function<void(Args...)> fun = nullptr) :
        Slot_shared<void(Args...)>(Slot<void(Args...)>::create(fun))
    {
    }

    ~SlotC()
    {
    }
};

template <class... Args>
class Signal<void(Args...)>
{
  public:
    using slot_type        = Slot<void(Args...)>;
    using slot_type_shared = Slot_shared<void(Args...)>;
    using slot_type_weak   = Slot_weak<void(Args...)>;

    Signal()
    {
    }

    ~Signal()
    {
    }

    void connect(slot_type_shared slot_)
    {
        for (const auto &i : connected_slots)
        {
            if (i.lock() == slot_)
            {
                return;
            }
        }
        connected_slots.push_back(slot_);
    }

    void disconnect(slot_type_shared slot_)
    {
        std::deque<slot_type_weak> new_connected_slots;

        for (auto i : connected_slots)
        {
            if (i.lock() != slot_)
            {
                new_connected_slots.push_back(i);
            }
        }

        connected_slots = new_connected_slots;
    }

    void emit(Args... args)
    {
        std::deque<slot_type_weak> new_connected_slots;

        std::deque<slot_type_shared> shared_connected_slots;

        for (auto i : connected_slots)
        {
            auto x = i.lock();
            if (x)
            {
                shared_connected_slots.push_back(x);
            }
        }

        for (auto i : shared_connected_slots)
        {
            i->on_emission(args...);
        }

        for (auto i : shared_connected_slots)
        {
            new_connected_slots.push_back(i);
        }

        connected_slots = new_connected_slots;
    }

  private:
    std::deque<slot_type_weak> connected_slots;
};

} // namespace wayround_i2p::ccedit

#endif
