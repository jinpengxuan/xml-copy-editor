#ifndef XML_DOC_H
#define XML_DOC_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/print.h>
#include "xmlctrl.h"

class XmlDoc : public XmlCtrl
{
  public:
    XmlDoc(
      wxWindow *parent,
      XmlCtrlProperties properties,
      bool *protectTags,
      int visibilityState = SHOW_TAGS,//bool hideAttributes,
      int type = FILE_TYPE_XML,
      wxWindowID id = wxID_ANY,
      //const std::string& buffer = DEFAULT_XML_DECLARATION_UTF8,
      const char *buffer = NULL,
      size_t bufferLen = 0,
      const std::string& catalogPath = "",
      const std::string& basePath = "",
      const std::string& auxPath = "",
      const wxPoint& position = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0);
    wxString& getDirectory();
    wxString& getFullFileName();
    wxString& getShortFileName();
    wxDateTime getLastModified();
    void setDirectory(const wxString& s);
    void setFullFileName(const wxString& s);
    void setShortFileName(const wxString& s);
    void setLastModified(wxDateTime dt);
  private:
    wxString directory, fullFileName, shortFileName;
    wxDateTime lastModified;
};

#endif

