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

#include "wx/wx.h"
#include "xercescatalogresolver.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#ifdef __WXMSW__
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
