#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "contexthandler.h"
#include "stringset.h"

using namespace std;

class Spellcheck
{
  public:
    Spellcheck(
      boost::shared_ptr<StringSet<char> > dictionaryParameter,
      boost::shared_ptr<StringSet<char> > passiveDictionaryParameter);
    ~Spellcheck();
    inline bool checkWord(string &s);
    void checkString(
      string &s,
      vector<ContextMatch> &v,
      int contextRange);
    string getSuggestion(string &s);
  private:
    boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
    bool checkWord(char *s, size_t len);
    char *getWord(char **s, size_t *len);
    inline bool isWordCharacter(char *s, size_t *bytes);
};
#endif
