/***************************************************************************
	revision             : $Id: mod_tunnel.c,v 1.1.1.1 2004-09-13 15:11:05 tellini Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include "mod_tunnel.h"

#define MOD_TUNNEL_VERSION_INFO_STRING	"mod_tunnel/1.0"
#define MODTUNNEL_TIMEOUT				300
#define TUNNEL_HANDLER					"tunnel-handler"

module MODULE_VAR_EXPORT tunnel_module;

// ------------------- UTILITY FUNCS -----------------

static char *set_host( cmd_parms *parms, void *dummy, char *arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Host = arg;

	return( NULL );
}

static char *set_port( cmd_parms *parms, void *dummy, char *arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Port = atoi( arg );

	return( NULL );
}

static char *set_enabled( cmd_parms *parms, void *dummy, char *arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Enabled = strcmp( arg, "on" ) == 0;

	return( NULL );
}

static int resolve_name( const char *name )
{
	struct in_addr		addr;
	struct hostent		*host;

	memset( &addr, 0, sizeof( addr ));

	if(( addr.s_addr = inet_addr( name )) == -1 ) {

		if( host = gethostbyname( name ))
			memcpy( &addr.s_addr, host->h_addr, sizeof( addr.s_addr ));
	}

	return( addr.s_addr );
}

static int open_sock( tunnel_state *cfg )
{
	struct sockaddr_in	addr;

	memset( &addr, 0, sizeof( addr ));

	addr.sin_family	= AF_INET;
	addr.sin_port	= htons( cfg->Port );

	if( addr.sin_addr.s_addr = resolve_name( cfg->Host )) {

		cfg->Sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

		if( cfg->Sock >= 0 ) {

			if( connect( cfg->Sock, (struct sockaddr *)&addr, sizeof( addr )) < 0 ) {
			
				close( cfg->Sock );
			
				cfg->Sock = -1;
			}
		}
	}
		
	return( cfg->Sock >= 0 );
}


// ------------------- HOOKS -----------------
	
/* Set up space for the various major configuration options */
static void *tunnel_make_state( pool *p, server_rec *s )
{
	tunnel_state *cfg = ( tunnel_state * ) ap_palloc( p, sizeof( tunnel_state ));

	memset( cfg, 0, sizeof( *cfg ));

	return( cfg );
}

/* Called on the exit of an httpd child process */
static void tunnel_child_exit( server_rec *s, pool *p )
{
	tunnel_state *cfg = ap_get_module_config( s->module_config, &tunnel_module );

	close( cfg->Sock );

	cfg->Sock = -1;
}

static void mod_tunnel_init( server_rec *server, pool *p )
{
	ap_add_version_component( MOD_TUNNEL_VERSION_INFO_STRING );
}

static int tunnel_type_checker( request_rec *r )
{
	tunnel_state	*cfg = ap_get_module_config( r->server->module_config, &tunnel_module );
	int				ret = DECLINED;

	if( cfg->Enabled && !strcmp( r->method, "TUNNEL" )) {
		r->handler = TUNNEL_HANDLER;
		ret        = OK;
	}

	return( ret );
}

static int tunnel_handler( request_rec *r )
{
	tunnel_state	*cfg = ap_get_module_config( r->server->module_config, &tunnel_module );
	int				ret = DECLINED;

	if( cfg->Enabled ) {
		   
		if( !strcmp( r->method, "TUNNEL" )) {
			BUFF *client = r->connection->client;

			ret = OK;

			// disable write buffering
			ap_bsetflag( client, B_WR, 0 );

			if( open_sock( cfg )) {
				int	go = 1, maxfd;

				ap_bwrite( client, "+OK\n", 4 );

				maxfd = (( client->fd > cfg->Sock ) ? client->fd : cfg->Sock ) + 1;

				do {
					fd_set			fds;
					struct timeval	to;

					FD_ZERO( &fds );
					FD_SET( client->fd, &fds );
					FD_SET( cfg->Sock, &fds );

					memset( &to, 0, sizeof( to ));

					to.tv_sec = MODTUNNEL_TIMEOUT;

					if( select( maxfd, &fds, NULL, NULL, &to ) > 0 ) {
						char buf[ 1024 * 64 ];
						int	 len;

						if( FD_ISSET( client->fd, &fds )) {

							if(( len = recv( client->fd, buf, sizeof( buf ), 0 )) > 0 )
								send( cfg->Sock, buf, len, 0 );
							
						} else {

							if(( len = recv( cfg->Sock, buf, sizeof( buf ), 0 )) > 0 )
								send( client->fd, buf, len, 0 );
						}

						if( len <= 0 )
							go = 0;

					} else
						go = 0;
					
				} while( go );

				close( cfg->Sock );

				cfg->Sock = -1;

			} else {
				char *err = ap_pstrcat( r->pool, "-ERROR: ", strerror( errno ), "\n", NULL );

				ap_bwrite( client, err, strlen( err ));
			}
			
		} else
			ret = FORBIDDEN;
	}

	return( ret );
}

// ------------------- MOD CONFIG -----------------

/* Setup of the available httpd.conf configuration commands.
 * command, function called, NULL, where available, how many arguments, verbose description
 */
typedef const char *(*hook_func)();

static const command_rec tunnel_cmds[] = 
{
	{ "TunnelEnabled", (hook_func) set_enabled, NULL, RSRC_CONF, TAKE1,
	  "Set to 'on' (without quotes) to enable the TUNNEL method." },

	{ "TunnelHost", (hook_func) set_host, NULL, RSRC_CONF, TAKE1,
	  "The host to connect to." },

	{ "TunnelPort", (hook_func) set_port, NULL, RSRC_CONF, TAKE1,
	  "The port to connect to." },

	{ NULL }
};


static const handler_rec tunnel_handlers[] =
{
	{ TUNNEL_HANDLER, tunnel_handler },
	{ NULL }
};

/* The configuration array that sets up the hooks into the module. */
module tunnel_module = 
{
	STANDARD_MODULE_STUFF,
	mod_tunnel_init,		 /* initializer */
	NULL,					 /* create per-dir config */
	NULL,					 /* merge per-dir config */
	tunnel_make_state,		 /* server config */
	NULL,					 /* merge server config */
	tunnel_cmds,			 /* command table */
	tunnel_handlers,		 /* handlers */
	NULL,					 /* filename translation */
	NULL,					 /* check_user_id */
	NULL,					 /* check auth */
	NULL,					 /* check access */
	tunnel_type_checker,	 /* type_checker */
	NULL,					 /* fixups */
	NULL,					 /* logger */
	NULL,					 /* header parser */
	NULL,                    /* child_init */
	tunnel_child_exit,		 /* process exit/cleanup */
	NULL					 /* [#0] post read-request */
};
