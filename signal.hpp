#ifndef WAYROUND_I2P_20241223_224139_493806
#define WAYROUND_I2P_20241223_224139_493806

// todo: move this signal tools to ccutils

#include <deque>
#include <functional>
#include <memory>

namespace wayround_i2p::ccedit
{

template <class>
class Slot;

template <class RetT, class... Args>
class Slot<RetT(Args...)>;

template <class RetT, class... Args>
using Slot_shared = std::shared_ptr<Slot<RetT, Args...>>;

template <class RetT, class... Args>
using Slot_weak = std::weak_ptr<Slot<RetT, Args...>>;

template <class>
class SlotC;

template <class RetT, class... Args>
class SlotC<RetT(Args...)>;

template <class>
class Signal;

template <class RetT, class... Args>
class Signal<RetT(Args...)>;

template <class RetT, class... Args>
using Signal_shared = std::shared_ptr<Signal<RetT, Args...>>;

template <class RetT, class... Args>
using Signal_weak = std::weak_ptr<Signal<RetT, Args...>>;

template <class RetT, class... Args>
class Slot<RetT(Args...)>
{
  public:
    using slotted_function_type = std::function<RetT(Args...)>;

    static Slot_shared<RetT(Args...)> create(slotted_function_type fun = nullptr)
    {
        auto ret     = Slot_shared<RetT(Args...)>(new Slot(fun));
        ret->own_ptr = ret;
        return ret;
    }

  protected:
    Slot(slotted_function_type fun = nullptr) :
        fun(fun)
    {
    }

  public:
    ~Slot()
    {
    }

    void setFun(slotted_function_type fun)
    {
        this->fun = fun;
    }

    RetT on_emission(Args... args)
    {
        if (!fun)
        {
            throw "fun not set";
        }
        return fun(args...);
    }

    Slot_shared<RetT(Args...)> getOwnPtr()
    {
        return own_ptr.lock();
    }

  private:
    Slot_weak<RetT(Args...)> own_ptr;
    slotted_function_type    fun;
};

template <class RetT, class... Args>
class SlotC<RetT(Args...)> : public Slot_shared<RetT(Args...)>
{
  public:
    SlotC(std::function<RetT(Args...)> fun = nullptr) :
        Slot_shared<RetT(Args...)>(Slot<RetT(Args...)>::create(fun))
    {
    }

    ~SlotC()
    {
    }
};

template <class RetT, class... Args>
class Signal<RetT(Args...)>
{
  public:
    using slot_type        = Slot<RetT(Args...)>;
    using slot_type_shared = Slot_shared<RetT(Args...)>;
    using slot_type_weak   = Slot_weak<RetT(Args...)>;

    // note: the point here is to remove second parameter from function if
    //       it's type is void. I don't know how better to do it, so here
    //       are some bdsm with std::conditionals.

    using cb_on_result_function = std::conditional<
        std::same_as<RetT, void>,
        std::function<void(slot_type_shared slot)>,
        std::function<
            void(
                slot_type_shared                                      slot,
                std::conditional<std::same_as<RetT, void>, int, RetT> result
            )>>::type;

    Signal(cb_on_result_function cb_on_result = nullptr) :
        cb_on_result(cb_on_result)
    {
    }

    ~Signal()
    {
    }

    void set_cb_on_result(cb_on_result_function cb_on_result)
    {
        this->cb_on_result = cb_on_result;
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
            if constexpr (std::same_as<RetT, void>)
            {
                i->on_emission(args...);
                if (cb_on_result)
                {
                    cb_on_result(i);
                }
            }
            else
            {
                auto ret = i->on_emission(args...);
                if (cb_on_result)
                {
                    cb_on_result(i, ret);
                }
            }
        }

        for (auto i : shared_connected_slots)
        {
            new_connected_slots.push_back(i);
        }

        connected_slots = new_connected_slots;
    }

  private:
    std::deque<slot_type_weak> connected_slots;
    cb_on_result_function      cb_on_result;
};

} // namespace wayround_i2p::ccedit

#endif
