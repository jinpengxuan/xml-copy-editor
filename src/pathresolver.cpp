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

#include <wx/filename.h>
#include "pathresolver.h"
#include <wx/stdpaths.h>

wxString PathResolver::run ( const wxString& path, const wxString& anchor )
{
	if ( path.empty() ) // no hope for empty paths
		return wxEmptyString;
	
	wxFileName pathObject ( path );
	if ( pathObject.IsAbsolute() || path.Contains ( _T ( "http://" ) ) )
		return path;

	// check anchor
	wxString myAnchor;
	myAnchor = anchor;
	wxFileName anchorObject ( myAnchor );
	if ( myAnchor.empty() )
	{
		myAnchor = wxFileName::GetCwd();
	}
	else if ( !anchorObject.IsDir() )
	{
		myAnchor = anchorObject.GetPath();
	}

	pathObject.MakeAbsolute ( myAnchor );

	return pathObject.GetFullPath();
}

std::string PathResolver::run (
    const std::string& path,
    const std::string& anchor )
{
	wxString widePath, wideAnchor;
	widePath = wxString ( path.c_str(), wxConvUTF8, path.size() );
	wideAnchor = wxString ( anchor.c_str(), wxConvUTF8, anchor.size() );
	wxString wideReturn = PathResolver::run ( widePath, wideAnchor );
	return ( const char * ) wideReturn.mb_str ( wxConvUTF8 );
}
