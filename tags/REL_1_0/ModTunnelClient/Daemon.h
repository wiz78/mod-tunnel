/***************************************************************************
	revision             : $Id: Daemon.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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
#ifndef DAEMON_H
#define DAEMON_H

#include "iodispatcher.h"
#include "TcpSocket.h"

class Daemon  
{
public:
					Daemon();
	virtual			~Daemon();

	IODispatcher	*GetDispatcher()	{ return( &IO ); }

private:
	IODispatcher	IO;
	TcpSocket		*ListenSock;

	static void		SocketCallback( Socket *socket, Prom_SC_Reason reason, int data, void *userdata );

	void			Accept( TcpSocket *socket );
};

#endif /* DAEMON_H */
