#include "wx/wx.h"
#include "catalogresolverthread.h"
#include "wraplibxml.h"
#include <memory>

wxCriticalSection catalogResolverCriticalSection;

CatalogResolverThread::CatalogResolverThread (
	std::string *publicId,
	std::string *systemId,
	bool *finished,
	const std::string& catalogPath ) : wxThread()//( wxTHREAD_JOINABLE )
{
	myPublicIdPtr = publicId;
	mySystemIdPtr = systemId;
	myFinishedPtr = finished;
	myCatalogPath = catalogPath;

	*myFinishedPtr = false;
}

void *CatalogResolverThread::Entry()
{
	{
		wxCriticalSectionLocker locker ( catalogResolverCriticalSection );

		std::auto_ptr<WrapLibxml> libxml ( new WrapLibxml ( false, myCatalogPath ) );
		*mySystemIdPtr = libxml->lookupPublicId ( *myPublicIdPtr );
	}
	Exit();
	return NULL;
}

void CatalogResolverThread::OnExit()
{
	{
		wxCriticalSectionLocker locker ( catalogResolverCriticalSection );
		*myFinishedPtr = true;
	}
}
