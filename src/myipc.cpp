#include "myipc.h"
#include "xmlcopyeditor.h"

MyServerConnection *server_connection = 0;
MyClientConnection *client_connection = 0;

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    if ( topic == IPC_TOPIC )
        return new MyServerConnection();

    // unknown topic
    return NULL;
}

MyServerConnection::MyServerConnection() : wxConnection()
{
  server_connection = this;
}

MyServerConnection::~MyServerConnection()
{
  if (server_connection)
  {
    server_connection = NULL;
  }
}

bool MyServerConnection::OnPoke(
  const wxString& WXUNUSED(topic),
  const wxString& item,
  wxChar *data,
  int size,
  wxIPCFormat WXUNUSED(format))
{
  if (!wxTheApp)
    return false;
  MyFrame *frame;
  frame = (MyFrame *)wxTheApp->GetTopWindow();
  if (!frame)
    return false;
  if (item == (wxString)IPC_NO_FILE) { ; }
  else if (frame->isOpen(item)) { frame->activateTab(item); }
  else { frame->openFile((wxString&)item); }
  frame->Raise();
  return true;
}

bool MyServerConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
  const wxString& WXUNUSED(item))
{
    return true;
}

MyClientConnection::MyClientConnection()
{

}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyClientConnection;
}

bool MyClientConnection::OnAdvise(const wxString& WXUNUSED(topic), const wxString& WXUNUSED(item), wxChar *data, int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
  return true;
}

bool MyClientConnection::OnDisconnect()
{
  client_connection = NULL;
  return wxConnection::OnDisconnect();
}

MyServer::MyServer()
{ }

MyClient::MyClient()
{ }
