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

#ifndef MP3_ALBUM_H
#define MP3_ALBUM_H

#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include <utility>
#include "wrapexpat.h"

struct Mp3AlbumData : public ParserData
{
    std::string albumTitle, trackNoString, buffer;
    std::vector<std::pair<std::string, std::string> > fileNameVector;
    int trackNo;
    bool armed;
};

class Mp3Album : public WrapExpat
{
	public:
		Mp3Album();
		virtual ~Mp3Album();
		void getFileNameVector ( std::vector<std::pair<std::string, std::string> >& v );
		std::string getAlbumTitle();
	private:
		std::auto_ptr<Mp3AlbumData> d;
		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
		static void XMLCALL characterdata ( void *data, const XML_Char *s, int len );
};

#endif
