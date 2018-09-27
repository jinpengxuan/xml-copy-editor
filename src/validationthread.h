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

#ifndef VALIDATION_THREAD_H
#define VALIDATION_THREAD_H

#include <wx/wx.h>
#include <utility>
#include <string>
#include <wx/thread.h>

DECLARE_EVENT_TYPE(wxEVT_COMMAND_VALIDATION_COMPLETED, wxID_ANY);

class ValidationThread : public wxThread
{
public:
	ValidationThread (
	                 wxEvtHandler *handler,
	                 const char *utf8Buffer,
	                 const wxString &system );
	virtual void *Entry();
	void setBuffer ( const char *buffer, const char *system );
	bool isSucceeded () { return myIsSucceeded; }
	const std::pair<int, int> &getPosition() { return myPosition; }
	const wxString &getMessage() { return myMessage; }

	void PendingDelete();
	// Since we can't call wxThread::m_internal->Cancel(), the original
	// TestDestroy() is useless. Here is the work around.
	virtual void Cancel() { mStopping = true; }
	virtual bool TestDestroy() { return mStopping || wxThread::TestDestroy(); }

protected:
	wxEvtHandler *myEventHandler;
	std::string myBuffer;
	wxString mySystem;
	bool myIsSucceeded;
	std::pair<int, int> myPosition;
	wxString myMessage;

	bool mStopping;
};

#endif
