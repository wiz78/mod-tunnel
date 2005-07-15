/***************************************************************************
	revision             : $Id: Client.h,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
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
#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "tcpsocket.h"
#include "url.h"
#include "sslctx.h"
#include "Resolver.h"

class Daemon;

class Client  
{
public:
					Client( Daemon *daemon, TcpSocket *socket );
	virtual			~Client();

	static void		ResolverCB( Prom_Addr *addr, void *userdata );

private:
	Resolver		*ResolverObj;
	URL				*Server;
	TcpSocket		*ClientSock;
	TcpSocket		*ServerSock;
	Daemon			*Parent;
	SSLCtx			*Ctx;
	char			ClientBuf[ 1024 * 64 ];
	char			ServerBuf[ 1024 * 64 ];

	static void		SocketCallback( Socket *socket, Prom_SC_Reason reason, int data, void *userdata );

	void			Resolved( Prom_Addr *addr );
	void			Connected( void );
	bool			ProxyConnect( void );
	void			Read( TcpSocket *socket, int len );
	void			Error( int errn );
};

#endif /* CLIENT_H */
