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

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <cstring>
#include "replace.h"
#include "wrapregex.h"
#include "mp3album.h"

Mp3Album::Mp3Album() : d ( new Mp3AlbumData() )
{
	XML_SetUserData ( p, d.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetCharacterDataHandler ( p, characterdata );
	
	d->trackNo = 0;
	d->armed = false;
}

Mp3Album::~Mp3Album()
{}

void XMLCALL Mp3Album::start ( void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
	Mp3AlbumData *ad;
	ad = ( Mp3AlbumData * ) data;

    char noArray[5];
    int currentNo;

    for ( ; *attr; attr += 2 )
    {

        if (
            !strcmp ( *attr, "smilref" ) &&
            !strncmp ( * ( attr + 1 ), "speechgen", 9 ) )
        {
            memcpy ( noArray, ( * ( attr + 1 ) ) + 9, 4 );
            noArray[4] = '\0';
            currentNo = atoi ( noArray );
            if ( currentNo > ad->trackNo )
            {
                ad->trackNoString = noArray;
                ad->trackNo = currentNo;
                ad->armed = true;
            }            
        }
    }
}

void XMLCALL Mp3Album::end ( void *data, const XML_Char *el )
{
	Mp3AlbumData *ad;
	ad = ( Mp3AlbumData * ) data;
	
    if ( ad->armed )
    {
        ad->armed = false;
        
        bool abbrev = false;
        if ( ad->buffer.size() > 22 )
        {
            ad->buffer.erase ( 18 );
            abbrev = true;  
        }
        
        int replacements;
        
        // no trailing whitespace
        std::string pattern = "\\s+$";
        WrapRegex re ( pattern, false );
        ad->buffer = re.replaceGlobal ( ad->buffer, &replacements );

        // no contiguous whitespace
        std::string pattern2 = "\\s\\s+";
        std::string replace2 = " ";
        WrapRegex re2 ( pattern2, false, replace2 );
        ad->buffer = re.replaceGlobal ( ad->buffer, &replacements );

        // alphanumeric only
        std::string pattern3 = "[;\\.\\[\\],!^&*()]";
        WrapRegex re3 ( pattern3, false );
        ad->buffer = re3.replaceGlobal ( ad->buffer, &replacements );

        if ( abbrev )
            ad->buffer.append ( "..." );

        if ( ad->trackNo == 1 )
        {
            ad->albumTitle = ad->buffer;
            if ( ad->albumTitle.empty() )
                ad->albumTitle = "album";
        }

        if ( abbrev )
            ad->buffer.append ( " " );

        std::string source;
        source.append ( "speechgen" );
        source.append ( ad->trackNoString );
        source.append ( ".mp3" );
        
        std::stringstream destinationStream;
        
    	destinationStream << ad->trackNo << " - " << ad->buffer.c_str();
    	ad->buffer.clear();

        ad->fileNameVector.push_back ( std::make_pair ( source, destinationStream.str()) );   
    }
    if ( !ad->armed || ad->buffer.empty() )
    {
	   ad->armed = false;
       return;
    }
}

void XMLCALL Mp3Album::characterdata (
    void *data,
    const XML_Char *s,
    int len )
{
	Mp3AlbumData *ad;
	ad = ( Mp3AlbumData * ) data;

	if ( ad->armed )
        ad->buffer.append ( s, len );
}

void Mp3Album::getFileNameVector ( std::vector<std::pair<std::string, std::string> >& v )
{
    v = d->fileNameVector;   
}

std::string Mp3Album::getAlbumTitle()
{
		return d->albumTitle;
}
