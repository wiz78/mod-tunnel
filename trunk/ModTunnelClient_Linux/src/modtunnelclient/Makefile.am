# $Id: Makefile.am,v 1.2 2005-07-16 18:42:48 tellini Exp $

MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS		= modtunnelclient

CPPFLAGS 			= @CPPFLAGS@
CXXFLAGS 			= @CXXFLAGS@

INCLUDES        	= -I$(top_builddir) -I$(top_srcdir) \
				   	  -I$(top_builddir)/src -I$(top_srcdir)/src \
				  	  -I$(top_builddir)/src/include -I$(top_srcdir)/src/include

modtunnelclient_LDADD = ../libprom/libprom.la

modtunnelclient_SOURCES	= Client.cpp Config.cpp Daemon.cpp Main.cpp \
			  Resolver.cpp settings.cpp utils.cpp url.cpp 
