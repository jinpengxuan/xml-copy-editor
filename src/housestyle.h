#ifndef HOUSESTYLE_H
#define HOUSESTYLE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <boost/shared_ptr.hpp>
#include "stringset.h"
#include "contexthandler.h"
#include "xmlrulereader.h"
#include "housestylereader.h"
#include "xmlfilterreader.h"
#include "spellcheck.h"
#include "casehandler.h"

class HouseStyle
{
  public:
    HouseStyle(
      const std::string& bufferParameter,
      const std::string& ruleDirectoryParameter,
      const std::string& ruleFileParameter,
      const std::string& filterDirectoryParameter,
      const std::string& filterFileParameter,
      const std::string& pathSeparatorParameter,
      int contextRangeParameter);
    ~HouseStyle();
    bool createReport();
    std::string getLastError();
    std::vector<ContextMatch> getMatchVector();
  private:
    std::string
      buffer,
      ruleDirectory,
      ruleFile,
      filterDirectory,
      filterFile,
      pathSeparator,
      error;
    int contextRange;
    boost::shared_ptr<std::vector<boost::shared_ptr<Rule> > > ruleVector;
    std::map<std::string, std::map<std::string, std::set<std::string> > >
      filterMap;
    std::vector<ContextMatch> matchVector;
    boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
    void collectFilter(
      std::string& fileName,
      std::set<std::string>& excludeSet,
      int *filterCount);
    void collectRules(
      string& fileName,
      boost::shared_ptr<std::vector<boost::shared_ptr<Rule> > > ruleVector,
      std::set<string>& excludeSet,
      int *ruleCount);
    int updateFilter();
    int updateRules();
};

#endif
