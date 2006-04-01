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

#ifndef PERCEPT_H
#define PERCEPT_H

#include "SDL.h"

#include <set>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"

enum Act {	// For Example
  ACT_NONE,
  ACT_START,	// Unit just arrived, or just became visible
  ACT_EQUIP,	// Unit needs to be (re)equipped
  ACT_DUCK,
  ACT_STAND,
  ACT_MAX
  };

struct UnitAct {
public:
  UnitAct(int i, int t, int xp, int yp, Act a, int t1 = 0, int t2 = 0)
        { id = i; time = t; x = xp; y = yp; act = a; targ1 = t1, targ2 = t2; };
  int id;
  Uint32 time;
  int x, y;	//X/Y position of the unit when it does this
  Act act;
  int targ1;    //Depending on action, may be a unit id, or x coord, or unused
  int targ2;    //Depending on action, may be a unit id, or y coord, or unused
  };

class Percept {
public:
  Percept();
  ~Percept();

  int Load(FILE *fl, unsigned int ver);
  int Save(FILE *fl, unsigned int ver);

  void Clear();

  set<int> my_units;		//List of all of my unit ids (doesn't change)
  vector<UnitAct> my_acts;	//List of unit actions of my own units
  vector<UnitAct> other_acts;	//List of unit actions of others' units
  };

#endif // PERCEPT_H

