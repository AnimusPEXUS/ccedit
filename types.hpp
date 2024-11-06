#ifndef WAYROUND_I2P_20241106_133730_900154
#define WAYROUND_I2P_20241106_133730_900154

// todo: delete this module?
//       I don't currently use it, and
//       probably never will

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

#endif
