#ifndef XERCES_CATALOG_RESOLVER_H
#define XERCES_CATALOG_RESOLVER_H

#include <memory>
#include <string>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax/Locator.hpp>
#include "catalogresolver.h"

using namespace xercesc;

class XercesCatalogResolver : public EntityResolver, public CatalogResolver
{
	public:
		XercesCatalogResolver()
		{
		}
		~XercesCatalogResolver()
		{
		}
		virtual InputSource *resolveEntity (
			const XMLCh * const publicID,
			const XMLCh* const systemId );
};

#endif
