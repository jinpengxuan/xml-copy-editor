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
#include "contexthandler.h"

ContextMatch ContextHandler::getContext (
    const char *s,
    int len,
    const char *origin,
    int context )
{
	const char *t, *u;
	int maxrange = context * 20;

	int range = context + 1, i = 0;
	for ( t = s; t != origin; --t )
		if ( *t == ' ' || *t == '\t' || *t == '\r' || *t == '\n' )
			if ( ++i == range )
			{
				++t;
				break;
			}

	i = 0;
	for ( u = s + len; *u; ++u )
		if ( *u == ' ' || *u == '\t' || *u == '\r' || *u == '\n' )
			if ( ++i == range )
				break;

	ContextMatch match;

	// prevent excessive context
	if ( ( s - t ) < maxrange )
		match.prelog.assign ( t, s - t );
	else
		match.prelog = "...";
	match.match.assign ( s, len );
	if ( ( u - s ) < maxrange )
		match.postlog.assign ( s + len, u - ( s + len ) );
	else
		match.postlog = "...";

	return match;
}
