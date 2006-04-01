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

#ifndef ORDERS_H
#define ORDERS_H

#include "SDL.h"

#include <set>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"

enum Order {	// For Example
  ORDER_NONE,
  ORDER_EQUIP,	// Unit got (re)equipped
  ORDER_DUCK,
  ORDER_STAND,
  ORDER_MAX
  };

struct UnitOrder {
public:
  UnitOrder(int i, int t, Order o, int t1 = 0, int t2 = 0)
	{ id = i; time = t; order = o; targ1 = t1, targ2 = t2; };
  int id;
  Uint32 time;
  Order order;
  int targ1;	//Depending on order, may be a unit id, or x coord, or unused
  int targ2;	//Depending on order, may be a unit id, or y coord, or unused
  };

class Orders {
public:
  Orders();
  ~Orders();

  int Load(FILE *fl, unsigned int ver);
  int Save(FILE *fl, unsigned int ver);

  void Clear();

  vector<UnitOrder> orders;	//List of unit orders
  };

#endif // ORDERS_H

