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

#include <sstream>
#include "readfile.h"

bool ReadFile::run ( std::string fname, std::string &buffer )
{
	std::ifstream ifs ( fname.c_str(), std::ios::binary|std::ios::in );
	if ( !ifs.is_open() )
		return false;

	ifs.seekg ( 0, std::ios::end );
	size_t size = ifs.tellg();
	ifs.seekg ( 0, std::ios::beg );

	buffer.reserve ( size + 1 );

	std::stringstream iss;
	iss << ifs.rdbuf();
	buffer = iss.str();

	return true;
}
