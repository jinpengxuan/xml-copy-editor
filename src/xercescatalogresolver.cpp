#include "xercescatalogresolver.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#ifdef __WXMSW__
#include "wx/wx.h"
#include "replace.h"
#endif

#include "wrapxerces.h"

InputSource *XercesCatalogResolver::resolveEntity (
			const XMLCh* const publicId,
			const XMLCh* const systemId )
{

	wxString pubId, sysId, resolved;
	pubId = WrapXerces::toString ( publicId );
	sysId = WrapXerces::toString ( systemId );
	resolved = catalogResolve ( pubId, sysId );
	if ( !resolved.empty() )
		return new LocalFileInputSource (
			( const XMLCh * ) WrapXerces::toString ( resolved ).GetData() );

	// Xerces-C++ can't open a file URL when there are multi-byte characters.
	// Parse the file URL here instead.
	wxFileName file = WrapLibxml::URLToFileName ( sysId );
	if ( file.IsFileReadable() )
		return new LocalFileInputSource (
			( const XMLCh * ) WrapXerces::toString ( file.GetFullPath() ).GetData() );

	return NULL;
}
