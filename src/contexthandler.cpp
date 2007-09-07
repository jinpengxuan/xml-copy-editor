#include <string>
#include "contexthandler.h"

ContextMatch ContextHandler::getContext(
  char *s,
  int len,
  char *origin,
  int context)
{
  char *t, *u;
  int maxrange = context * 20;
  
  int range = context + 1, i = 0;
  for (t = s; t != origin; --t)
    if (*t == ' ' || *t == '\t' || *t == '\r' || *t == '\n')
      if (++i == range)
      {
        ++t;
        break;
      }

  i = 0;
  for (u = s + len; *u; ++u)
    if (*u == ' ' || *u == '\t' || *u == '\r' || *u == '\n')
      if (++i == range)
        break;
  
  ContextMatch match;
  
  // prevent excessive context
  if ((s - t) < maxrange)
    match.prelog.assign(t, s - t);
  else
    match.prelog = "...";
  match.match.assign(s, len);
  if ((u - s) < maxrange)
    match.postlog.assign(s + len, u - (s + len));
  else
    match.postlog = "...";
  
  return match;
}
