/***************************************************************************
                                   url.cpp
                             -------------------
    revision             : $Id: url.cpp,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

    description          : URL parser
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>

#include "url.h"

//---------------------------------------------------------------------------
URL::URL()
{
	Clear();
}
//---------------------------------------------------------------------------
URL::URL( const char *string )
{
	Decode( string );
}
//---------------------------------------------------------------------------
void URL::Clear( void )
{
	Scheme     = "";
	Host       = "";
	User       = "";
	Password   = "";
	Rest       = "/";
	HostPort   = "";
	EncodedURL = "";
	Port       = 80;
}
//---------------------------------------------------------------------------
void URL::Decode( const char *string )
{
	char 	*temp;

	Clear();

	if( !string || ( *string == '\0' ))
		return;

	if( temp = strdup( string )) {
		char   *string2 = temp, *ptr, *host = temp;

		if( ptr = strchr( string2, ' ' ))
			*ptr = '\0';

		// scheme?
		if(( ptr = strchr( string2, ':' )) && ( strncmp( ptr + 1, "//", 2 ) == 0 )) {

			*ptr   = '\0';
			Scheme = string2;
			host   = string2 = ptr + 3;

			if( Scheme == "https" )
				Port = 443;
		}

		// rest?
		if( ptr = strchr( string2, '/' )) {

			*ptr++ = '\0';

			if( *ptr )
				Rest += ptr;
		}

		// user?
		if( ptr = strchr( string2, '@' )) {

			*ptr = '\0';
			host = ptr + 1;

			if( ptr = strchr( string2, ':' )) {
				*ptr++   = '\0';
				Password = ptr;
			}

			User = string2;
		}

		if( host ) {
			
			// port?
			if( ptr = strchr( host, ':' )) {
				*ptr = '\0';
				Port = atoi( ptr + 1 );
			}

			Host = host;
		}

		free( temp );
	}
}
//---------------------------------------------------------------------------
const char *URL::Encode( void )
{
	EncodedURL = "";

	if( !Host.empty() ) {

		if( !Scheme.empty() )
			EncodedURL += Scheme + "://";

		if( !User.empty() ) {

			EncodedURL += User;

			if( !Password.empty() )
				EncodedURL += ":" + Password;

			EncodedURL += "@";
		}

		EncodedURL += Host;

		if( Port ) {
			char	buf[ 8 ];

			sprintf( buf, ":%d", Port );

			EncodedURL += buf;
		}
	}

	EncodedURL += Rest;

	return( EncodedURL.c_str() );
}
//---------------------------------------------------------------------------
const char *URL::GetHostPort( void )
{
	char	buf[ 8 ];

	sprintf( buf, ":%d", Port );

	HostPort = Host + buf;

	return( HostPort.c_str() );
}
//---------------------------------------------------------------------------
