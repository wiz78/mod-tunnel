/***************************************************************************
	revision             : $Id: settings.cpp,v 1.2 2005-07-16 17:56:14 tellini Exp $
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

#include "main.h"
#include "settings.h"
#include "utils.h"

#include <fstream>

//---------------------------------------------------------------------------
Settings::Settings( const string &file )
{
	ifstream	fh( file.c_str() );
	
	if( fh ) {
	
		fh.exceptions( ifstream::badbit );
		
		try {
			int 	ln = 1;
			string	section;

			while( !fh.eof() ) {
				string line;
				
				getline( fh, line );

				line = Utils::Trim( line );
				
				if( line[0] == '[' ) {

					section = line.substr( 1, line.length() - 2 );
						
				} else if( !line.empty() && ( line[0] != '#' ) && ( line[0] != ';' )) {
					string::size_type pos = line.find( "=" );
					
					if( pos == string::npos )
						fprintf( stderr, "Configuration error at line %d: no value specified\n", ln );
					else {
						string key = Utils::Trim( line.substr( 0, pos ));
						string val = Utils::Trim( line.substr( pos + 1 ));

						Cfg[ section + "/" + key ] = val;
					}
				}
				
				ln++;
			}
		}
		catch( ifstream::failure& e ) {
			fprintf( stderr, "Error while reading the configuration: %s\n", e.what() );
		}

		fh.close();
		
	} else
		fprintf( stderr, "Configuration file not found!\n" );
}
//---------------------------------------------------------------------------
string Settings::GetString( string const& section, string const& key, const string& def )
{
	string ret = def;

	if( Cfg.find( section + "/" + key ) != Cfg.end() )
		ret = Cfg[ section + "/" + key ];

	return( ret );
}
//---------------------------------------------------------------------------
int Settings::GetInt( string const& section, string const& key, int def )
{
	if( Cfg.find( section + "/" + key ) != Cfg.end() )
		def = atoi( Cfg[ section + "/" + key ].c_str() );
		
	return( def );
}
//---------------------------------------------------------------------------

