/***************************************************************************
	revision             : $Id: Client.cpp,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
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
#include "Client.h"
#include "Daemon.h"
#include "Cfg.h"
#include "sslsocket.h"
#include "Resolver.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
Client::Client( Daemon *daemon, TcpSocket *socket )
{
	Config& cfg = Config::GetInstance();

	ClientSock  = socket;
	ServerSock  = NULL;
	Ctx         = NULL;
	Parent      = daemon;
	Server	    = new URL( cfg.GetServer() );
	ResolverObj = new Resolver( ResolverCB, this );

	if( cfg.IsUsingProxy() )
		ResolverObj->Resolve( cfg.GetProxyHost() );
	else
		ResolverObj->Resolve( Server->GetHost() );
}
//---------------------------------------------------------------------------
Client::~Client()
{
	delete ResolverObj;
	delete ClientSock;
	delete ServerSock;
	delete Ctx;
	delete Server;
}
//---------------------------------------------------------------------------
void Client::SocketCallback( Socket *socket, Prom_SC_Reason reason, int data, void *userdata )
{
	Client *obj = reinterpret_cast<Client *>( userdata );

	switch( reason ) {

		case PROM_SOCK_CONNECTED:
			obj->Connected();
			break;

		case PROM_SOCK_READ:
			obj->Read( reinterpret_cast<TcpSocket *>( socket ), data );
			break;

		case PROM_SOCK_ERROR:
		case PROM_SOCK_TIMEOUT:
			obj->Error( data );
			break;
	}
}
//---------------------------------------------------------------------------
void Client::ResolverCB( Prom_Addr *addr, void *userdata )
{
	reinterpret_cast<Client *>( userdata )->Resolved( addr );
}
//---------------------------------------------------------------------------
void Client::Resolved( Prom_Addr *addr )
{
	if( addr ) {
		Config& cfg = Config::GetInstance();
		int		port;

		DBG( printf( "Resolved() -> %s\n", TcpSocket::AddrToName( addr )));

		ServerSock = new TcpSocket();

		ServerSock->UseDispatcher( Parent->GetDispatcher() );
		ServerSock->SetAsyncCallback( SocketCallback, this );

		if( cfg.IsUsingProxy() )
			port = cfg.GetProxyPort();
		else
			port = Server->GetPort();

		ServerSock->AsyncConnect( addr, port, 300 );

		delete ResolverObj;

		ResolverObj = NULL;

	} else
		delete this;
}
//---------------------------------------------------------------------------
void Client::Connected( void )
{
	char line[1024];

	if( !Config::GetInstance().IsUsingProxy() || ProxyConnect() ) {

		if( !strcmp( Server->GetScheme(), "https" )) {
			SSLSocket *ssl;
			
			Ctx = new SSLCtx( SSLCtx::CLIENT );
			ssl = new SSLSocket( Ctx, ServerSock->GetFD() );

			ssl->SSLInitSession( SSLCtx::CLIENT );

			ServerSock->SetFD( -1 );
			
			delete ServerSock;

			ServerSock = ssl;

			ServerSock->UseDispatcher( Parent->GetDispatcher() );
			ServerSock->SetAsyncCallback( SocketCallback, this );
		}

		ServerSock->Printf( "TUNNEL / HTTP/1.1\r\n"
							"Host: %s\r\n"
							"\r\n",
							Server->GetHost() );

		ServerSock->RecvLine( line, sizeof( line ), 300 );

		if( line[0] == '+' ) {

			ClientSock->UseDispatcher( Parent->GetDispatcher() );
			ClientSock->SetAsyncCallback( SocketCallback, this );

			if( ServerSock->AsyncRecv( ServerBuf, sizeof( ServerBuf )))
				ClientSock->AsyncRecv( ClientBuf, sizeof( ClientBuf ));

		} else
			delete this;

	} else
		delete this;
}
//---------------------------------------------------------------------------
bool Client::ProxyConnect( void )
{
	Config& cfg = Config::GetInstance();
	char	line[1024], *ptr;
	bool	ret = false;

	ServerSock->Printf( "CONNECT %s:%d HTTP/1.1\r\n"
						"Host: %s:%d\r\n"
						"\r\n",
						Server->GetHost(), Server->GetPort(),
						Server->GetHost(), Server->GetPort() );

	ServerSock->RecvLine( line, sizeof( line ), 300 );

	ptr = strchr( line, ' ' );

	if( ptr && ( ptr < &line[ sizeof( line ) - 4 ] ) && !strncmp( ptr + 1, "200 ", 4 )) {

		ret = true;

		do {
			ServerSock->RecvLine( line, sizeof( line ), 300 );
		} while( line[0] );
	}

	return( ret );
}
//---------------------------------------------------------------------------
void Client::Read( TcpSocket *socket, int len )
{
	if( len <= 0 )
		delete this;
	else if( socket == ClientSock ) {

		if( ServerSock->AsyncSend( ClientBuf, len ))
			ClientSock->AsyncRecv( ClientBuf, sizeof( ClientBuf ));

	} else {

		if( ClientSock->AsyncSend( ServerBuf, len ))
			ServerSock->AsyncRecv( ServerBuf, sizeof( ServerBuf ));
	}
}
//---------------------------------------------------------------------------
void Client::Error( int errn )
{
	delete this;
}
//---------------------------------------------------------------------------
