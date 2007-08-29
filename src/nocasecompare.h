#include <string>

using namespace std;

class NoCaseCompare
{
  public:  
    NoCaseCompare();
    ~NoCaseCompare();

    bool operator() (const string& s, const string& t) const
    {
      return lexicographical_compare(
        s.begin(), s.end(),
        t.begin(), t.end(),
        noCaseCompare);
    }
  private:
    static bool noCaseCompare(char c1, char c2)
    {
        return toupper(c1) < toupper(c2);
    }
};
