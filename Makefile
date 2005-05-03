# *************************************************************************
#  This file is part of Life, Death, and the Objective ("LDO")
#  a simple squad-tactics strategy game by Steaphan Greene
#
#  Copyright 2005 Steaphan Greene <stea@cs.binghamton.edu>
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

all:	ldo
win32:	ldo.exe

#PRODUCTION OPTIONS (STANDARD)
#CXX=	g++
#CXXFLAGS=	-s -O2 -Wall `sdl-config --cflags`
#LIBS=	../simplegui/libsimplegui.a ../simplemodel/libsimplemodel.a ../simplevideo/libsimplevideo.a ../simpleaudio/libsimpleaudio.a ../simpleconnect/libsimpleconnect.a `sdl-config --libs` -lSDL_net -lSDL_ttf -lSDL_image -lSDL_mixer -lGL -lGLU

#PRODUCTION OPTIONS (WORKAROUND FOR MacOS-X)
#CXX=	g++
#CXXFLAGS=	-s -O2 -Wall `sdl-config --cflags`
#LIBS=	../simplegui/libsimplegui.a ../simplemodel/libsimplemodel.a ../simplevideo/libsimplevideo.a ../simpleaudio/libsimpleaudio.a ../simpleconnect/libsimpleconnect.a `sdl-config --libs` -lSDL_net -lSDL_ttf -lSDL_image -lSDL_mixer -framework OpenGL

#PRODUCTION OPTIONS (WORKAROUND FOR CYGWIN)
#CXX=	g++
#CXXFLAGS=	-s -O2 -Wall `sdl-config --cflags`
#LIBS=	../simplegui/libsimplegui.a ../simplemodel/libsimplemodel.a ../simplevideo/libsimplevideo.a ../simpleaudio/libsimpleaudio.a ../simpleconnect/libsimpleconnect.a `sdl-config --libs` -lSDL_net -lSDL_ttf -lSDL_image -lSDL_mixer -L/usr/X11R6/bin -lopengl32 -lglu32

#DEGUGGING OPTIONS (NO EFENCE)
CXX=	g++
CXXFLAGS=	-g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE `sdl-config --cflags`
LIBS=	../simplegui/libsimplegui.a ../simplemodel/libsimplemodel.a ../simplevideo/libsimplevideo.a ../simpleaudio/libsimpleaudio.a ../simpleconnect/libsimpleconnect.a `sdl-config --libs` -lSDL_net -lSDL_ttf -lSDL_image -lSDL_mixer -lGL -lGLU

#DEGUGGING OPTIONS (WITH EFENCE)
#CXX=	g++
#CXXFLAGS=	-g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE `sdl-config --cflags`
#LIBS=	../simplegui/libsimplegui.a ../simplemodel/libsimplemodel.a ../simplevideo/libsimplevideo.a ../simpleaudio/libsimpleaudio.a ../simpleconnect/libsimpleconnect.a `sdl-config --libs` -lefence -lSDL_net -lSDL_ttf -lSDL_image -lSDL_mixer -lGL -lGLU

OBJS:=	screens.o percept.o orders.o world.o \
	game.o unit.o player.o player_local.o \
	main.o

#PRODUCTION OPTIONS (CROSS-COMPILED FOR WINDOWS)
WCXX=	i586-mingw32msvc-g++
WCXXFLAGS=	-s -O2 -Wall `/usr/i586-mingw32msvc/bin/i586-mingw32msvc-sdl-config --cflags`
WLIBS=	../simplegui/libsimplegui.win32_a ../simplemodel/libsimplemodel.win32_a ../simplevideo/libsimplevideo.win32_a ../simpleaudio/libsimpleaudio.win32_a ../simpleconnect/libsimpleconnect.win32_a `/usr/i586-mingw32msvc/bin/i586-mingw32msvc-sdl-config --libs` -lSDL_net -lwsock32 -lSDL_ttf -lSDL_image -lSDL_mixer -lSDL -lpng -ljpeg -lz -lpng -lopengl32 -lglu32

WOBJS:=	screens.win32_o percept.win32_o orders.win32_o world.win32_o \
	game.win32_o unit.win32_o player.win32_o player_local.win32_o \
	main.win32_o

%.h:	%.tga
	./scripts/tga2raw.csh $*
	./scripts/raw2header.csh $*

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.win32_o:	%.cpp %.o
	$(WCXX) $(WCXXFLAGS) -c $< -o $@

deps.mk:	*.cpp *.h
	$(CXX) $(CXXFLAGS) -MM *.cpp > deps.mk

clean:
	rm -f deps.mk *.o ldo *.win32_o *.exe

ldo:	$(OBJS)
	$(CXX) $(CXXFLAGS) -o ldo $(OBJS) $(LIBS)

win32:	$(WOBJS)

ldo.exe:	$(WOBJS)
	$(WCXX) $(WCXXFLAGS) -o ldo.exe $(WOBJS) $(WLIBS)

TSTR:=	$(shell date -u +"%Y%m%d%H%M")
backup:	tar
tar:
	cd .. ; tar chvf ldo/ldo.$(TSTR).tar \
		ldo/Makefile ldo/TODO ldo/COPYING ldo/*.cpp ldo/*.h \
		ldo/deps.mk
	gzip -9 ldo.$(TSTR).tar

include deps.mk
