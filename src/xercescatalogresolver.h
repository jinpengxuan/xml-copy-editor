/*
 * Copyright 2005-2009 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
