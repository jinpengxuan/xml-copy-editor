#ifndef VALIDATION_THREAD_H
#define VALIDATION_THREAD_H

#include <wx/wx.h>
#include <utility>
#include <string>
#include <wx/thread.h>

wxDECLARE_EVENT(wxEVT_COMMAND_VALIDATION_COMPLETED, wxThreadEvent);

class ValidationThread : public wxThread
{
public:
	ValidationThread (
	                 wxEvtHandler *handler,
	                 const char *buffer,
	                 const char *system,
	                 const char *catalogPath,
	                 const char *catalogUtilityPath );
	virtual void *Entry();
	void setBuffer ( const char *buffer, const char *system );
	bool isSucceeded () { return myIsSucceeded; }
	const std::pair<int, int> &getPosition() { return myPosition; }
	const wxString &getMessage() { return myMessage; }
private:
	wxEvtHandler *myEventHandler;
	std::string myBuffer, mySystem, myCatalogPath, myCatalogUtilityPath;
	bool myIsSucceeded;
	std::pair<int, int> myPosition;
	wxString myMessage;
};

#endif
