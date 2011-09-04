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

#ifndef ORDERS_H
#define ORDERS_H

#include "SDL.h"

#include <map>
#include <cstdio>
using namespace std;

#include "unit.h"

enum Order {	// For Example
  ORDER_NONE,
  ORDER_EQUIP,	// Unit got (re)equipped
  ORDER_DUCK,
  ORDER_STAND,
  ORDER_MOVE,
  ORDER_RUN,
  ORDER_SHOOT,
  ORDER_THROW,
  ORDER_MAX
  };

struct UnitOrder {
public:
  UnitOrder(int i, int t, Order o, int t1 = 0, int t2 = 0, int t3 = 0)
	{ id = i; time = t; order = o; targ1 = t1, targ2 = t2; targ3 = t3; };
  int id;
  Uint32 time;
  Order order;
  int targ1;	//Depending on order, may be a unit id, or x coord, or unused
  int targ2;	//Depending on order, may be a unit id, or y coord, or unused
  int targ3;	//Depending on order, may be a unit id, or z coord, or unused

  bool operator < (const UnitOrder &other) const {
    if(time < other.time) return true;
    if(time > other.time) return false;
    if(id < other.id) return true;
    if(id > other.id) return false;
    if(order < other.order) return true;
    if(order > other.order) return false;
    if(targ1 < other.targ1) return true;
    if(targ1 > other.targ1) return false;
    if(targ2 < other.targ2) return true;
    if(targ2 > other.targ2) return false;
    if(targ3 < other.targ3) return true;
    return false;
    };
  };

class Orders {
public:
  Orders();
  ~Orders();

  int Load(FILE *fl, unsigned int ver);
  int Save(FILE *fl, unsigned int ver);

  void AddOrder(int i, int t, Order o, int t1 = 0, int t2 = 0, int t3 = 0) {
	orders[UnitOrder(i, t, o, t1, t2, t3)] = false;
	};


  void Completed(const UnitOrder &ord);
  void ClearCompleted();
  void Clear();

  map<UnitOrder, bool> orders;	//List of unit orders & Completed state
  };

#endif // ORDERS_H

