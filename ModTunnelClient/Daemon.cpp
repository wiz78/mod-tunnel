/***************************************************************************
	revision             : $Id: Daemon.cpp,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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
//---------------------------------------------------------------------------
#include "Daemon.h"
#include "Config.h"
#include "Client.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
Daemon::Daemon()
{
	WORD	VersionRequested = MAKEWORD( 2, 0 );
    WSADATA	WsaData;

	WSAStartup( VersionRequested, &WsaData );

	ListenSock = new TcpSocket();

	ListenSock->UseDispatcher( &IO );
	ListenSock->SetAsyncCallback( SocketCallback, this );
	ListenSock->Bind( Config::GetInstance().GetPort() );
	ListenSock->Listen();
}
//---------------------------------------------------------------------------
Daemon::~Daemon()
{
	delete ListenSock;

	WSACleanup();
}
//---------------------------------------------------------------------------
void Daemon::SocketCallback( Socket *socket, Prom_SC_Reason reason, int data, void *userdata )
{
	Daemon *obj = reinterpret_cast<Daemon *>( userdata );

	switch( reason ) {

		case PROM_SOCK_ACCEPT:
			obj->Accept( reinterpret_cast<TcpSocket *>( data ));
			break;
	}
}
//---------------------------------------------------------------------------
void Daemon::Accept( TcpSocket *socket )
{
	new Client( this, socket );
}
//---------------------------------------------------------------------------
