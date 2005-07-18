/***************************************************************************
	revision             : $Id: Cfg.h,v 1.2 2005-07-18 09:01:14 tellini Exp $
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
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using namespace std;

class Config
{
public:
	static Config&		GetInstance();

	static void			SetCfgFile( const char *file );

	int					GetPort() const				{ return( Port ); }
	int					GetProxyPort() const		{ return( ProxyPort ); }
	const char			*GetServer() const			{ return( Server.c_str() ); }
	const char			*GetProxyHost() const		{ return( ProxyHost.c_str() ); }
	const char			*GetProxyUser() const		{ return( ProxyUser.c_str() ); }
	const char			*GetProxyPass() const		{ return( ProxyPass.c_str() ); }

	bool				IsUsingProxy() const		{ return( !ProxyHost.empty() ); }

private:
	string				CfgFile;
	string				Server;
	string				ProxyHost;
	string				ProxyUser;
	string				ProxyPass;
	int					Port;
	int					ProxyPort;
	
	static Config		*Instance;

						Config( const char *file = NULL );

	void				Load( void );
};

#endif /* CONFIG_H */
