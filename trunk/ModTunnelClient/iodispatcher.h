/***************************************************************************
                               iodispatcher.h
                             -------------------
	revision             : $Id: iodispatcher.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

	description          : this class handles the multiplexed I/O,
                           dispatching the read/write/exception events
						   to the right objects
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROM_IODISPATCHER_H
#define PROM_IODISPATCHER_H

#include "socket.h"

#define MAX_FDS		1024 // max number of fd's we can handle

class IODispatcher
{
public:
				IODispatcher();

	void		AddFD( Socket *fd, int flags );
	void		RemFD( Socket *fd, int flags );

	void		Dispatch( int fd, int event );

	void		CheckTimeouts( time_t now );

private:
	HWND		Wnd;
	struct {
		Socket	*FD;
		int		Flags;
	}			FDTable[ MAX_FDS ];
	int			MaxFD;

    void		SetSelect( Socket *fd, int flags );
};

// AddFD()/RemFD() flags
#define PROM_IOF_READ		(1 << 0)
#define PROM_IOF_WRITE		(1 << 1)
#define PROM_IOF_ACCEPT		(1 << 2)
#define PROM_IOF_CONNECT	(1 << 3)

#endif

