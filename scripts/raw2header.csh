#!/bin/csh -f

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

if("$#argv" != "1") then
  echo "USAGE: $0 <basefilename>"
  exit
endif

if(!(-r "${1}.raw")) then
  echo "${1}.raw can not be read."
  exit
endif

set fl=`echo "$0" | sed 's-raw2header.csh-hexdump_format-g'`

#echo "WARNING! DOESN'T WORK IF SIZE OF RAW IS NOT DIVISIBLE BY 8!"

echo '// *************************************************************************' > "${1}.h"
echo '//  This file was created by a data-conversion script written by' >> "${1}.h"
echo '//  Steaphan Greene <stea@cs.binghamton.edu>' >> "${1}.h"
echo '//' >> "${1}.h"
echo '//  I make no claim to ownership or copyright of this data or this file.' >> "${1}.h"
echo '//' >> "${1}.h"
echo '//  Anyone can feel free to do whatever they want with it.' >> "${1}.h"
echo '//' >> "${1}.h"
echo '//  This file is distributed in the hope that it will be useful,' >> "${1}.h"
echo '//  but WITHOUT ANY WARRANTY; without even the implied warranty of' >> "${1}.h"
echo '//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.' >> "${1}.h"
echo '// *************************************************************************' >> "${1}.h"
echo '' >> "${1}.h"

echo "static unsigned char $1[] = {" >> "${1}.h"
hexdump -vf "$fl" "${1}.raw" >> "${1}.h"
echo "~};" | tr '~' '\t' >> "${1}.h"
