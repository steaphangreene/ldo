# *************************************************************************
#  This file is part of Life, Death, and the Objective ("LDO")
#  a simple squad-tactics strategy game by Steaphan Greene
#
#  Copyright 2005-2008 Steaphan Greene <stea@cs.binghamton.edu>
#
#  LDO is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  LDO is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with LDO (see the file named "COPYING");
#  if not, write to the the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# *************************************************************************

ARCH=	$(shell gcc -v 2>&1 | grep Target | cut -f2 -d" ")

.PHONY: all
all:	ldo.$(ARCH)

#Purge all default rules
.SUFFIXES:

CXX=	g++
LIBS=	`simple-config --libs`

#PRODUCTION OPTIONS
#CXXFLAGS=	-s -O2 -Wall `simple-config --cflags`

#DEGUGGING OPTIONS
CXXFLAGS=	-g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE `simple-config --cflags`

OBJS:=	screens.$(ARCH).o percept.$(ARCH).o orders.$(ARCH).o \
	game.$(ARCH).o unit.$(ARCH).o player.$(ARCH).o main.$(ARCH).o \
	player_local.$(ARCH).o player_ai.$(ARCH).o world.$(ARCH).o

#PRODUCTION OPTIONS (CROSS-COMPILED FOR WINDOWS)
WARCH=	i586-mingw32msvc
WCXX=	i586-mingw32msvc-g++
WCXXFLAGS=	-s -O2 -Wall `i586-mingw32msvc-simple-config --cflags`
WLIBS=	`i586-mingw32msvc-simple-config --libs`
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
	rm -f deps.*.mk *.o ldo.* *.exe

ldo.$(ARCH):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: win32
win32:	ldo.exe

ldo.exe:	*.cpp *.h
	make ARCH=$(WARCH) CXX='$(WCXX)' FLAGS='$(WFLAGS)' LIBS='$(WLIBS)'
	mv -vf ldo.$(WARCH) ldo.exe

TSTR:=	$(shell date -u +"%Y%m%d%H%M")
.PHONY: tar
backup:	tar
.PHONY: backup
tar:
	cd .. ; tar chvf ldo/ldo.$(TSTR).tar \
		ldo/Makefile ldo/TODO ldo/COPYING ldo/*.cpp ldo/*.h
	gzip -9 ldo.$(TSTR).tar

include deps.$(ARCH).mk
