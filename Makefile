# *************************************************************************
#  This file is part of the SimpleGUI Example Module by Steaphan Greene
#
#  Copyright 2005 Steaphan Greene <stea@cs.binghamton.edu>
#
#  SimpleGUI is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  SimpleGUI is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with SimpleGUI (see the file named "COPYING");
#  if not, write to the the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# *************************************************************************

all:	ldo

#PRODUCTION OPTIONS (STANDARD)
CXX=	gcc
CXXFLAGS=	-s -O2 -Wall `sdl-config --cflags`
LIBS=	`sdl-config --libs` -lSDL_ttf -lGL -lGLU

#PRODUCTION OPTIONS (WORKAROUND FOR CYGWIN)
#CXX=	gcc
#CXXFLAGS=	-s -O2 -Wall `sdl-config --cflags`
#LIBS=	`sdl-config --libs` -lSDL_ttf -L/usr/X11R6/bin -lopengl32 -lglu32

#DEGUGGING OPTIONS
#CXX=	gcc
#CXXFLAGS=	-g -Wall -DSDL_DEBUG=SDL_INIT_NOPARACHUTE `sdl-config --cflags`
#LIBS=	`sdl-config --libs` -lefence -lSDL_ttf -lGL -lGLU

OBJS:=	screens.o main.o renderer.o audio.o

SG_OBJS:=	../simplegui/sg.o ../simplegui/sg_widget.o \
	../simplegui/sg_texture.o ../simplegui/sg_alignment.o \
	../simplegui/sg_table.o ../simplegui/sg_panel.o \
	../simplegui/sg_textarea.o ../simplegui/sg_editable.o \
	../simplegui/sg_button.o ../simplegui/sg_stickybutton.o \
	../simplegui/sg_compound.o ../simplegui/sg_filebrowser.o \
	../simplegui/sg_multitab.o ../simplegui/sg_sliderbar.o \
	../simplegui/sg_passthrough.o ../simplegui/sg_translabel.o

#PRODUCTION OPTIONS (CROSS-COMPILED FOR WINDOWS)
WCXX=	i586-mingw32msvc-g++
WCXXFLAGS=	-s -O2 -Wall `/usr/i586-mingw32msvc/bin/i586-mingw32msvc-sdl-config --cflags`
WLIBS=	`/usr/i586-mingw32msvc/bin/i586-mingw32msvc-sdl-config --libs` -lSDL_ttf -lopengl32 -lglu32

WOBJS:=	screens.win32_o main.win32_o renderer.win32_o audio.win32_o

SG_WOBJS:=	../simplegui/sg.win32_o ../simplegui/sg_widget.win32_o \
	../simplegui/sg_texture.win32_o ../simplegui/sg_alignment.win32_o \
	../simplegui/sg_table.win32_o ../simplegui/sg_panel.win32_o \
	../simplegui/sg_textarea.win32_o ../simplegui/sg_editable.win32_o \
	../simplegui/sg_button.win32_o ../simplegui/sg_stickybutton.win32_o \
	../simplegui/sg_compound.win32_o ../simplegui/sg_filebrowser.win32_o \
	../simplegui/sg_multitab.win32_o ../simplegui/sg_sliderbar.win32_o \
	../simplegui/sg_passthrough.win32_o ../simplegui/sg_translabel.win32_o

all: $(OBJS)

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.win32_o:	%.cpp %.o
	$(WCXX) $(WCXXFLAGS) -c $< -o $@

deps.mk:	*.cpp *.h
	$(CXX) $(CXXFLAGS) -MM *.cpp > deps.mk

clean:
	rm -f deps.mk *.o ldo *.win32_o *.exe

ldo:	$(OBJS)
	$(CXX) $(CXXFLAGS) -o ldo $(OBJS) $(SG_OBJS) $(LIBS)

win32:	$(WOBJS)

ldo.exe:	$(WOBJS)
	$(CXX) $(CXXFLAGS) -o ldo.exe $(WOBJS) $(SG_WOBJS) $(LIBS)

TSTR:=	$(shell date -u +"%Y%m%d%H%M")
backup:	tar
tar:
	cd .. ; tar chvf ldo/ldo.$(TSTR).tar \
		ldo/Makefile ldo/TODO ldo/COPYING ldo/*.cpp ldo/*.h \
		ldo/deps.mk
	gzip -9 ldo.$(TSTR).tar

include deps.mk
