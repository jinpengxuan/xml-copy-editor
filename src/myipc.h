#ifndef MY_IPC_H
#define MY_IPC_H

#include <wx/wx.h>
#include <wx/ipc.h>

#define IPC_SERVICE _T("4242")
#define IPC_TOPIC _T("IPC TEST")
#define IPC_ADVISE_NAME _T("Item")
#define IPC_NO_FILE _T("[nofile]")

class MyServerConnection;
class MyClientConnection;
extern MyServerConnection *server_connection;
extern MyClientConnection *client_connection;

class MyServerConnection : public wxConnection
{
  public:
    MyServerConnection();
    ~MyServerConnection();
    bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    bool OnStartAdvise(const wxString& topic, const wxString& item);
};

class MyClientConnection: public wxConnection
{
  public:
    MyClientConnection();
    bool OnAdvise(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    bool OnDisconnect();
};

class MyClient: public wxClient
{
  public:
    MyClient();
    wxConnectionBase *OnMakeConnection();
};

class MyServer: public wxServer
{
  public:
    MyServer();
    wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

#endif
