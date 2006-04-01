// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2006 Steaphan Greene <stea@cs.binghamton.edu>
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

#include "orders.h"
#include "unit.h"
#include "defs.h"

Orders::Orders() {
  }

Orders::~Orders() {
  Clear();
  }

//static char buf[BUF_LEN];

int Orders::Load(FILE *fl, unsigned int ver) {
  //FIXME: Implement this!
  return 1;
  }

int Orders::Save(FILE *fl, unsigned int ver) {
  //FIXME: Implement this!
  return 1;
  }

void Orders::Clear() {
  orders.clear();
  }
