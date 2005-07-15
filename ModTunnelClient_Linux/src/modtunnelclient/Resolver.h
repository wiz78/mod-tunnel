/***************************************************************************
	revision             : $Id: Resolver.h,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
    copyright            : (C) 2002-2005 by Simone Tellini
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

private:
	ResolverCallback	Callback;
	void				*UserData;
};

#endif /* RESOLVER_H */
