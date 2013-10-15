#include "xercescatalogresolver.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#ifdef __WXMSW__
#include "wx/wx.h"
#include "replace.h"
#endif

InputSource *XercesCatalogResolver::resolveEntity (
			const XMLCh* const publicID,
			const XMLCh* const systemId )
{
/* the following _should_ work but doesn't always, so drop it for now
#ifndef __WXMSW__
	resolved = lookupPublicId ( narrowPublicId );
#else
     return NULL;
	std::string stdPublicId = narrowPublicId;

    // on Windows, call libxml's own xmlcatalog binary
    // because calling libxml from xerces causes a protection fault

	std::string narrowCommand = "";
    narrowCommand += myCatalogUtilityPath;
	narrowCommand += " \"";
	narrowCommand += myCatalogPath;
	narrowCommand += "\" \"";
	narrowCommand += narrowPublicId;
	narrowCommand += "\"";
	
	wxString wideCommand = wxString (
             narrowCommand.c_str(),
             wxConvUTF8,
             narrowCommand.size() );
             
	wxArrayString stringArray;
    long ret = wxExecute (
         wideCommand,
         stringArray,
         wxEXEC_SYNC | wxEXEC_NODISABLE );
	if ( ret == -1 || stringArray.empty() )
	{
        return NULL;
	}
	wxString returnValue = stringArray[0];

    std::string narrowReturnValue = (const char *)returnValue.mb_str ( wxConvLocal );
    
    Replace::run ( narrowReturnValue, "%20", " ", false );
    
    char *s, *it;
    s = (char *) narrowReturnValue.c_str();
    
    for (char *scan = s; *scan; scan++ )
        if (*scan == '/')
           *scan = '\\';
    
    if ( strstr ( s, "No entry" ) )
    {
       return NULL;
    }
	   
    it = strstr ( s, "\\\\\\" );
    if ( it )
       resolved = it + 3;
    else
        resolved = (const char *)s;//narrowReturnValue;
#endif	
*/

	if ( publicID == NULL || *publicID == '\0' )
		return NULL;

	char *narrowPublicId = XMLString::transcode ( publicID );
	if ( narrowPublicId == NULL )
		return NULL;

	std::string resolved;
	resolved = lookupPublicId ( narrowPublicId );

	XMLString::release ( &narrowPublicId );

	if ( resolved.empty() )
		return NULL;

	XMLCh *wideResolved = XMLString::transcode ( resolved.c_str() );

	InputSource *source = (InputSource *)new LocalFileInputSource ( wideResolved );
	XMLString::release ( &wideResolved );

	return source;
}
