/***************************************************************************
                                sslsocket.cpp
                             -------------------
    revision             : $Id: sslsocket.cpp,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

    description          : TLS/SSL enabled TCP socket
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#pragma hdrstop
#include <string.h>
#include <openssl/err.h>

#include "sslsocket.h"

//---------------------------------------------------------------------------
SSLSocket::SSLSocket( SSLCtx *ctx ) : TcpSocket()
{
	SslCtx = ctx;

	Setup();
}
//---------------------------------------------------------------------------
SSLSocket::SSLSocket( SSLCtx *ctx, int fd ) : TcpSocket( fd )
{
	SslCtx = ctx;

	Setup();
}
//---------------------------------------------------------------------------
SSLSocket::~SSLSocket()
{
	SSLEndSession();

    Close();
}
//---------------------------------------------------------------------------
void SSLSocket::Setup( void )
{
	Ssl = NULL;
}
//---------------------------------------------------------------------------
bool SSLSocket::SSLInitSession( SSLCtx::SessionType type )
{
	bool	ok = false;

	if( SslCtx->IsValid() ) {

		Ssl = SSL_new( SslCtx->GetCtx() );

		if( Ssl ) {
			int res;

			SSL_set_fd( Ssl, FD );

			if( type == SSLCtx::CLIENT )
				res = SSL_connect( Ssl );
			else
				res = SSL_accept( Ssl );

			ok = res != -1;
		}
	}

	return( ok );
}
//---------------------------------------------------------------------------
void SSLSocket::SSLEndSession( void )
{
	if( Ssl ) {

		if( SSL_shutdown( Ssl ) == 0 )
			SSL_shutdown( Ssl );

		SSL_free( Ssl );

		Ssl = NULL;
	}
}
//---------------------------------------------------------------------------
bool SSLSocket::Send( const void *data, int size, int flags )
{
	return( SSL_write( Ssl, data, size ) == size );
}
//---------------------------------------------------------------------------
int SSLSocket::Recv( void *buffer, int size, int flags, int timeout )
{
	int nread;

    if( flags & MSG_PEEK )
    	nread = SSL_peek( Ssl, buffer, size );
    else
		nread = SSL_read( Ssl, buffer, size );

	return( nread );
}
//---------------------------------------------------------------------------
const char *SSLSocket::SSLGetErrorStr( void )
{
	return( ERR_error_string( ERR_get_error(), NULL ));
}
//---------------------------------------------------------------------------
