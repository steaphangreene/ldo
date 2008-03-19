# *************************************************************************
#  This file is part of Life, Death, and the Objective ("LDO")
#  a simple squad-tactics strategy game by Steaphan Greene
#
#  Copyright 2005-2008 Steaphan Greene <stea@cs.binghamton.edu>
#
#  LDO is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  LDO is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with LDO (see the file named "COPYING");
#  If not, see <http://www.gnu.org/licenses/>.
#  
# *************************************************************************

ARCH=	$(shell gcc -v 2>&1 | grep Target | cut -f2 -d" ")
SSFLAGS=	$(shell $(ARCH)-simple-config --cflags)

.PHONY: all
all:	ldo.$(ARCH)

#Purge all default rules
.SUFFIXES:

CXX=	g++
LIBS=	$(shell $(ARCH)-simple-config --libs)

#PRODUCTION OPTIONS
CXXFLAGS=	-s -O2 -Wall $(SSFLAGS)

#DEGUGGING OPTIONS
debug:
	make CXXFLAGS='-g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE $(SSFLAGS)'

#PROFILING OPTIONS
prof:
	make CXXFLAGS'-pg -g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE $(SSFLAGS)'

OBJS:=	screens.$(ARCH).o percept.$(ARCH).o orders.$(ARCH).o \
	game.$(ARCH).o unit.$(ARCH).o player.$(ARCH).o main.$(ARCH).o \
	player_local.$(ARCH).o player_ai.$(ARCH).o world.$(ARCH).o

#PRODUCTION OPTIONS (CROSS-COMPILED FOR WINDOWS)
WARCH=	i586-mingw32msvc
WCXX=	i586-mingw32msvc-g++
WCXXFLAGS=	-s -O2 -Wall $(shell i586-mingw32msvc-simple-config --cflags)
WLIBS=	$(shell i586-mingw32msvc-simple-config --libs)
WARCH=	i586-mingw32msvc

%.h:	%.tga
	./scripts/tga2raw.csh $*
	./scripts/raw2header.csh $*

ChangeLog:      *.cpp *.h TODO COPYING Makefile .svn scripts/*
	./scripts/svn2cl.sh | sed 's-  stea-  sgreene-g' > ChangeLog

%.$(ARCH).o:	%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.$(WARCH).o:	%.cpp
	$(WCXX) $(WCXXFLAGS) -c $< -o $@

deps.$(ARCH).mk:	*.cpp *.h
	$(CXX) $(CXXFLAGS) -MM *.cpp | sed 's/\.o:/.$(ARCH).o:/' > $@

.PHONY: clean
clean:
	rm -f deps.*.mk *.o ldo.*

ldo.$(ARCH):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: win32
win32:	ldo.exe

WPFIX=	$(shell i586-mingw32msvc-simple-config --prefix)

ldo.exe:	*.cpp *.h $(WPFIX)/include/simple/*.h
	make ARCH=$(WARCH) CXX='$(WCXX)' FLAGS='$(WFLAGS)' LIBS='$(WLIBS)'
	cp -av ldo.$(WARCH) ldo.exe

TSTR:=	$(shell date -u +"%Y%m%d%H%M")
.PHONY: tar
backup:	tar
.PHONY: backup
tar:
	cd .. ; tar chvf ldo/ldo.$(TSTR).tar \
		ldo/Makefile ldo/TODO ldo/COPYING ldo/*.cpp ldo/*.h
	gzip -9 ldo.$(TSTR).tar

include deps.$(ARCH).mk
