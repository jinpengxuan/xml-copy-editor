#include "getlinuxappdir.h"
#include <wx/filename.h>

wxString GetLinuxAppDir::run()
{
  wxString s;
  const int stringArrayLen = 2;
  wxString stringArray[stringArrayLen];
  stringArray[0] = _T("/usr/share/xmlcopyeditor");
  stringArray[1] = _T("/usr/local/share/xmlcopyeditor");

  for (int i = 0; i < stringArrayLen; i++)
  {
    s = stringArray[i];
    if (wxFileName::DirExists(s))
      break;
  }
  if (s.empty())
    s = wxGetCwd();

  return s;
}
