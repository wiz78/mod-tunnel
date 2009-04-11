/***************************************************************************
	revision             : $Id: mod_tunnel.c,v 1.2 2009-04-11 17:26:41 tellini Exp $
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
#include <netdb.h>

#define CORE_PRIVATE

#include "mod_tunnel.h"

#define MOD_TUNNEL_VERSION_INFO_STRING	"mod_tunnel/2.0"
#define TUNNEL_HANDLER					"tunnel-handler"

static int tunnel_method;

module AP_MODULE_DECLARE_DATA tunnel_module;

// ------------------- UTILITY FUNCS -----------------

static const char *set_host( cmd_parms *parms, void *dummy, const char *arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Host = arg;

	return( NULL );
}

static const char *set_port( cmd_parms *parms, void *dummy, const char *arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Port = atoi( arg );

	return( NULL );
}

static const char *set_enabled( cmd_parms *parms, void *dummy, int arg )
{
	tunnel_state *cfg = (tunnel_state *) ap_get_module_config( parms->server->module_config, &tunnel_module );

	cfg->Enabled = arg;

	return( NULL );
}

static apr_socket_t *open_sock( tunnel_state *cfg, apr_pool_t *p )
{
	apr_sockaddr_t	*sa;
	apr_socket_t	*ret;

	if( apr_sockaddr_info_get( &sa, cfg->Host, APR_UNSPEC, cfg->Port, APR_IPV4_ADDR_OK, p ) == APR_SUCCESS ) {

		if( apr_socket_create( &ret, AF_INET, SOCK_STREAM, IPPROTO_TCP, p ) == APR_SUCCESS ) {

			if( apr_socket_connect( ret, sa ) != APR_SUCCESS ) {
			
				apr_socket_close( ret );
			
				ret = NULL;
			}
		}
	}
		
	return( ret );
}


// ------------------- HOOKS -----------------
	
/* Set up space for the various major configuration options */
static void *tunnel_make_state( apr_pool_t *p, server_rec *s )
{
	tunnel_state *cfg = ( tunnel_state * ) apr_pcalloc( p, sizeof( tunnel_state ));

	memset( cfg, 0, sizeof( *cfg ));

	return( cfg );
}

static int mod_tunnel_init( apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s )
{
	tunnel_method = ap_method_register( pconf, "TUNNEL" );

	ap_add_version_component( pconf, MOD_TUNNEL_VERSION_INFO_STRING );

	return( OK );
}

static int tunnel_handler( request_rec *r )
{
	tunnel_state	*cfg = ap_get_module_config( r->server->module_config, &tunnel_module );
	int				ret = DECLINED;

	if( cfg->Enabled ) {
		   
		if( r->method_number == tunnel_method ) {
			struct apr_socket_t *client = ap_get_module_config( r->connection->conn_config, &core_module );
			apr_pollset_t 		*pollset;
			apr_socket_t		*sock;

			ret = OK;

			// we're a tunnel
			r->output_filters             = NULL;
			r->connection->output_filters = NULL;

			if( apr_pollset_create( &pollset, 2, r->pool, 0 ) != APR_SUCCESS )
				return( HTTP_INTERNAL_SERVER_ERROR );

			if( sock = open_sock( cfg, r->pool )) {
				apr_size_t			len = 4;
				apr_pollfd_t 		pollfd;
				const apr_pollfd_t	*fds;
				apr_int32_t 		pollcnt;
				apr_status_t		status;

				apr_socket_send( client, "+OK\n", &len );

				pollfd.p           = r->pool;
				pollfd.desc_type   = APR_POLL_SOCKET;
				pollfd.reqevents   = APR_POLLIN;
				pollfd.desc.s      = client;
				pollfd.client_data = NULL;

				apr_pollset_add( pollset, &pollfd );

				pollfd.desc.s = sock;

				apr_pollset_add( pollset, &pollfd );

				status = apr_pollset_poll( pollset, r->server->timeout, &pollcnt, &fds );

				while( status == APR_SUCCESS ) {

					while(( status == APR_SUCCESS ) && ( pollcnt-- > 0 )) {
						char	buf[ 1024 * 64 ];

						len = sizeof( buf );

						if( fds->rtnevents & APR_POLLIN ) {

							if( fds->desc.s != sock ) {

								if(( status = apr_socket_recv( client, buf, &len )) == APR_SUCCESS )
									status = apr_socket_send( sock, buf, &len );
							
							} else if(( status = apr_socket_recv( sock, buf, &len )) == APR_SUCCESS )
								status = apr_socket_send( client, buf, &len );
						
						} else if( fds->rtnevents & ( APR_POLLERR | APR_POLLHUP ))
							status = APR_EOF;

						fds++;
					}

					if( status == APR_SUCCESS )
						status = apr_pollset_poll( pollset, r->server->timeout, &pollcnt, &fds );
				}

				apr_socket_close( sock );

			} else {
				char 		*err = apr_pstrcat( r->pool, "-ERROR: ", strerror( errno ), "\n", NULL );
				apr_size_t	len = strlen( err );

				apr_socket_send( client, err, &len );
			}
			
		} else
			ret = HTTP_FORBIDDEN;
	}

	return( ret );
}

static void tunnel_register_hooks( apr_pool_t *p )
{
	ap_hook_post_config( mod_tunnel_init, NULL, NULL, APR_HOOK_MIDDLE );
	ap_hook_handler( tunnel_handler, NULL, NULL, APR_HOOK_FIRST );
}

// ------------------- MOD CONFIG -----------------

/* Setup of the available httpd.conf configuration commands.
 * command, function called, NULL, where available, how many arguments, verbose description
 */
typedef const char *(*hook_func)();

static const command_rec tunnel_cmds[] = 
{
	AP_INIT_FLAG( "TunnelEnabled", set_enabled, NULL, RSRC_CONF,
				  "Enable the TUNNEL method." ),

	AP_INIT_TAKE1( "TunnelHost", set_host, NULL, RSRC_CONF,
	  			   "The host to connect to." ),

	AP_INIT_TAKE1( "TunnelPort", set_port, NULL, RSRC_CONF,
				   "The port to connect to." ),

	{ NULL }
};

/* The configuration array that sets up the hooks into the module. */
module AP_MODULE_DECLARE_DATA tunnel_module = 
{
	STANDARD20_MODULE_STUFF,
	NULL,					 /* create per-dir config */
	NULL,					 /* merge per-dir config */
	tunnel_make_state,		 /* server config */
	NULL,					 /* merge server config */
	tunnel_cmds,			 /* command table */
	tunnel_register_hooks,
};
