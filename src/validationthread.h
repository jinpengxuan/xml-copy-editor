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
