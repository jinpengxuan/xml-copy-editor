#ifndef XERCES_CATALOG_RESOLVER_H
#define XERCES_CATALOG_RESOLVER_H

#include <memory>
#include <string>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>

#ifndef __WXMSW__
#include "catalogresolver.h"
#endif

using namespace xercesc;

class XercesCatalogResolver : public EntityResolver
{
	public:
		XercesCatalogResolver( std::string catalogPath = "",
                               std::string catalogUtilityPath = "" )
		{
#ifndef __WXMSW__
			cr = new CatalogResolver ( catalogPath );
#else
			myCatalogPath = catalogPath;
			myCatalogUtilityPath = catalogUtilityPath;
#endif
		}
		~XercesCatalogResolver()
		{
#ifndef __WXMSW__
			delete cr;
#endif
		}
		InputSource *resolveEntity (
			const XMLCh * const publicID,
			const XMLCh* const systemId );
	private:
#ifndef __WXMSW__
		CatalogResolver *cr;
#else
		std::string myCatalogPath;
		std::string myCatalogUtilityPath;
#endif
};

#endif
