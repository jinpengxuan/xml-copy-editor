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

#include <wx/wx.h>
#include <wx/strconv.h>
#include <wx/buffer.h>
#include <wx/log.h>
#include <string>
#include <utility>
#include <boost/utility.hpp>

#if !wxCHECK_GCC_VERSION(4,7)
#define XERCES_TMPLSINC
#endif

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "xercescatalogresolver.h"

using namespace xercesc;

class MySAX2Handler : public DefaultHandler
{
	public:
		MySAX2Handler()
		{
			mEOL = _T("\n");
			reset();
		}
		void error ( const SAXParseException& e )
		{
			logError ( _("Error"), wxLOG_Error, e );
			throw e;
		}
		void warning ( const SAXParseException& e )
		{
			logError ( _("Warning"), wxLOG_Warning, e );
		}
		void fatalError ( const SAXParseException& e )
		{
			logError ( _("FatalError"), wxLOG_FatalError, e );
			throw e;
		}
		void reset()
		{
			mErrors.clear();
			mErrorPosition = std::make_pair ( 1, 1 );
			mLevel = wxLOG_Max;
		}
		const wxString &getErrors() const
		{
			return mErrors;
		}
		wxString &getErrors()
		{
			return mErrors;
		}
		const std::pair<int, int> &getErrorPosition() const
		{
			return mErrorPosition;
		}
		void logError ( const wxString &type, wxLogLevel level,
				const SAXParseException& e );
		void setEOL ( const wxChar *eol )
		{
			mEOL = eol;
		}
	protected:
		wxString mErrors;
		std::pair<int, int> mErrorPosition;
		wxLogLevel mLevel;
		wxString mEOL;
};

class WrapXerces : private boost::noncopyable
{
	public:
		static void Init ( bool enableNetAccess ) throw ();

		WrapXerces();
		virtual ~WrapXerces();
		// Returns true if the file is valid. But there can be warnings
		bool validate ( const wxString &fileName );
		// Returns true if the content is valid. But there can be warnings
		bool validateMemory ( const char *utf8Buffer, size_t len,
		    const wxString &fileName, wxThread *thread = NULL,
		    bool forceGrammarCheck = true, /* Must specify a grammar */
		    const wxChar *messageEOL = _T("[br]") );
		const wxString &getLastError()
		{
			return mySAX2Handler.getErrors();
		}
		const std::pair<int, int> &getErrorPosition()
		{
			return mySAX2Handler.getErrorPosition();
		}

		static wxString toString ( const XMLCh *str );
		// Convert Unicode string to const XMLCh *
//#if wxCHECK_VERSION(2,9,0)
//		static wxCharTypeBuffer<XMLCh> toString ( const wxString &str );
//#else
		static wxMemoryBuffer toString ( const wxString &str );
//#endif
		// Returns original value
		static bool enableNetwork ( bool enable = true );

		static xercesc::InputSource *resolveEntity (
			const wxString &publicId,
			const wxString &systemId,
			const wxString &fileName );
	private:
		static const wxMBConv &getMBConv();
		static XMLNetAccessor *mOriginalNetAccessor;

		XercesCatalogResolver *catalogResolver;
		MySAX2Handler mySAX2Handler;
};

#endif
