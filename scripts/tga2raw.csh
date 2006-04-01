#!/bin/csh -f

# *************************************************************************
#  This file is part of Life, Death, and the Objective ("LDO")
#  a simple squad-tactics strategy game by Steaphan Greene
#
#  Copyright 2005-2006 Steaphan Greene <stea@cs.binghamton.edu>
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

if("$#argv" != "1") then
  echo "USAGE: $0 <basefontname>"
  exit
endif

if(!(-r "${1}.tga")) then
  echo "${1}.tga can not be read."
  exit
endif

set sz=`ls -l ${1}.tga | sed 's-  *-;-g' | cut -f5 -d';'`
@ sz -= 44

#echo "WARNING! DOESN'T UNDO RLE COMPRESSION OR HANDLE ORIGIN POSITION!"

tail -c +19 "${1}.tga" | head -c +$sz > "${1}.raw"
