/***************************************************************************
	revision             : $Id: mod_tunnel.h,v 1.1.1.1 2004-09-13 15:11:05 tellini Exp $
    copyright            : (C) 2004 by Simone Tellini
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

#ifndef MOD_TUNNEL_H
#define MOD_TUNNEL_H

#include <time.h>

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_core.h"


typedef struct 
{
	char			*Host;
	int				Port;
	int				Sock;
	int				Enabled;
} tunnel_state;


#endif /* MOD_TUNNEL_H */
