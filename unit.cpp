// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2008 Steaphan Greene <stea@cs.binghamton.edu>
//
//  LDO is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  LDO is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with LDO (see the file named "COPYING");
//  If not, see <http://www.gnu.org/licenses/>.
//
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
  if(fscanf(f, "%d;%d;%[^\n;];\n", &id, &troop, buf) < 2) return 0;
  name = buf;
  return 1;
  }

int Unit::Save(FILE *f) {
  if(fprintf(f, "%d;%d;%s;\n", id, troop, name.c_str()) < 4) return 0;
  return 1;
  }
