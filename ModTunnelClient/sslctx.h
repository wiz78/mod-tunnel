/***************************************************************************
                                 sslctx.h
                             -------------------
    revision             : $Id: sslctx.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

    description          : SSL context wrapper
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROM_SSLCTX_H
#define PROM_SSLCTX_H

#include <openssl/ssl.h>

class SSLCtx
{
public:
	typedef enum { CLIENT, SERVER, DUMMY } SessionType;

					SSLCtx( SessionType type );
					~SSLCtx();

	bool			IsValid( void ) const { return( Ctx != NULL ); }

	SSL_CTX			*GetCtx( void ) const { return( Ctx ); }

private:
	SSL_CTX			*Ctx;
	SessionType		Type;

	void			InitClient( void );
	void			InitServer( void );
};

#endif /* PROM_SSLCTX_H */

