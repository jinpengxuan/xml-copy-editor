#ifndef PATH_RESOLVER_H
#define PATH_RESOLVER_H

#include <string>
#include <wx/wx.h>

class PathResolver
{
  public:
    static wxString run(
      const wxString& path,
      const wxString& anchor = wxEmptyString);
    static std::string run(
        const std::string& path,
        const std::string& anchor = "");
};

#endif
