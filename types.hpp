#ifndef TYPES_HPP
#define TYPES_HPP

using namespace std;

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
