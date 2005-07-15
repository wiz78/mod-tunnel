/***************************************************************************
	revision             : $Id: settings.h,v 1.1.1.1 2005-07-15 13:54:06 tellini Exp $
    copyright            : (C) 2005 by Simone Tellini
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <ext/hash_map>
#include <string>

using namespace __gnu_cxx;
using namespace std;

namespace __gnu_cxx
{
	template<>
	struct hash<string>
	{
		size_t operator()( const string& s ) const
		{
			const collate<char>& c = use_facet<collate<char> >( locale::classic() );
			
			return c.hash( s.c_str(), s.c_str() + s.size() );
		}
	};
}
    
class Settings
{
public:
								Settings( const string &file );

	string						GetString( const string &section, const string &key, const string &def = "" );
	int							GetInt( const string &section, const string &key, int def = 0 );

private:
	hash_map<string, string>	Cfg;
};

#endif /* SETTINGS_H */
