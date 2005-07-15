/***************************************************************************
	revision             : $Id: Config.cpp,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
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
#include "Cfg.h"
#include "settings.h"

#define INI_FILE_NAME ".ModTunnelClient.ini"

//---------------------------------------------------------------------------
Config::Config()
{
	Port = 22;

	Load();
}
//---------------------------------------------------------------------------
Config& Config::GetInstance()
{
	static Config *Instance = NULL;

	if( !Instance )
		Instance = new Config();

	return( *Instance );
}
//---------------------------------------------------------------------------
void Config::Load()
{
	string		file = string( getenv( "HOME" )) + "/"INI_FILE_NAME;
	Settings	s( file );

	Port      = s.GetInt( "Local", "Port", 22 );
	Server    = s.GetString( "Server", "URL" );
	ProxyPort = s.GetInt( "Proxy", "Port", 8080 );
	ProxyHost = s.GetString( "Proxy", "Host" );
	ProxyUser = s.GetString( "Proxy", "User" );
	ProxyPass = s.GetString( "Proxy", "Pass" );
}
//---------------------------------------------------------------------------
