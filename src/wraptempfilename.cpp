#include <stdexcept>
#include "wraptempfilename.h"

WrapTempFileName::WrapTempFileName ( wxString hint, wxString extension )
{
    keepFiles = false;
    myTempFileName = wxFileName::CreateTempFileName ( hint );
    if ( !extension.empty() )
    {
        myTempFileNameOriginal = myTempFileName;
        myTempFileName.Replace ( _T ( ".tmp" ), extension );
    }

    if ( myTempFileName == hint ||
            myTempFileName.empty() )
    {
        myTempFileName = wxFileName::CreateTempFileName ( wxEmptyString );
        if ( myTempFileName.empty() )
            throw std::runtime_error ( "Cannot create temporary filename" );
    }
}

WrapTempFileName::~WrapTempFileName()
{
    if ( keepFiles )
        return;
    wxRemoveFile ( myTempFileName );
    if ( !myTempFileNameOriginal.empty() )
        wxRemoveFile ( myTempFileNameOriginal );
}

wxString WrapTempFileName::wideName()
{
    return myTempFileName;
}

std::string WrapTempFileName::name()
{
    std::string tmp ( ( const char * ) myTempFileName.mb_str ( wxConvLocal ) );
    return tmp;
}

wxString WrapTempFileName::originalWideName()
{
    return myTempFileNameOriginal;
}

std::string WrapTempFileName::originalName()
{
    std::string tmp ( ( const char* ) myTempFileNameOriginal.mb_str ( wxConvLocal ) );
    return tmp;
}

void WrapTempFileName::setKeepFiles ( bool b )
{
    keepFiles = b;
}
