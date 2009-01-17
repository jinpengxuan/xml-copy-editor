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
    const std::string& catalogPath,
    const std::string& catalogUtilityPath,
    const std::string& basePath,
    const std::string& auxPath,
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
		    catalogPath,
		    catalogUtilityPath,
		    basePath,
		    auxPath,
		    position,
		    size,
		    style )
{ }

wxString& XmlDoc::getDirectory()
{
	return directory;
}
wxString& XmlDoc::getFullFileName()
{
	return fullFileName;
}
wxString& XmlDoc::getShortFileName()
{
	return shortFileName;
}
wxDateTime XmlDoc::getLastModified()
{
	return lastModified;
}

void XmlDoc::setDirectory ( const wxString& s )
{
	directory = s;
}

void XmlDoc::setFullFileName ( const wxString& s )
{
	fullFileName = s;
}
void XmlDoc::setShortFileName ( const wxString& s )
{
	shortFileName = s;
}
void XmlDoc::setLastModified ( wxDateTime dt )
{
	lastModified = dt;
}

