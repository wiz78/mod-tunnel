/***************************************************************************
                              iodispatcher.cpp
                             -------------------
	revision             : $Id: iodispatcher.cpp,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

	description          : this class handles the multiplexed I/O,
                           dispatching the read/write/exception events
						   to the right objects
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iodispatcher.h"

#define WM_SOCKET_EVENT		(WM_USER + 100)

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//---------------------------------------------------------------------------
IODispatcher::IODispatcher()
{
    WNDCLASS wc;

	memset( &wc, 0, sizeof( wc ));

    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.hInstance     = static_cast<HINSTANCE>( GetModuleHandle( NULL ));;
    wc.lpszClassName = "IODispatcherWndClass";
    wc.cbWndExtra    = sizeof( IODispatcher * );

    if( !RegisterClass( &wc ))
		throw "Cannot register the IODispatcher class!";

    // Create the main window.
    Wnd = CreateWindow( wc.lpszClassName, "IODispatcher",
                        0, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL,
                        (HMENU) NULL, wc.hInstance, (LPVOID) NULL );

    if( !Wnd )
    	throw "Cannot create the IODispatcher window!";

    SetWindowLong( Wnd, 0, reinterpret_cast<LONG>( this ));

	memset( FDTable, 0, sizeof( FDTable ));

	MaxFD = 0;
}
//---------------------------------------------------------------------------
static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	IODispatcher   *io;

    switch( uMsg ) {

		case WM_SOCKET_EVENT:
        	io = reinterpret_cast<IODispatcher *>( GetWindowLong( hWnd, 0 ));

            io->Dispatch( wParam, lParam );
            break;

        default:
            return( DefWindowProc( hWnd, uMsg, wParam, lParam ));
    }

    return( 0 );
}
//---------------------------------------------------------------------------
void IODispatcher::AddFD( Socket *fd, int flags )
{
	int fdnum;

	for( fdnum = 0; fdnum < MaxFD; fdnum++ )
    	if( FDTable[ fdnum ].FD == fd )
            break;

	if( fd->IsValid() && (( fdnum < MaxFD ) || ( MaxFD < MAX_FDS ))) {

    	if( fdnum >= MaxFD ) {
        	fdnum                  = MaxFD++;
            FDTable[ fdnum ].Flags = 0;
        }

		FDTable[ fdnum ].FD     = fd;
		FDTable[ fdnum ].Flags |= flags;

        SetSelect( fd, FDTable[ fdnum ].Flags );
	}
}
//---------------------------------------------------------------------------
void IODispatcher::RemFD( Socket *fd, int flags )
{
	int	fdnum;

	for( fdnum = 0; fdnum < MaxFD; fdnum++ )
    	if( FDTable[ fdnum ].FD == fd )
            break;

	if( fdnum < MaxFD ) {

		FDTable[ fdnum ].Flags &= ~flags;

        SetSelect( fd, FDTable[ fdnum ].Flags );

		if( !FDTable[ fdnum ].Flags && ( --MaxFD > 0 )) {

			FDTable[ fdnum ].FD    = FDTable[ MaxFD ].FD;
            FDTable[ fdnum ].Flags = FDTable[ MaxFD ].Flags;
		}
	}
}
//---------------------------------------------------------------------------
void IODispatcher::SetSelect( Socket *fd, int flags )
{
	int	events = 0;

    if( flags & PROM_IOF_READ )
    	events |= FD_READ;

    if( flags & PROM_IOF_WRITE )
    	events |= FD_WRITE;

    if( flags & PROM_IOF_ACCEPT )
    	events |= FD_ACCEPT;

    if( flags & PROM_IOF_CONNECT )
    	events |= FD_CONNECT;

    WSAAsyncSelect( fd->GetFD(), Wnd, WM_SOCKET_EVENT, events );
}
//---------------------------------------------------------------------------
void IODispatcher::CheckTimeouts( time_t now )
{
	for( int i = 0; i < MaxFD; i++ ) {
		Socket	*fd = FDTable[ i ].FD;

		if( fd && fd->HasTimedOut( now ))
			fd->HandleTimeout();
	}
}
//---------------------------------------------------------------------------
void IODispatcher::Dispatch( int fd, int event )
{
	int flags = 0;

	if( event & FD_READ )
    	flags |= PROM_IOF_READ;

	if( event & FD_WRITE )
    	flags |= PROM_IOF_WRITE;

	if( event & FD_ACCEPT )
    	flags |= PROM_IOF_ACCEPT;

	if( event & FD_CONNECT )
    	flags |= PROM_IOF_CONNECT;

	for( int i = 0; i < MaxFD; i++ )
		if( FDTable[ i ].FD->GetFD() == fd ) {

        	if( FDTable[ i ].Flags & flags ) {

				switch( flags ) {

					case PROM_IOF_READ:
					case PROM_IOF_ACCEPT:
						FDTable[ i ].FD->HandleRead();
						break;

					case PROM_IOF_WRITE:
					case PROM_IOF_CONNECT:
						FDTable[ i ].FD->HandleWrite();
						break;
				}
			}

            break;
		}
}
//---------------------------------------------------------------------------
