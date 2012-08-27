/*
 * Copyright 2012 Zane U. Ji.
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

#include "threadreaper.h"

extern wxCriticalSection xmlcopyeditorCriticalSection;

ThreadReaper::ThreadReaper ()
{
}

ThreadReaper::~ThreadReaper ()
{
	//wxCriticalSectionLocker lock ( xmlcopyeditorCriticalSection );

	std::vector<boost::shared_ptr<wxThread> >::iterator itr;
	for ( itr = mList.begin(); itr != mList.end(); itr++)
	{
		(**itr).Kill();
	}
}

ThreadReaper &ThreadReaper::get()
{
	static ThreadReaper reaper;
	return reaper;
}

void ThreadReaper::add ( wxThread *thread )
{
	// Make sure everything is valid when wxPostMessage is called
	// and protect mList
	wxCriticalSectionLocker lock ( xmlcopyeditorCriticalSection );

	mList.push_back ( boost::shared_ptr<wxThread> ( thread ) );

	std::vector<boost::shared_ptr<wxThread> >::iterator itr;
	for ( itr = mList.begin(); itr != mList.end(); )
	{
		if ( (**itr).IsAlive() )
			itr++;
		else
			itr = mList.erase ( itr );
	}
}
