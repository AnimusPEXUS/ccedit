#ifndef WAYROUND_I2P_20241125_061830_596011
#define WAYROUND_I2P_20241125_061830_596011

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
