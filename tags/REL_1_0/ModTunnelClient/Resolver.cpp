/***************************************************************************
	revision             : $Id: Resolver.cpp,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Resolver.h"

#define WM_RESOLVED		(WM_USER + 100)

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//---------------------------------------------------------------------------
Resolver::Resolver( ResolverCallback callback, void *userdata )
{
    WNDCLASS wc;

	Callback = callback;
	UserData = userdata;

	memset( &ServerAddr, 0, sizeof( ServerAddr ));
	memset( &HostEnt, 0, sizeof( HostEnt ));
	memset( &wc, 0, sizeof( wc ));

    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.hInstance     = static_cast<HINSTANCE>( GetModuleHandle( NULL ));;
    wc.lpszClassName = "ResolverWndClass";
    wc.cbWndExtra    = sizeof( Resolver * );

    RegisterClass( &wc );

    // Create the main window.
    Wnd = CreateWindow( wc.lpszClassName, "Resolver",
                        0, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL,
                        (HMENU) NULL, wc.hInstance, (LPVOID) NULL );

    if( !Wnd )
    	throw "Cannot create the Resolver window!";

    SetWindowLong( Wnd, 0, reinterpret_cast<LONG>( this ));
}
//---------------------------------------------------------------------------
Resolver::~Resolver()
{
	if( AsyncReq )
		WSACancelAsyncRequest( AsyncReq );

	DestroyWindow( Wnd );
}
//---------------------------------------------------------------------------
void Resolver::Resolved( LPARAM param )
{
	Prom_Addr *addr = NULL;

	AsyncReq = NULL;

	if( !WSAGETASYNCERROR( param )) {
        struct hostent *ent = (struct hostent *)HostEnt;

		memcpy( &ServerAddr.s_addr, ent->h_addr, ent->h_length );

    	addr = &ServerAddr;
    }

	( *Callback )( addr, UserData );
}
//---------------------------------------------------------------------------
void Resolver::Resolve( const char *host )
{
	AsyncReq = WSAAsyncGetHostByName( Wnd, WM_RESOLVED, host, HostEnt, sizeof( HostEnt ));
}
//---------------------------------------------------------------------------
static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	Resolver	*resolved;

    switch( uMsg ) {

		case WM_RESOLVED:
        	resolved = reinterpret_cast<Resolver *>( GetWindowLong( hWnd, 0 ));

            resolved->Resolved( lParam );
            break;

        default:
            return( DefWindowProc( hWnd, uMsg, wParam, lParam ));
    }

    return( 0 );
}
//---------------------------------------------------------------------------
