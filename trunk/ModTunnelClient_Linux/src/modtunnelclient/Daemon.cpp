/***************************************************************************
	revision             : $Id: Daemon.cpp,v 1.2 2005-07-18 09:01:14 tellini Exp $
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

#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"
#include "Daemon.h"
#include "Cfg.h"
#include "Client.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
Daemon::Daemon( int argc, const char *argv[] )
{
	if( argc >= 2 )
		Config::SetCfgFile( argv[ 1 ] );

	ListenSock = new TcpSocket();

	ListenSock->UseDispatcher( &IO );
	ListenSock->SetAsyncCallback( SocketCallback, this );
	ListenSock->SetLinger( false );
	ListenSock->Bind( Config::GetInstance().GetPort() );
	ListenSock->Listen();
}
//---------------------------------------------------------------------------
Daemon::~Daemon()
{
	delete ListenSock;
}
//---------------------------------------------------------------------------
void Daemon::Run( void )
{
#if !DEBUG
	if( fork() == 0 ) {

		freopen( "/dev/null", "r", stdin  );
		freopen( "/dev/null", "r", stdout );
		freopen( "/dev/null", "r", stderr );

		setsid();
		umask( 022 );
#endif

		for(;;)
			IO.WaitEvents( 300 );

#if !DEBUG
	}
#endif
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
