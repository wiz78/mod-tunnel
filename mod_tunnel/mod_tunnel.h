/***************************************************************************
	revision             : $Id: mod_tunnel.h,v 1.2 2009-04-11 17:26:41 tellini Exp $
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
#include "apr_strings.h"
#include "apr_network_io.h"


typedef struct 
{
	const char		*Host;
	int				Port;
	int				Enabled;
} tunnel_state;


#endif /* MOD_TUNNEL_H */
