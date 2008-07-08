#ifndef VALIDATION_THREAD_H
#define VALIDATION_THREAD_H

#include <utility>
#include <string>
#include <wx/thread.h>

class ValidationThread : public wxThread
{
public:
	ValidationThread ( const char *buffer, const char *system, bool *finished, bool *success, std::pair<int, int> *position, std::string *message );
	virtual void *Entry();
	virtual void OnExit();
private:
	std::string myBuffer, mySystem;
	bool *myFinishedPtr, *mySuccessPtr;
	std::pair<int, int> *myPositionPtr;
	std::string *myMessagePtr;
};

#endif
