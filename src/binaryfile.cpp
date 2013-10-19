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

#include "binaryfile.h"
#include <wx/wfstream.h>

BinaryFile::BinaryFile ( const wxString &fname ) : m_data ( 0 ), m_dataLen ( 0 )
{
	wxFileInputStream stream ( fname );
	size_t lSize;
	char *buffer;

	if ( !stream.IsOk() )
	{
		return;
	}

	lSize = stream.GetSize();

	// allocate memory to contain the whole file:

	//buffer = new char[lSize]; // for some reason this is much slower than malloc
	buffer = ( char* ) malloc ( sizeof ( char ) *lSize );
	if ( buffer == NULL )
	{
		return;
	}

	// copy the file into the buffer:
	stream.Read ( buffer, lSize );
	if ( stream.LastRead() != lSize )
	{
		if ( !stream.Eof() )
			return;
	}

	/* the whole file is now loaded in the memory buffer. */

	m_data = buffer;
	m_dataLen = lSize;
}

BinaryFile::~BinaryFile()
{
	//delete[] m_data;
	free ( m_data );
}

const char *BinaryFile::getData()
{
	return ( const char * ) m_data;
}

size_t BinaryFile::getDataLen()
{
	return m_dataLen;
}
