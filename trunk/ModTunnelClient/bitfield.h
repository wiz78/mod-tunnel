/***************************************************************************
                                  bitfield.h
                             -------------------
	revision             : $Id: bitfield.h,v 1.1.1.1 2004-09-13 15:11:42 tellini Exp $
    copyright            : (C) 2002-2004 by Simone Tellini
    email                : tellini@users.sourceforge.net

	description          : utility class to manipulate flags
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROM_BITFIELD_H
#define PROM_BITFIELD_H

class BitField
{
public:

					BitField() : Flags(0) {};

	BitField&		operator =( const int f )
    				{
                    	Flags = f;
                        
                        return( *this );
    				}

	void			Set( unsigned int f )
					{
						Flags |= f;
					}

	void			Set( unsigned int f, bool set )
					{
						if( set )
							Set( f );
						else
							Clear( f );
					}

	void			Clear( unsigned int f )
					{
						Flags &= ~f;
					}

	void			Clear( void )
					{
						Flags = 0;
					}

	bool			IsSet( unsigned int f ) const
					{
						return(( Flags & f ) ? true : false );
					}

    unsigned int	Get( void ) const
    				{
                    	return( Flags );
                    }

private:
	unsigned int	Flags;
};

#endif
