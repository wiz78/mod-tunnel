/***************************************************************************
                                sslsocket.h
                             -------------------
    revision             : $Id: sslsocket.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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

#ifndef PROM_SSLSOCKET_H
#define PROM_SSLSOCKET_H

#include <openssl/ssl.h>

#include "TCPSocket.h"
#include "sslctx.h"

class SSLSocket : public TcpSocket
{
public:
					SSLSocket( SSLCtx *ctx );
					SSLSocket( SSLCtx *ctx, int fd );
					~SSLSocket();

	bool			SSLInitSession( SSLCtx::SessionType type );
	void			SSLEndSession( void );

	const char		*SSLGetErrorStr( void );

	X509			*SSLGetPeerCert( void ) { return( SSL_get_peer_certificate( Ssl )); }
	STACK_OF(X509)	*SSLGetPeerCertChain( void ) { return( SSL_get_peer_cert_chain( Ssl )); }

	void			SSLSetCert( X509 *cert ) { SSL_use_certificate( Ssl, cert ); }

	// flags is ignored unless it's MSG_PEEK, which is emulated
	// buffering data
	virtual bool	Send( const void *data, int size, int flags = 0 );
	// flags is ignored, timeout too at the moment
	virtual int		Recv( void *buffer, int size, int flags = 0, int timeout = -1 );

private:
	SSL				*Ssl;
	SSLCtx			*SslCtx;

	void			Setup( void );
};

#endif

