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

#include <wx/wx.h>
#include <wx/textbuf.h>
#include "xmlctrl.h"
#include "validationthread.h"
#include "wrapxerces.h"
#include <stdexcept>
#include <memory>
#include "threadreaper.h"

extern wxCriticalSection xmlcopyeditorCriticalSection;

DEFINE_EVENT_TYPE(wxEVT_COMMAND_VALIDATION_COMPLETED);

ValidationThread::ValidationThread (
	wxEvtHandler *handler,
	const char *utf8Buffer,
	const wxString &system )
	: wxThread ( wxTHREAD_JOINABLE )
	, mStopping ( false )
{
	if ( utf8Buffer == NULL )
	{
		throw;
	}

	myEventHandler = handler;
	myBuffer = utf8Buffer;
	mySystem = system;
	myIsSucceeded = false;
}

void *ValidationThread::Entry()
{
	std::auto_ptr<WrapXerces> validator ( new WrapXerces() );
	
	if ( TestDestroy()  )
	{
		myBuffer.clear();
		return NULL;
	}

	myIsSucceeded = validator->validateMemory (
		myBuffer.c_str(),
		myBuffer.size(),
		mySystem,
		this,
		// Don't be too harsh to new created documents, which may haven't
		// associated any schema yet
		/*forceCheckGrammar*/false,
		wxTextBuffer::GetEOL() );

	myBuffer.clear();

	if ( TestDestroy() )
	{
		return NULL;
	}

	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	myPosition = validator->getErrorPosition();
	myMessage = validator->getLastError();

	if ( !TestDestroy() )
	{
		wxCommandEvent event ( wxEVT_COMMAND_VALIDATION_COMPLETED );
		wxPostEvent ( myEventHandler, event );
	}

	return NULL;
}

void ValidationThread::PendingDelete ()
{
	Cancel();

	ThreadReaper::get().add ( this );
}
