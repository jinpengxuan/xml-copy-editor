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

#include <stdexcept>
#include "wraptempfilename.h"

WrapTempFileName::WrapTempFileName ( wxString hint, wxString extension )
{
	keepFiles = false;
	myTempFileName = wxFileName::CreateTempFileName ( hint );
	if ( !extension.empty() )
	{
		myTempFileNameOriginal = myTempFileName;
		myTempFileName.Replace ( _T ( ".tmp" ), extension );
	}

	if ( myTempFileName == hint ||
	        myTempFileName.empty() )
	{
		myTempFileName = wxFileName::CreateTempFileName ( wxEmptyString );
		if ( myTempFileName.empty() )
			throw std::runtime_error ( "Cannot create temporary filename" );
	}
}

WrapTempFileName::~WrapTempFileName()
{
	if ( keepFiles )
		return;
	wxRemoveFile ( myTempFileName );
	if ( !myTempFileNameOriginal.empty() )
		wxRemoveFile ( myTempFileNameOriginal );
}

wxString WrapTempFileName::wideName()
{
	return myTempFileName;
}

std::string WrapTempFileName::name()
{
	std::string tmp ( ( const char * ) myTempFileName.mb_str ( wxConvLocal ) );
	return tmp;
}

wxString WrapTempFileName::originalWideName()
{
	return myTempFileNameOriginal;
}

std::string WrapTempFileName::originalName()
{
	std::string tmp ( ( const char* ) myTempFileNameOriginal.mb_str ( wxConvLocal ) );
	return tmp;
}

void WrapTempFileName::setKeepFiles ( bool b )
{
	keepFiles = b;
}
