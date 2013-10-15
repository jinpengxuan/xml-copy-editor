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
	const wxString& WXUNUSED ( topic )
	, const wxString& item
#if wxCHECK_VERSION(2,9,0)
	, const void *data
	, size_t size
#else
	, wxChar *data
	, int size
#endif
	, wxIPCFormat WXUNUSED ( format )
	)
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
		if ( frame->activateTab ( item ) )
			frame->reloadTab();
	}
	else
	{
		frame->openFile ( ( wxString& ) item );
		//frame->addToFileQueue ( ( wxString& ) item ); // prevent event loop problems
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

bool MyClientConnection::OnAdvise (
	const wxString& WXUNUSED ( topic )
	, const wxString& WXUNUSED ( item )
#if wxCHECK_VERSION(2,9,0)
	, const void * WXUNUSED ( data )
	, size_t WXUNUSED ( size )
#else
	, wxChar * WXUNUSED ( data )
	, int WXUNUSED ( size )
#endif
	, wxIPCFormat WXUNUSED ( format )
	)
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
