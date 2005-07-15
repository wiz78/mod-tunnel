/***************************************************************************
	revision             : $Id: Resolver.cpp,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
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

#include "Resolver.h"
#include "main.h"

//---------------------------------------------------------------------------
Resolver::Resolver( ResolverCallback callback, void *userdata )
{
	Callback = callback;
	UserData = userdata;
}
//---------------------------------------------------------------------------
Resolver::~Resolver()
{
}
//---------------------------------------------------------------------------
void Resolver::Resolve( const char *host )
{
	struct hostent	*ent;

	DBG( printf( "Resolve( %s )\n", host ));

	if( ent = gethostbyname( host )) {
		Prom_Addr addr;
		   	
		memcpy( &addr.s_addr, ent->h_addr, sizeof( addr.s_addr ));

		Callback( &addr, UserData );
		
	} else
		Callback( NULL, UserData );
}
//---------------------------------------------------------------------------
