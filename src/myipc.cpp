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
#include "pathresolver.h"
#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )
#include <gtk/gtk.h>

guint32 XTimeNow()
{
	struct timespec ts;
	clock_gettime ( CLOCK_MONOTONIC, &ts );
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
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
	, mFrameWnd ( ( wxIntPtr ) NULL )
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
		//frame->addToFileQueue ( ( wxString& ) item ); // prevent event loop problems
	}
#ifndef __WXMSW__
#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )
	// Processes mostly cannot raise their own windows.
	// http://osdir.com/ml/gnome.gaim.devel/2004-12/msg00077.html
	GtkWidget *widget = frame->GetHandle();
	GdkWindow *window = gtk_widget_get_window ( widget );
	gdk_x11_window_set_user_time ( window,
		//XTimeNow() ); // This works too.
		gdk_x11_get_server_time ( window ) );
	//gdk_window_show ( window );
	//gdk_window_raise ( window );
	//gtk_window_present ( GTK_WINDOW ( widget ) );
#endif // __WXGTK__ && !__NO_GTK__
	frame->Show();
	frame->Raise();
#endif // __WXMSW__
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

	if ( item == IPC_FRAME_WND )
	{
		if ( !mFrameWnd )
		{
			wxWindow *window = wxTheApp->GetTopWindow();
			if ( window )
			{
#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )
				GtkWidget *wnd = window->GetHandle();
				if ( wnd )
				{
					GdkWindow *gwnd = gtk_widget_get_window ( wnd );
					if ( gwnd )
						mFrameWnd = GDK_WINDOW_XID ( gwnd );
				}
#else
				mFrameWnd = window->GetHandle();
#endif
			}
		}
		*size = sizeof mFrameWnd;
		return ( IPCData * ) &mFrameWnd;
	}

	*size = 0;
	return NULL;
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

	if ( !data )
		return false;

	// Bring the window to front
#ifdef __WXMSW__
 	if ( size == sizeof ( HWND ) )
	{
		HWND hwnd = * ( const HWND * )data;
		if ( ::IsIconic ( hwnd ) )
			::ShowWindow ( hwnd, SW_RESTORE );
		else
			::SetForegroundWindow ( hwnd );
	}
#elif defined ( __WXGTK__x ) // It doesn't work
	if ( size == sizeof ( GdkNativeWindow ) )
	{
		GdkNativeWindow xWnd = * ( GdkNativeWindow * ) data;
		GdkWindow *window = gdk_window_foreign_new ( xWnd );
		if ( window )
		{
			gdk_x11_window_set_user_time ( window,
				XTimeNow() );
				//gdk_x11_get_server_time ( window ) );
			gdk_window_show ( window );
			gdk_window_raise ( window );
			gdk_window_unref ( window );
			//GtkWidget *widget;
			//gdk_window_get_user_data(window, (void**)&widget);
			//printf ("widget: %p\n",widget);
			//gtk_window_present ( GTK_WINDOW ( widget ) );
		}
	}
#endif // __WXMSW__

	return true;
}
