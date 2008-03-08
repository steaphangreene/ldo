// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2008 Steaphan Greene <stea@cs.binghamton.edu>
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

#include <map>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"

enum ObjectType {
  OBJECT_NONE,
  GROUND_FLOOR,
  WALL_EASTWEST,
  WALL_NORTHSOUTH,
  OBJECT_MISC,
  OBJECT_MAX
  };

struct MapObject {
  ObjectType type;
  int xpos, ypos, zpos;
  int which;
  };

enum Act {	// For Example
  ACT_NONE,
  ACT_FALL,
  ACT_START,	// Unit just arrived, or just became visible
  ACT_EQUIP,	// Unit needs to be (re)equipped
  ACT_DUCK,
  ACT_STAND,
  ACT_MOVE,
  ACT_RUN,
  ACT_SHOOT,
  ACT_THROW,
  ACT_MAX
  };

struct UnitAct {
public:
  UnitAct(int i, int t,
	int xp, int yp, int zp,
	Act a, int t1 = 0, int t2 = 0, int t3 = 0
	) {
    id = i; time = t;
    x = xp; y = yp; z = zp;
    act = a; targ1 = t1, targ2 = t2; targ3 = t3;
    };
  int id;
  Uint32 time;
  int x, y, z;	//X/Y/Z position of the unit when it does this
  Act act;
  int targ1;	//Depending on action, may be a unit id, or x coord, or unused
  int targ2;	//Depending on action, may be a unit id, or y coord, or unused
  int targ3;	//Depending on action, may be a unit id, or z coord, or unused
  };

struct Coord {
  int x, y, z;
  };

class Percept {
public:
  Percept();
  ~Percept();

  int Load(FILE *fl, unsigned int ver);
  int Save(FILE *fl, unsigned int ver);

  void Clear();

  //Basic map info
  int mapxs, mapys, mapzs;
  string mapname, mapdesc;

  //List of all of unit ids and actions for own and others
  map<int, vector<UnitAct> > my_units;
  map<int, vector<UnitAct> > other_units;
  vector<MapObject> objects;

  int UnitPresent(int xc, int yc, int &id);  // Enemy:-1, Own:1, None/Neutral:0
  int UnitAt(int xc, int yc);

  void GetPos(int id, int &x, int &y, int &z);
  vector<Coord> GetPath(const Coord &start, const Coord &end);
  vector<Coord> GetPath2x2(const Coord &start, const Coord &end);
  };

#endif // PERCEPT_H

