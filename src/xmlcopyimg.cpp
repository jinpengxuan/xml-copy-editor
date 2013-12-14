#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <cstring>
#include <wx/wx.h>
#include <wx/filename.h>
#include "xmlcopyimg.h"
#include "xmlcopyeditorcopy.h"

XmlCopyImg::XmlCopyImg (
    const wxString& blankImage,
    const wxString& imagesDir,
    const wxString& mediaDir,
    const wxString& path,
    bool parseDeclaration,
    bool expandInternalEntities,
    size_t size ) :
		d ( new ImgData() )
{
    wxCopyFile (
        blankImage,
        imagesDir + wxFileName::GetPathSeparator() + _T("blank.jpg"),
        true );
    
	d->buffer.reserve ( size );
	d->blankImage = blankImage;
	d->imagesDir = imagesDir;
	d->mediaDir = mediaDir;

    // use only dir component of path
    wxString volumeComponent, pathComponent;
    wxFileName::SplitPath ( path, &volumeComponent, &pathComponent, NULL, NULL );
    
    d->path = volumeComponent + _T(":") + pathComponent;

	XML_SetUserData ( p, d.get() );

	// parse declaration?
	if ( parseDeclaration )
		XML_SetXmlDeclHandler ( p, xmldeclhandler );

	// internal entities
	if ( expandInternalEntities )
		XML_SetDefaultHandlerExpand ( p, defaulthandler );
	else
		XML_SetDefaultHandler ( p, defaulthandler );

	XML_SetElementHandler ( p, start, end );
}

XmlCopyImg::~XmlCopyImg()
{}

void XMLCALL XmlCopyImg::xmldeclhandler (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	ImgData *d;
	d = ( ImgData * ) data;

	d->encoding = ( encoding ) ? encoding : "UTF-8";

	d->buffer.append ( "<?xml version=\"" );
	d->buffer.append ( version );
	d->buffer.append ( "\" encoding=\"" );
	d->buffer.append ( d->encoding );
	d->buffer.append ( "\"" );

	if ( standalone != -1 )
	{
		d->buffer.append ( " standalone=\"" );
		d->buffer.append ( ( standalone == 1 ) ? "yes" : "no" );
		d->buffer.append ( "\"" );
	}
	d->buffer.append ( "?>" );
}

void XMLCALL XmlCopyImg::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	ImgData *d;
	d = ( ImgData * ) data;
	d->buffer.append ( s, len );
}

void XMLCALL XmlCopyImg::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	ImgData *pd;
	pd = ( ImgData * ) data;
    
    std::string tag, value;
    tag += "<";
    tag += el;
    
    wxString wideFile, wideDestination;
    for ( ; *attr; attr += 2 )
    {
        value = ( const char * ) *(attr + 1);
		if ( !strcmp ( el, "img" ) && !strcmp (*attr, "src") )
		{
            wideFile = wxString ( value.c_str(), wxConvUTF8, value.size() );
            wxFileName fn ( wideFile );
            if ( fn.IsRelative() )
            {
                fn.MakeAbsolute ( pd->path );
                wideFile = fn.GetFullPath();
            }

            if ( !wxFileExists ( wideFile ) )
            {
                wideDestination = pd->blankImage;
                value = "images/blank.jpg";
            }
            else
            {
                value = "images/";
                value += fn.GetFullName().mb_str( wxConvUTF8 );
                wideDestination = pd->imagesDir + 
                    wxFileName::GetPathSeparator() + 
                    fn.GetFullName();
                wxCopyFile ( wideFile, wideDestination, true );
            }

            if (
                wideDestination.Contains ( _T(".eps" ) ) ||
                wideDestination.Contains ( _T(".tif") ) ||
                wideDestination.Contains ( _T(".bmp") ) )
            {
                wxString cmd;
                cmd += IMAGEMAGICK_CONVERT_PATH;
                cmd += _T(" -resize 720x720>");
                cmd += _T(" \"");
                cmd += wideFile;
                cmd += _T("\" \"");
                cmd += wideDestination;
                cmd += _T(".png\"");
                
                if ( wxExecute ( cmd, wxEXEC_SYNC ) )
                {
                    wxString error;
                    error.Printf ( _T("Please ensure that ImageMagick is installed on your system and that the conversion tool is located at:\n\n%s\n\nYou can download ImageMagick from:\n\n%s\n\nIf your document contains encapsulated PostScript (EPS) graphics, you need to install GhostScript as well.\n\nYou can download GhostScript from:\n\n%s"),
                        IMAGEMAGICK_CONVERT_PATH,
                        IMAGEMAGICK_INSTALL_URL,
                        GHOSTSCRIPT_INSTALL_URL );
                    wxMessageBox ( error, _T("ImageMagick conversion failed") );
                }
                value += ".png";
            }
            wideDestination.clear();
        } // img + src
        else if ( !strcmp ( el, "a" ) &&
            !strcmp (*attr, "href") &&
            ( strstr ( value.c_str(), ".mp3" ) || strstr ( value.c_str(), ".pdf" ) ) )
        {
            wideFile = wxString ( value.c_str(), wxConvUTF8, value.size() );
            wxFileName fn ( wideFile );
            if ( fn.IsRelative() )
            {
                fn.MakeAbsolute ( pd->path );
                wideFile = fn.GetFullPath();
            }
            if ( !wxFileExists ( wideFile ) )
            {
                value = "images/blank.jpg";   
            }
            else
            {
                value = "media/";
                value += fn.GetFullName().mb_str( wxConvUTF8 );
                wideDestination = pd->mediaDir + 
                    wxFileName::GetPathSeparator() + 
                    fn.GetFullName();
                wxCopyFile ( wideFile, wideDestination, true );
            }
        } // a + href
        
        tag += " ";
        tag += *attr;
        tag += "=\"";
        tag += value.c_str();
        tag += "\"";
    }
    
    tag += ">";
    
    pd->buffer += tag;
}

void XMLCALL XmlCopyImg::end ( void *data, const XML_Char *el )
{
	ImgData *pd;
	pd = ( ImgData * ) data;
    
    pd->buffer += "</";
    pd->buffer += el;
    pd->buffer += ">";
}
