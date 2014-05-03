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

#ifndef XML_DOC_H
#define XML_DOC_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/print.h>
#include <wx/filename.h>
#include "xmlctrl.h"

class XmlDoc : public XmlCtrl
{
	public:
		XmlDoc (
		    wxWindow *parent,
		    XmlCtrlProperties properties,
		    bool *protectTags,
		    int visibilityState = SHOW_TAGS,//bool hideAttributes,
		    int type = FILE_TYPE_XML,
		    wxWindowID id = wxID_ANY,
		    //const std::string& buffer = DEFAULT_XML_DECLARATION_UTF8,
		    const char *buffer = NULL,
		    size_t bufferLen = 0,
		    const wxString& basePath = wxEmptyString,
		    const wxString& auxPath = wxEmptyString,
		    const wxPoint& position = wxDefaultPosition,
		    const wxSize& size = wxDefaultSize,
		    long style = 0 );
		wxString getDirectory();
		wxString getFullFileName();
		wxString getShortFileName();
		const wxDateTime& getLastModified();
		void setFullFileName ( const wxString &s );
		void setShortFileName ( const wxString &s );
		void setLastModified ( const wxDateTime &dt );
	private:
		wxFileName mFileName;
		wxDateTime lastModified;
};

#endif

