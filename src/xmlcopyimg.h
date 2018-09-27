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

#ifndef XML_COPYIMG_H
#define XML_COPYIMG_H

#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct ImgData : public ParserData
{
    wxString path, blankImage, imagesDir, mediaDir;
    std::string buffer, encoding;
    std::vector<std::string> tempFiles;
};

class XmlCopyImg : public WrapExpat
{
	public:
		XmlCopyImg (
		    const wxString& blankImage,
		    const wxString& imagesDir,
		    const wxString& mediaDir,
		    const wxString& path,
		    bool parseDeclaration = false,
		    bool expandInternalEntities = true,
		    size_t size = BUFSIZ
		);
		virtual ~XmlCopyImg();
		std::string getBuffer()
		{
			return d->buffer;
		}
		std::string getEncoding()
		{
			return d->encoding;
		}
	private:
		std::auto_ptr<ImgData> d;
		std::string encoding, element, attribute;
		static void XMLCALL xmldeclhandler (
		    void *data,
		    const XML_Char *version,
		    const XML_Char *encoding,
		    int standalone );
		static void XMLCALL defaulthandler (
		    void *data,
		    const XML_Char *s,
		    int len );
		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
};

#endif
