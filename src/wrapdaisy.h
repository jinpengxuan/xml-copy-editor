#ifndef WRAPDAISY_H
#define WRAPDAISY_H

#include <wx/wx.h>

class WrapDaisy
{
public:
    WrapDaisy ( const wxString& daisyDir );
    ~WrapDaisy();
    bool run (
        wxString& fileIn,
        wxString& stylesheet,
        wxString& folder,
        bool quiet,
	bool suppressOptional,
	bool epub,
	bool rtf,
	bool fullDaisy,
        bool mp3Album );
    wxString getLastError();
private:
    wxString daisyDir, classPath, commandLineUI, baseCmd, error,
        memoryCwd, daisyCwd, albumCover;
};

#endif
