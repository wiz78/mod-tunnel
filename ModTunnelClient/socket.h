/***************************************************************************
                                  socket.h
                             -------------------
	revision             : $Id: socket.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

	description          : socket interface wrapper
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROM_SOCKET_H
#define PROM_SOCKET_H

#include <winsock2.h>
#include <windows.h>
#include <time.h>

#include "bitfield.h"
#include "buffer.h"

class IODispatcher;
class Socket;

typedef struct in_addr	Prom_Addr;
typedef int socklen_t;

/* Asynchronous callback for socket I/O */
typedef enum {
				PROM_SOCK_TIMEOUT	= 0,	// data is 0
				PROM_SOCK_ERROR 	= 1,	// data is errno
				PROM_SOCK_READ		= 2,	// data is the number of bytes read
				PROM_SOCK_WRITTEN	= 3,	// data is 0
				PROM_SOCK_ACCEPT	= 5,	// data is the new Socket *
				PROM_SOCK_CONNECTED	= 6,	// data is 0 if successful, errno otherwise
			 } Prom_SC_Reason;

typedef void ( *Prom_SockCallback )( Socket *socket, Prom_SC_Reason reason, int data, void *userdata );
typedef void ( *Prom_Destructor )( void *userdata ); 

// Flags
#define PROM_SOCKF_LISTEN			(1 << 0)
#define PROM_SOCKF_CONNECTING		(1 << 1)
#define PROM_SOCKF_SENDING			(1 << 2)
#define PROM_SOCKF_RECVING			(1 << 3)
#define PROM_SOCKF_NOTIFY_DISCONN	(1 << 4)
#define PROM_SOCKF_NONBLOCKING		(1 << 5)


class Socket
{
public:
						Socket( int family, int type, int proto );
						Socket( int fd );
	virtual				~Socket();

	void				Close( void );

	bool				Shutdown( int how = 2 );

	// synchronous functions
	bool				Connect( const char *host );	// host is a dotted form name
														// no name resolution is performed
	bool				Connect( struct sockaddr *addr, socklen_t len );
	virtual bool		Send( const void *data, int size, int flags = 0 );
	virtual int			Recv( void *buffer, int size, int flags = 0, int timeout = -1 );
	// receive a CRLF, LF or \0 terminated line
	int					RecvLine( char *buffer, int size, int timeout = -1 );
	bool				Printf( const char *fmt, ... );

	// asynchronous ones
	// in case of error they return false AND the
	// callback is called with PROM_SOCK_ERROR reason
	bool				AsyncConnect( const char *host, int timeout = -1 ); // see comment on Connect( char *host )
	bool				AsyncConnect( struct sockaddr *addr, socklen_t len, int timeout = -1 );
	bool				AsyncSend( const void *data, int size, int flags = 0 );
	bool				AsyncRecv( void *data, int size, int flags = 0, int timeout = -1 );
	bool				AsyncPrintf( const char *fmt, ... );
	// send PROM_SOCK_READ, 0 when the peer disconnects
	// call it when no AsyncRecv()'s are pending
	// call it with timeout = -1 to reset it
	void				NotifyOnDisconnect( int timeout = 0 );
	bool				WritePending( void ) const { return( Flags.IsSet( PROM_SOCKF_SENDING )); }

	// miscellaneous functions
	bool				Listen( int backlog = 5 );

	// a callback MUST be set if using any async call, including Listen()
	void				SetAsyncCallback( Prom_SockCallback callback, void *userdata );

	// socket-family dependant functions
	virtual Socket		*Accept( void ) = 0;
	virtual char		*GetPeerName( void ) = 0;	// human readable name
	virtual char		*GetLocalName( void ) = 0;

	bool				SetLinger( bool on, int timeout = 0 );
	bool				SetReuseAddr( bool on );

	// try to guess whether the socket is still connected to the other end
	bool				StillConnected( void );

	// Fd interface implementation
	virtual void		HandleRead( void );
	virtual void		HandleWrite( void );
	virtual void		HandleTimeout( void );

	int					GetFD( void ) const { return( FD ); }
	void				SetFD( int fd ) { FD = fd; }

	bool				IsValid( void ) const { return( FD >= 0 ); }

	void				SetTimeout( int seconds );
	bool				HasTimedOut( time_t now ) const { return( Timeout && ( now > Timeout )); }

	bool				SetNonBlocking( bool dontblock );
	bool				IsNonBlocking( void ) const;

	void				UseDispatcher( IODispatcher *iod );

	void				SetUserData( void *userdata, Prom_Destructor destructor );
	void				*GetUserData( void ) const { return( UserData ); }

protected:
	int					FD;
	time_t				Timeout;
	IODispatcher		*Dispatcher;
	void				*UserData;
	Prom_Destructor		UserDataDestructor;
	BitField			Flags;
	Prom_SockCallback	AsyncCallback;
	void				*AsyncUserData;
	void				*AsyncReadBuffer;
	int					AsyncReadFlags;
	int					AsyncWriteFlags;
	Buffer				Data;
	unsigned int		ReadSize;	// how much of Data we've already sent/recvd
	unsigned int		WriteOffset;

	void				Init( void );

	void				Callback( Prom_SC_Reason reason, int data );

	virtual bool		NameToAddr( const char *name, struct sockaddr **addr, socklen_t *len ) = 0;
};

#endif
