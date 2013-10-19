#ifndef CATALOG_RESOLVER_THREAD_H
#define CATALOG_RESOLVER_THREAD_H

#include <utility>
#include <string>
#include <wx/thread.h>

class CatalogResolverThread : public wxThread
{
public:
	CatalogResolverThread ( std::string *publicId, std::string *systemId, bool *finished, const std::string& catalogPath );
	virtual void *Entry();
	virtual void OnExit();
private:
	std::string *myPublicIdPtr, *mySystemIdPtr;
	std::string myCatalogPath;
	bool *myFinishedPtr;
};

#endif
