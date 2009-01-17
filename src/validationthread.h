#ifndef VALIDATION_THREAD_H
#define VALIDATION_THREAD_H

#include <utility>
#include <string>
#include <wx/thread.h>

class ValidationThread : public wxThread
{
public:
	ValidationThread (
                     const char *buffer,
                     const char *system,
                     const char *catalogPath,
                     const char *catalogUtilityPath,
                     bool *finished,
                     bool *success,
                     bool *release, std::pair<int, int> *position,
                     std::string *message );
	virtual void *Entry();
	virtual void OnExit();
private:
	std::string myBuffer, mySystem, myCatalogPath, myCatalogUtilityPath;
	bool *myFinishedPtr, *mySuccessPtr, *myReleasePtr;
	std::pair<int, int> *myPositionPtr;
	std::string *myMessagePtr;
};

#endif
