#ifndef WRAP_TEMPFILENAME_H
#define WRAP_TEMPFILENAME_H

#include <string>
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/strconv.h>

class WrapTempFileName
{
public:
    WrapTempFileName ( wxString hint = _T ( "" ), wxString extension = _T ( "" ) );
    ~WrapTempFileName();
    wxString wideName();
    std::string name();
    wxString originalWideName();
    std::string originalName();
    void setKeepFiles ( bool b );
private:
    bool keepFiles;
    wxString myTempFileName;
    wxString myTempFileNameOriginal;
};

#endif
