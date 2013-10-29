#include "wx/wx.h"
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
	const char *buffer,
	const wxString &system )
	: wxThread ( wxTHREAD_JOINABLE )
	, mStopping ( false )
{
	if ( buffer == NULL )
	{
		throw;
	}

	myEventHandler = handler;
	myBuffer = buffer;
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
		this );

	myBuffer.clear();

	if ( TestDestroy() )
	{
		return NULL;
	}

	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( myIsSucceeded )
	{
		myPosition = std::make_pair ( 0, 0 );
		myMessage = wxEmptyString;
	}
	else
	{
		myPosition = validator->getErrorPosition();
		myMessage = validator->getLastError();
	}

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
