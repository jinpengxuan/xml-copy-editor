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

#ifndef WRAP_XERCES
#define WRAP_XERCES

#if __GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ < 7 )
#define XERCES_TMPLSINC
#endif

#include <wx/wx.h>
#include <string>
#include <utility>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "xercescatalogresolver.h"

using namespace xercesc;

class WrapXerces
{
	public:
		WrapXerces( std::string catalogPath = "",
                    std::string catalogUtilityPath = "" );
		~WrapXerces();
		bool validate ( const std::string& fileName );
		bool validateMemory ( const char *buffer, const char *system, unsigned len );
		const wxString &getLastError();
		std::pair<int, int> getErrorPosition();
		static wxString toString ( const XMLCh *str );
	private:
		XercesCatalogResolver *catalogResolver;
		wxString lastError;
		std::pair<int, int> errorPosition;
};

class MySAX2Handler : public DefaultHandler
{
	public:
		void error ( const SAXParseException& e )
		{
			throw e;
		}
		void warning ( const SAXParseException& e )
		{
			throw e;
		}
};

#endif
