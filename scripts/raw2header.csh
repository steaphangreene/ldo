#!/bin/csh -f

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

echo "static unsigned char $1[] = {" > "${1}.h"
hexdump -vf "$fl" "${1}.raw" >> "${1}.h"
echo "~};" | tr '~' '\t' >> "${1}.h"
