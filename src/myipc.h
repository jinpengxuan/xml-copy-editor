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

#ifndef MY_IPC_H
#define MY_IPC_H

//#define wxUSE_DDE_FOR_IPC 0

#include <wx/wx.h>
#include <wx/ipc.h>

#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )
#if wxCHECK_VERSION(2,9,0) // GSocket is defined in wxWidgets 2.8
#include <gdk/gdkx.h>
#else // wxCHECK_VERSION(2,9,0)
#define GSocket GlibGSocket
#include <gdk/gdkx.h>
#undef GSocket
#endif // wxCHECK_VERSION(2,9,0)
#endif // __WXGTK__ && !__NO_GTK__

#define IPC_SERVICE _T("4242")
#define IPC_TOPIC _T("IPC TEST")
#define IPC_ADVISE_NAME _T("Item")
#define IPC_FRAME_WND _T("FrameWnd")
#define IPC_NO_FILE _T("[nofile]")

#if wxCHECK_VERSION(2,9,0)
typedef const void IPCData;
typedef size_t IPCSize_t;
#else
typedef wxChar IPCData;
typedef int IPCSize_t;
#endif

class MyServerConnection;
class MyClientConnection;
extern MyServerConnection *server_connection;
extern MyClientConnection *client_connection;

class MyServerConnection : public wxConnection
{
	public:
		MyServerConnection();
		~MyServerConnection();
		bool OnPoke ( const wxString& topic
					, const wxString& item
					, IPCData *data
					, IPCSize_t size
					, wxIPCFormat format
					);
		bool OnStartAdvise ( const wxString& topic, const wxString& item );
		IPCData *OnRequest(const wxString& topic, const wxString& item,
				IPCSize_t *size, wxIPCFormat format = wxIPC_PRIVATE );

	protected:
#if defined ( __WXGTK__ ) && !defined ( __NO_GTK__ )
		GdkNativeWindow mFrameWnd;
#else
		WXWidget mFrameWnd;
#endif
};

class MyClientConnection: public wxConnection
{
	public:
		MyClientConnection();
		bool OnAdvise ( const wxString& topic
					, const wxString& item
					, IPCData *data
					, IPCSize_t size
					, wxIPCFormat format );
		bool OnDisconnect();
};

class MyClient: public wxClient
{
	public:
		MyClient();
		wxConnectionBase *OnMakeConnection();
		bool talkToServer ( int argc, const wxChar * const *argv );
};

class MyServer: public wxServer
{
	public:
		MyServer();
		wxConnectionBase *OnAcceptConnection ( const wxString& topic );
};

#endif

