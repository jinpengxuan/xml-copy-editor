/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#ifndef CATALOG_RESOLVER_H
#define CATALOG_RESOLVER_H

#include <string>
#include "wraplibxml.h"

class CatalogResolver : protected WrapLibxml
{
	public:
		CatalogResolver();
		~CatalogResolver();
		wxString catalogResolve
		    ( const wxString &pubIdUtf8
		    , const wxString &sysIdUtf8
		    );
};

#endif
