/***************************************************************************
	revision             : $Id: Config.cpp,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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
#include <windows.h>

#include "config.h"

#define INI_FILE_NAME "ModTunnelClient.ini"

//---------------------------------------------------------------------------
Config::Config()
{
	Server[0] = '\0';
	Port      = 22;

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
	char	IniFile[ MAX_PATH ];

	GetModuleFileName( NULL, IniFile, sizeof( IniFile ));

	strrchr( IniFile, '\\' )[1] = '\0';
	strncat( IniFile, INI_FILE_NAME, sizeof( IniFile ));

	Port = GetPrivateProfileInt( "Local", "Port", 22, IniFile );

	GetPrivateProfileString( "Server", "URL", "", 
                             Server, sizeof( Server ), IniFile );

	ProxyPort = GetPrivateProfileInt( "Proxy", "Port", 8080, IniFile );

	GetPrivateProfileString( "Proxy", "Host", "", 
                             ProxyHost, sizeof( ProxyHost ), IniFile );

	GetPrivateProfileString( "Proxy", "User", "", 
                             ProxyUser, sizeof( ProxyUser ), IniFile );

	GetPrivateProfileString( "Proxy", "Pass", "", 
                             ProxyPass, sizeof( ProxyPass ), IniFile );
}
//---------------------------------------------------------------------------
