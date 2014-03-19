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
	clear();
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
			++itr;
		else
			itr = mList.erase ( itr );
	}
}

void ThreadReaper::clear()
{
	xmlcopyeditorCriticalSection.Enter();

	std::vector<boost::shared_ptr<wxThread> > threads = mList;
	mList.clear();

	xmlcopyeditorCriticalSection.Leave();

	// It's safe to call wxThread::Wait() now
	std::vector<boost::shared_ptr<wxThread> >::iterator itr;
	for ( itr = threads.begin(); itr != threads.end(); ++itr)
	{
		// This will cause the whole program to abort in linux with early
		// versions of wxWidgets. A easy way to fix this is to rethrow
		// abi::__forced_unwind& exceptions and avoid calling pthread_exit
		// in such a condition.
#if defined(__WXGTK__) && !wxCHECK_VERSION(2,9,5)
		wxPrintf ( _T ( "Expecting crash..." ) );
#endif
		(**itr).Kill();
		(**itr).Wait();
	}
}
