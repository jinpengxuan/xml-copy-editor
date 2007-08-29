#include <wx/filename.h>
#include "xmldoc.h"

XmlDoc::XmlDoc(
  wxWindow *parent,
  XmlCtrlProperties properties,
  bool *protectTags,
  int visibilityState,
  int type,
  wxWindowID id,
  //const std::string& buffer,
  const char *buffer,
  size_t bufferLen,
  const std::string& catalogPath,
  const std::string& basePath,
  const std::string& auxPath,
  const wxPoint &position,
  const wxSize& size,
  long style)
  : XmlCtrl(
    parent,
    properties,
    protectTags,
    visibilityState,
    type,
    id,
    buffer,
    bufferLen, // new
    catalogPath,
    basePath,
    auxPath,
    position,
    size,
    style)
{ }

wxString& XmlDoc::getDirectory() { return directory; }
wxString& XmlDoc::getFullFileName() { return fullFileName; }
wxString& XmlDoc::getShortFileName() { return shortFileName; }
wxDateTime XmlDoc::getLastModified() { return lastModified; }

void XmlDoc::setDirectory(const wxString& s)
{
  directory = s;
}

void XmlDoc::setFullFileName(const wxString& s)
{
  fullFileName = s;
}
void XmlDoc::setShortFileName(const wxString& s) { shortFileName = s; }
void XmlDoc::setLastModified(wxDateTime dt) { lastModified = dt; }

