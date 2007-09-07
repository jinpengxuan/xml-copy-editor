#include <cstring>
#include "replace.h"

int Replace::run(
  std::string& buffer,
  const std::string& find,
  const std::string& replace,
  bool matchCase)
{
  if (buffer.empty() || find.empty())
    return 0;

  std::string output;
  size_t capacity = buffer.size();
  int diff = replace.size() - find.size();
  if (diff > 0)
    capacity += diff * BUFSIZ;

  output.reserve(capacity);
  size_t findLength = find.size();
  int matchCount = 0;

  // comparison function pointer
  int (*comp)(const char *, const char *, size_t) =
#ifdef __WXMSW__
    (matchCase) ? strncmp : strnicmp;
#else
    (matchCase) ? strncmp : strncasecmp;
#endif

  char *bufferPtr;
  const char *findPtr, *replacePtr;
  bufferPtr = (char *)buffer.c_str();
  findPtr = find.c_str();
  replacePtr = replace.c_str();
  
  while (*bufferPtr)
  {
    if (!comp((const char *)bufferPtr, findPtr, findLength))
    {
      ++matchCount;
      output += replacePtr;
      bufferPtr += findLength;
    }
    else
    {
      output += *bufferPtr;
      ++bufferPtr;
    }
  }
  if (!matchCount)
    return 0;
  buffer = output;
  return matchCount;
}
