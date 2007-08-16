#include <wx/filename.h>
#include "pathresolver.h"

wxString PathResolver::run(const wxString& path, const wxString& anchor)
{
  if (path.empty())
    return wxEmptyString;

  wxString myPath, myAnchor;
  myPath = path;
  myAnchor = anchor;
  wxFileName pathObject(myPath);
  if (pathObject.IsAbsolute() ||
    myPath.Contains(_T("http://")))
    return myPath;
  
  // check anchor
  wxFileName anchorObject(myAnchor);
  if (myAnchor.empty()) { myAnchor = wxFileName::GetCwd(); }
  else if (!anchorObject.IsDir())
  {
    myAnchor = anchorObject.GetPath();
  }

  pathObject.MakeAbsolute(myAnchor);
  
  return pathObject.GetFullPath();
}

std::string PathResolver::run(
  const std::string& path,
  const std::string& anchor)
{
  wxString widePath, wideAnchor;
  widePath = wxString(path.c_str(), wxConvUTF8, path.size());
  wideAnchor = wxString(anchor.c_str(), wxConvUTF8, anchor.size());
  wxString wideReturn = PathResolver::run(widePath, wideAnchor);
  return (const char *)wideReturn.mb_str(wxConvUTF8);
}
