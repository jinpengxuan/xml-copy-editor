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
#include "xmldoc.h"

XmlDoc::XmlDoc (
    wxWindow *parent,
    XmlCtrlProperties properties,
    bool *protectTags,
    int visibilityState,
    int type,
    wxWindowID id,
    //const std::string& buffer,
    const char *buffer,
    size_t bufferLen,
    const wxString& basePath,
    const wxString& auxPath,
    const wxPoint &position,
    const wxSize& size,
    long style )
		: XmlCtrl (
		    parent,
		    properties,
		    protectTags,
		    visibilityState,
		    type,
		    id,
		    buffer,
		    bufferLen, // new
		    basePath,
		    auxPath,
		    position,
		    size,
		    style )
{ }

wxString XmlDoc::getDirectory()
{
	return mFileName.GetPath();
}

wxString XmlDoc::getFullFileName()
{
	return mFileName.GetFullPath();
}

wxString XmlDoc::getShortFileName()
{
	return mFileName.GetFullName();
}

const wxDateTime& XmlDoc::getLastModified()
{
	return lastModified;
}

void XmlDoc::setFullFileName ( const wxString &s )
{
	mFileName.Assign ( s );
	mFileName.Normalize();
}

void XmlDoc::setShortFileName ( const wxString &s )
{
	mFileName.SetFullName ( s );
}

void XmlDoc::setLastModified ( const wxDateTime& dt )
{
	lastModified = dt;
}

