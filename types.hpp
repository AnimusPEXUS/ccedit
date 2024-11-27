#ifndef WAYROUND_I2P_20241125_061830_596011
#define WAYROUND_I2P_20241125_061830_596011

// todo: delete this module?
//       I don't currently use it, and
//       probably never will

/*

class error : public std::exception
{
  public:
    error(std::string text)
    {
        this->text = text;
    }

    std::string getText();

  private:
    std::string text;
};

*/

namespace wayround_i2p::ccedit
{

template <class T>
class TableItemTpl : public Glib::Object
{
  public:
    T value;

    static Glib::RefPtr<TableItemTpl<T>> create()
    {
        return Glib::make_refptr_for_instance<TableItemTpl<T>>(
            new TableItemTpl<T>()
        );
    }

    ~TableItemTpl()
    {
    }

  protected:
    TableItemTpl() :
        Glib::ObjectBase(typeid(TableItemTpl<T>))
    {
    }
};

} // namespace wayround_i2p::ccedit

#endif
