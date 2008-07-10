#include "wx/wx.h"
#include "validationthread.h"
#include "wrapxerces.h"
#include <stdexcept>
#include <memory>

ValidationThread::ValidationThread (
	const char *buffer,
	const char *system,
	bool *finished,
	bool *success,
	bool *release,
	std::pair<int, int> *position,
	std::string *message ) : wxThread()
{
	if (!buffer || !success || !position || !message )
	{
		throw;
	}

	myBuffer = buffer;
	mySystem = system;
	myFinishedPtr = finished;
	mySuccessPtr = success;
	myReleasePtr = release;
	myPositionPtr = position;
	myMessagePtr = message;
}

void *ValidationThread::Entry()
{
	std::auto_ptr<WrapXerces> validator ( new WrapXerces() );
	
		if ( *myReleasePtr || TestDestroy()  )
		{
			Exit();
			return NULL;
		}
	
	bool res = validator->validateMemory (
		myBuffer.c_str(),
		mySystem.c_str(),
		myBuffer.size() );
	
		if ( *myReleasePtr  || TestDestroy() )
		{
			Exit();
			return NULL;
		}
	
		if ( !res )
		{
			*mySuccessPtr = res;
			*myPositionPtr = validator->getErrorPosition();
			*myMessagePtr = validator->getLastError();		
		}
		else
		{
			*mySuccessPtr = true;
			*myPositionPtr = std::make_pair ( 0, 0 );
			*myMessagePtr = "";
		}
	return NULL;
}

void ValidationThread::OnExit()
{
	if ( *myReleasePtr )
		return;

	*myFinishedPtr = true;
}
