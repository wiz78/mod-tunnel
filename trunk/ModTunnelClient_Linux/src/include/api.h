/***************************************************************************
                                    api.h
                             -------------------
    revision             : $Id: api.h,v 1.1.1.1 2005-07-15 13:54:09 tellini Exp $
    copyright            : (C) 2002 by Simone Tellini
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

#ifndef PROMETEO_API_H
#define PROMETEO_API_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#ifdef __cplusplus
#define CDECL "C"
#else
#define CDECL
#endif

#ifndef API_EXPORT
#define API_EXPORT(type)    extern CDECL type
#endif

#ifndef BOOL
#define BOOL	int
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

/* Hostname resolution */
#if HAVE_IPV6
typedef struct in6_addr	Prom_Addr;
#else
typedef struct in_addr	Prom_Addr;
#endif

/* Asynchronous callback for socket I/O */
typedef enum {
				PROM_SOCK_TIMEOUT	= 0,	// data is 0
				PROM_SOCK_ERROR 	= 1,	// data is errno
				PROM_SOCK_READ		= 2,	// data is the number of bytes read
				PROM_SOCK_WRITTEN	= 3,	// data is 0
				PROM_SOCK_EXCEPT	= 4,	// data is 0
				PROM_SOCK_ACCEPT	= 5,	// data is the new Socket *
				PROM_SOCK_CONNECTED	= 6,	// data is 0 if successful, errno otherwise
			 } Prom_SC_Reason;
#ifdef __cplusplus
class Socket;
typedef Socket *SOCKREF;
#else
typedef void *SOCKREF;
#endif
typedef void ( *Prom_SockCallback )( SOCKREF socket, Prom_SC_Reason reason, int data, void *userdata );

/* generic destructor prototype */
typedef void ( *Prom_Destructor )( void *userdata );

#endif /* PROMETEO_API_H */
