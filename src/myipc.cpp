/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
#include "pathresolver.h"

#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

void MyRaiseWindow ( wxTopLevelWindow *wnd )
{
	GtkWidget *widget = wnd->GetHandle();
	if ( !widget )
		return;

	GdkWindow *gwnd = gtk_widget_get_window ( widget );
	if ( !gwnd )
		return;

	// The default time GDK_CURRENT_TIME doesn't work.
	// https://github.com/GNOME/gtk/blob/96b37f4eb8ec0e7598534a83816be7c013fd1c9b/gtk/gtkwindow.c#L7521
	// https://github.com/wxWidgets/wxWidgets/blob/dfc6cdc06355fa46ffc3725079a653324c45b974/src/gtk/toplevel.cpp#L1103
	// If a file is changed (not saved) in the editor, when it is opened
	// again from the command line, a dialog that prompts the user to save
	// the file pops up. After the dialog is closed, opening files from the
	// command line is unable to bring the editor to the front.
	guint32 time = GDK_IS_X11_WINDOW ( gwnd ) // GDK_BACKEND=x11?
					? gdk_x11_get_server_time ( gwnd )
					: GDK_CURRENT_TIME;
	gtk_window_present_with_time ( GTK_WINDOW ( widget ), time );
}

#else // __WXGTK__

void MyRaiseWindow ( wxTopLevelWindow *wnd )
{
	wnd->Raise();
}

#endif // __WXGTK__

MyServerConnection *server_connection = 0;
MyClientConnection *client_connection = 0;

wxConnectionBase *MyServer::OnAcceptConnection ( const wxString& topic )
{
	if ( topic == IPC_TOPIC )
		return new MyServerConnection();

	// unknown topic
	return NULL;
}

MyServerConnection::MyServerConnection()
	: wxConnection()
	, mFrameWnd ( 0 )
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
	, IPCData *data
	, IPCSize_t size
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
#if wxCHECK_VERSION(2,9,0)
		wxCommandEvent event ( wxEVT_MENU, wxID_OPEN );
#else
		wxCommandEvent event ( wxEVT_COMMAND_MENU_SELECTED, wxID_OPEN );
#endif
		event.SetString ( item );
		wxPostEvent ( frame->GetEventHandler(), event );
	}

	if ( frame->IsIconized() )
	{
		frame->Iconize ( false );
	}
	frame->Show();
	MyRaiseWindow ( frame );

	return true;
}

IPCData *MyServerConnection::OnRequest
	( const wxString& WXUNUSED ( topic )
	, const wxString& item
	, IPCSize_t *size
	, wxIPCFormat WXUNUSED ( format ) /*= wxIPC_PRIVATE */
	)
{
	if ( size == NULL )
		return NULL;

#if wxUSE_DDE_FOR_IPC
	// wxDDEConnection::Request validates the return data
	const static char data[] = "Data";
	*size = 1;
	return data;
#else
	*size = 0;
	return NULL;
#endif
}

bool MyServerConnection::OnStartAdvise ( const wxString& WXUNUSED ( topic ),
        const wxString& WXUNUSED ( item ) )
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

bool MyClientConnection::OnAdvise (
	const wxString& WXUNUSED ( topic )
	, const wxString& WXUNUSED ( item )
	, IPCData * WXUNUSED ( data )
	, IPCSize_t WXUNUSED ( size )
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
{
}

MyClient::MyClient()
{
}

bool MyClient::talkToServer ( int argc, const wxChar * const *argv )
{
	MyClientConnection *connection = ( MyClientConnection * )
			MakeConnection ( _T ( "localhost" ), IPC_SERVICE, IPC_TOPIC );
	if ( !connection || !connection->StartAdvise ( IPC_ADVISE_NAME ) )
		return false;

	// Grab what we need before the server is too busy to response
	IPCSize_t size;
	const void *data = connection->Request ( IPC_FRAME_WND, &size );

	wxString argument;
	// wxConnectionBase::Poke expects something other than NULL in debug
	// version
	static wxChar whatBuffer[] = _T ( "Data" );
	const static size_t bufSize = sizeof ( whatBuffer ) - sizeof ( wxChar );
	if ( argc <= 1 )
	{
		connection->Poke ( IPC_NO_FILE, whatBuffer, bufSize );
	}
	else for ( int i = 1; i < argc; i++ )
	{
		argument = argv[i];
		argument = PathResolver::run ( argument );
		if ( ! connection->Poke ( argument, whatBuffer, bufSize ) )
			break;
	}

	return true;
}

