// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005 Steaphan Greene <stea@cs.binghamton.edu>
//
//  LDO is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  LDO is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with LDO (see the file named "COPYING");
//  if not, write to the the Free Software Foundation, Inc.,
//  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// *************************************************************************

#include <cstdio>
#include <cstring>
using namespace std;

#include "unit.h"
#include "defs.h"

Unit::Unit() {
  }

static char buf[BUF_LEN];

int Unit::Load(FILE *f, unsigned int ver) {
  memset(buf, 0, BUF_LEN);
  if(fscanf(f, "%d;%[^\n;];\n", &id, buf) < 2) return 0;
  name = buf;
  return 1;
  }

int Unit::Save(FILE *f) {
  if(fprintf(f, "%d;%s;\n", id, name.c_str()) < 4) return 0;
  return 1;
  }
