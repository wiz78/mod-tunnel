/***************************************************************************
	revision             : $Id: Resolver.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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

#ifndef RESOLVER_H
#define RESOLVER_H

#include "socket.h"

typedef void ( *ResolverCallback )( Prom_Addr *addr, void *userdata );

class Resolver
{
public:
						Resolver( ResolverCallback callback, void *userdata );
	virtual				~Resolver();

	void				Resolve( const char *host );
	void				Resolved( LPARAM param );	

private:
	HWND				Wnd;
	ResolverCallback	Callback;
	void				*UserData;
    char				HostEnt[ MAXGETHOSTSTRUCT ];
    HANDLE				AsyncReq;
    Prom_Addr			ServerAddr;
};

#endif /* RESOLVER_H */
