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
#define XERCES_TMPLSINC

#include <string>
#include <utility>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

using namespace xercesc;

class WrapXerces
{
	public:
		WrapXerces();
		~WrapXerces();
		bool validate ( const std::string& fileName );
		bool validateMemory ( const char *buffer, const char *system, unsigned len );
		std::string getLastError();
		std::pair<int, int> getErrorPosition();
	private:
		std::string lastError;
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
