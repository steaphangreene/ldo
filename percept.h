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

#ifndef PERCEPT_H
#define PERCEPT_H

#include "SDL.h"

#include <set>
#include <map>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"

#define CELL_HEIGHT 3.0

enum ObjectType {
  OBJECT_NONE,
  GROUND_FLOOR,
  WALL_NORTH,
  WALL_WEST,
  OBJECT_MISC,
  OBJECT_MAX
};

struct MapCoord {
  int x, y, z;
  // These are so set<> and map<> can sort by these, and for basic comparisons.
  bool operator<(const MapCoord &b) const {
    return (z < b.z || (z == b.z && (y < b.y || (y == b.y && x < b.x))));
  }
  bool operator>(const MapCoord &b) const {
    return (z > b.z || (z == b.z && (y > b.y || (y == b.y && x > b.x))));
  }
  bool operator!=(const MapCoord &b) const {
    return (z != b.z || y != b.y || x != b.x);
  }
  bool operator==(const MapCoord &b) const {
    return (z == b.z && y == b.y && x == b.x);
  }
};

struct MapObject {
  ObjectType type;
  int id;
  int which;
  float height;
  map<int, multimap<Uint32, Uint32> > seen;

  map<Uint32, Uint8> burn;
  map<int, int> seers;
  void See(int plnum, Uint32 tm);
  void Unsee(int plnum, Uint32 tm);
};

enum Act {  // For Example
  ACT_NONE,
  ACT_FALL,
  ACT_START,  // Unit just arrived, or just became visible
  ACT_EQUIP,  // Unit needs to be (re)equipped
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
  UnitAct(int i, Uint32 f, Uint32 d, int xp, int yp, int zp, float ang, Act a,
          int t1 = 0, int t2 = 0, int t3 = 0) {
    id = i;
    finish = f;
    duration = d;
    x = xp;
    y = yp;
    z = zp;
    angle = ang;
    act = a;
    targ1 = t1, targ2 = t2;
    targ3 = t3;
  };
  int id;
  Uint32 finish, duration;
  int x, y, z;  // X/Y/Z position of the unit when it does this
  float angle;
  Act act;
  int targ1;  // Depending on action, may be a unit id, or x coord, or unused
  int targ2;  // Depending on action, may be a unit id, or y coord, or unused
  int targ3;  // Depending on action, may be a unit id, or z coord, or unused

  bool operator<(const UnitAct &other) const {
    return (finish < other.finish ||
            (finish == other.finish && duration < other.duration));
  }
};

class Percept {
 public:
  Percept();
  ~Percept();

  int Load(FILE *fl, unsigned int ver);
  int Save(FILE *fl, unsigned int ver);

  void Clear();

  void FillActionsTo(int id, Uint32 f);
  void AddAction(int i, Uint32 f, Uint32 d, int xp, int yp, int zp, float ang,
                 Act a, int t1 = 0, int t2 = 0, int t3 = 0);
  void AddAction(const UnitAct &act);

  // Basic map info
  int mapxs, mapys, mapzs;
  string mapname, mapdesc;
  unsigned int round;  // Current To-Be-Declared Game Round

  // List of all of unit ids and actions for own and others
  map<int, vector<UnitAct> > my_units;
  map<int, vector<UnitAct> > other_units;
  multimap<MapCoord, MapObject> objects;

  // Enemy:-1, Own:1, None/Neutral:0
  int UnitPresent(int xc, int yc, int zc, int &id);
  int UnitAt(int xc, int yc, int zc);
  int PlayerIDForUnit(const int unitid) { return unplayer[unitid]; };
  map<int, int> unplayer;  // Lookup PlayerID by UnitID

  float HeightAt(const MapCoord &pos);
  void GetPos(int id, int &x, int &y, int &z);
  void GetPos(int id, int &x, int &y, int &z, float &a);
  vector<MapCoord> GetPath(const MapCoord &start, const MapCoord &end);
  vector<MapCoord> GetPath2x2(const MapCoord &start, const MapCoord &end);

 private:  // Utility Functions & Cache Data
  int RDist(const MapCoord &first, const MapCoord &second);
  int HDist(const MapCoord &first, const MapCoord &second);
  void See(int id, Uint32 tm, int xp, int yp, int zp, float ang, int dist,
           float fov);
  map<int, set<MapObject *> > lastseen;
};

#endif  // PERCEPT_H
