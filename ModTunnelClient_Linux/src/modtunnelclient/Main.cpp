/* $Id: Main.cpp,v 1.2 2005-07-18 09:01:14 tellini Exp $ */
//---------------------------------------------------------------------------

#include "Daemon.h"

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int main( int argc, const char *argv[] )
{
	Daemon	*daemon = new Daemon( argc, argv );
		
	daemon->Run();
	
	delete daemon;

    return( 0 );
}
//---------------------------------------------------------------------------