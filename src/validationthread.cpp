#include "wx/wx.h"
#include "xmlctrl.h"
#include "validationthread.h"
#include "wrapxerces.h"
#include <stdexcept>
#include <memory>


wxDEFINE_EVENT(wxEVT_COMMAND_VALIDATION_COMPLETED, wxThreadEvent);

ValidationThread::ValidationThread (
	wxEvtHandler *handler,
	const char *buffer,
	const char *system,
	const char *catalogPath,
	const char *catalogUtilityPath )
	: wxThread ( wxTHREAD_JOINABLE )
{
	if ( buffer == NULL )
	{
		throw;
	}

	myEventHandler = handler;
	myBuffer = buffer;
	mySystem = system;
	myCatalogPath = catalogPath;
	myCatalogUtilityPath = catalogUtilityPath;
	myIsSucceeded = false;
}

void *ValidationThread::Entry()
{
	std::auto_ptr<WrapXerces> validator ( new WrapXerces(
                              myCatalogPath,
                              myCatalogUtilityPath ) );
	
	{
		//wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );
		if ( TestDestroy()  )
		{
			return NULL;
		}
	}

	myIsSucceeded = validator->validateMemory (
		myBuffer.c_str(),
		mySystem.c_str(),
		myBuffer.size() );

	if ( TestDestroy() )
	{
		return NULL;
	}

	extern wxCriticalSection xmlcopyeditorCriticalSection;
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

	wxEvent *event = new wxThreadEvent(wxEVT_THREAD, wxEVT_COMMAND_VALIDATION_COMPLETED);
	wxQueueEvent ( myEventHandler, event );

	return NULL;
}
