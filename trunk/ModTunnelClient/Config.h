/***************************************************************************
	revision             : $Id: Config.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
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

class Config
{
public:
	static Config&	GetInstance();

	int				GetPort() const				{ return( Port ); }
	int				GetProxyPort() const		{ return( ProxyPort ); }
	const char		*GetServer() const			{ return( Server ); }
	const char		*GetProxyHost() const		{ return( ProxyHost ); }
	const char		*GetProxyUser() const		{ return( ProxyUser ); }
	const char		*GetProxyPass() const		{ return( ProxyPass ); }

	bool			IsUsingProxy() const		{ return( ProxyHost[0] != '\0' ); }

private:
	char			Server[ 256 ];
	char			ProxyHost[ 256 ];
	char			ProxyUser[ 256 ];
	char			ProxyPass[ 256 ];
	int				Port;
	int				ProxyPort;

					Config();

	void			Load();
};

#endif CONFIG_H