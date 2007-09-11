/* 
 * Copyright 2005-2007 Gerald Schmidt.
 * 
 * This file is part of Xml Copy Editor.
 * 
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include "myipc.h"
#include "xmlcopyeditor.h"

MyServerConnection *server_connection = 0;
MyClientConnection *client_connection = 0;

wxConnectionBase *MyServer::OnAcceptConnection ( const wxString& topic )
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
    if ( server_connection )
    {
        server_connection = NULL;
    }
}

bool MyServerConnection::OnPoke (
    const wxString& WXUNUSED ( topic ),
    const wxString& item,
    wxChar *data,
    int size,
    wxIPCFormat WXUNUSED ( format ) )
{
    if ( !wxTheApp )
        return false;
    MyFrame *frame;
    frame = ( MyFrame * ) wxTheApp->GetTopWindow();
    if ( !frame )
        return false;
    if ( item == ( wxString ) IPC_NO_FILE )
    {
        ;
    }
    else if ( frame->isOpen ( item ) )
    {
        frame->activateTab ( item );
    }
    else
    {
        frame->openFile ( ( wxString& ) item );
    }
    frame->Raise();
    return true;
}

bool MyServerConnection::OnStartAdvise ( const wxString& WXUNUSED ( topic ),
        const wxString& WXUNUSED ( item ) )
{
    return true;
}

MyClientConnection::MyClientConnection()
{}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyClientConnection;
}

bool MyClientConnection::OnAdvise ( const wxString& WXUNUSED ( topic ), const wxString& WXUNUSED ( item ), wxChar *data, int WXUNUSED ( size ), wxIPCFormat WXUNUSED ( format ) )
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
