#include <string>
#include "rule.h"

using namespace std;

Rule::Rule(
  const string& pattern,
  bool matchCase,
  const string& replace,
  const int arrayLength) : WrapRegex(
  pattern,
  matchCase,
  replace,
  arrayLength)
{ }

bool Rule::getAdjustCaseAttribute()
{
  return adjustCaseAttribute;
}

bool Rule::getTentativeAttribute()
{
  return tentativeAttribute;
}

string Rule::getReport()
{
  return reportString;
}

void Rule::setAdjustCaseAttribute(bool b)
{
  adjustCaseAttribute = b;
}

void Rule::setTentativeAttribute(bool b)
{
  tentativeAttribute = b;
}

void Rule::setReport(string &s)
{
  reportString = s;
}
