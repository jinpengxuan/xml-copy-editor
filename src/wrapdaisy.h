#ifndef WRAPDAISY_H
#define WRAPDAISY_H

#include <wx/wx.h>
#include "xmlcopyeditor.h"

class WrapDaisy
{
public:
    WrapDaisy (
        MyFrame *frame,
        const wxString& daisyDir,
        const wxString& path );
    ~WrapDaisy();
    bool run (
        wxString& fileIn,
        wxString& stylesheet,
        wxString& folder,
        bool quiet,
        bool suppressOptional,
        bool epub,
        bool rtf,
        bool doc,
        bool fullDaisy,
        bool mp3Album );
    const wxString &getLastError();
private:
    MyFrame *frame;
    wxString daisyDir, path, blankImage, classPath, commandLineUI, baseCmd, error,
        memoryCwd, daisyCwd, albumCover;
};

#endif
