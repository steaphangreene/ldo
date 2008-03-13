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

#include "percept.h"
#include "unit.h"
#include "defs.h"

#include <set>
#include <cmath>
using namespace std;

Percept::Percept() {
  }

Percept::~Percept() {
  Clear();
  my_units.clear();
  }

//static char buf[BUF_LEN];

int Percept::Load(FILE *fl, unsigned int ver) {
  unsigned int num, val;

  if(fscanf(fl, "%d\n", &num) < 1) return 0;
  for(unsigned int unit = 0; unit < num; ++unit) {
    if(fscanf(fl, "%d;", &val) < 1) return 0;
//    my_units.insert(val);
    fscanf(fl, "\n");
    }

  return 1;
  }

int Percept::Save(FILE *fl, unsigned int ver) {
  if(fprintf(fl, "%d\n", (int)(my_units.size())) < 2) return 0;
  map<int, vector<UnitAct> >::iterator itrm = my_units.begin();
  for(; itrm != my_units.end(); ++itrm) {
    if(fprintf(fl, "%d\n", itrm->first) < 2) return 0;
    }

  return 1;
  }

void Percept::Clear() {	//Prepares for next frame - DOES NOT CLEAR my_units
  my_units.clear();
  other_units.clear();
  }

int Percept::UnitPresent(int xc, int yc, int zc, int &id) {
  map<int, vector<UnitAct> >::iterator unit;
  unit = my_units.begin();
  for(; unit != my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.end();
    --act;
    if(act->x == xc && act->y == yc && act->z == zc) {
      id = act->id;
      return 1;
      }
    }
  unit = other_units.begin();
  for(; unit != other_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.end();
    --act;
    if(act->x == xc && act->y == yc && act->z == zc) {
      id = act->id;
      return -1;
      }
    }
  return 0;     //Nothing there
  }

int Percept::UnitAt(int xc, int yc, int zc) {
  map<int, vector<UnitAct> >::iterator unit;
  unit = my_units.begin();
  for(; unit != my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.end();
    --act;
    if(act->x == xc && act->y == yc && act->z == zc) {
      return act->id;
      }
    }
  unit = other_units.begin();
  for(; unit != other_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    --act;
    if(act->x == xc && act->y == yc && act->z == zc) {
      return act->id;
      }
    }
  return 0;
  }

void Percept::GetPos(int id, int &x, int &y, int &z) {
  vector<UnitAct>::const_iterator last;
  if(my_units.count(id) > 0) {
    last = my_units[id].end();
    for(; last != my_units[id].begin(); ++last) {
      last--;
      x = last->x;
      y = last->y;
      z = last->z;
      return;
      }
    }
  else if (other_units.count(id) > 0) {
    last = other_units[id].end();
    for(; last != other_units[id].begin(); ++last) {
      last--;
      x = last->x;
      y = last->y;
      z = last->z;
      return;
      }
    }
  }

float Percept::HeightAt(const MapCoord &pos) {
  float height = 0.0;
  multimap<MapCoord, MapObject>::const_iterator obj = objects.find(pos);
  if(obj != objects.end()) {
    for(; obj != objects.upper_bound(pos); ++obj) {
      if(obj->second.type == OBJECT_MISC) {
	height = obj->second.height;
	}
      }
    }
  return height;
  }

int Percept::RDist(const MapCoord &start, const MapCoord &end) {
  int ret = 0;
  int dx = end.x - start.x;
  int dy = end.y - start.y;
  int dz = end.z - start.z;
  if(dx == 0 && dy == 0 && dz == 0) {
    ret = -1;		// No self-links
    }
  else if(dx > 1 || dx < -1 || dy > 1 || dy < -1 || dz > 1 || dz < -1) {
    ret = -1;		// Not Adjacent
    }
  else {	// Need Floors, Can't go through walls, Can't climb tall objects
    float height, height2 = 0.0;
    height = HeightAt(start);
    multimap<MapCoord, MapObject>::const_iterator obj = objects.find(end);
    if(obj != objects.end()) {
      for(; ret >= 0 && obj != objects.upper_bound(end); ++obj) {
	if(obj->second.type == GROUND_FLOOR && obj->second.height == 0.0) {
	  ret = HDist(start, end);
	  }
	else if(obj->second.type == OBJECT_MISC) {
	  height2 = obj->second.height;
	  }
	}
      }
    if(dz > 0) {	// Moving Up
      height2 += CELL_HEIGHT;
      }
    else if(dz < 0) {	// Moving Down
      height += CELL_HEIGHT;
      }
    if(fabsf(height2 - height) > 1.0) {
//      fprintf(stderr, "(%d,%d,%d)->(%d,%d,%d) is too big a step (%f->%f)\n",
//	start.x, start.y, start.z, end.x, end.y, end.z, height, height2 );
      ret = -1;	// Too big a step
      }
    if(ret > 0 && (dy == -1 || dy == 1)) {		// Moving North/South
      MapCoord tmp = { start.x, start.y, start.z };
      if(dy == 1) { ++(tmp.y); }			// Moving South
      obj = objects.find(tmp);
      if(obj != objects.end()) {
	for(; obj != objects.upper_bound(tmp); ++obj) {
	  if(obj->second.type == WALL_NORTH && obj->second.height > 0.0) {
	    ret = -1;
	    }
	  }
	}
      tmp.x += dx;
      obj = objects.find(tmp);
      if(dx != 0 && obj != objects.end()) {
	for(; obj != objects.upper_bound(tmp); ++obj) {
	  if(obj->second.type == WALL_NORTH && obj->second.height > 0.0) {
	    ret = -1;
	    }
	  }
	}
      }
    if(ret > 0 && (dx == -1 || dx == 1)) {		// Moving West/East
      MapCoord tmp = { start.x, start.y, start.z };
      if(dx == 1) { ++(tmp.x); }			// Moving East
      obj = objects.find(tmp);
      if(obj != objects.end()) {
	for(; obj != objects.upper_bound(tmp); ++obj) {
	  if(obj->second.type == WALL_WEST && obj->second.height > 0.0) {
	    ret = -1;
	    }
	  }
	}
      tmp.y += dy;
      obj = objects.find(tmp);
      if(dy != 0 && obj != objects.end()) {
	for(; obj != objects.upper_bound(tmp); ++obj) {
	  if(obj->second.type == WALL_WEST && obj->second.height > 0.0) {
	    ret = -1;
	    }
	  }
	}
      }
    }
  return ret;
  }

int Percept::HDist(const MapCoord &start, const MapCoord &end) {
  int xd = (end.x - start.x)*128;
  int yd = (end.y - start.y)*128;
  int zd = (end.z - start.z)*128;
  return int(sqrt(xd*xd + yd*yd + zd*zd));
  }

vector<MapCoord> Percept::GetPath(const MapCoord &start, const MapCoord &end) {
  set<MapCoord> closed;
  map<MapCoord, int> open;
  map<MapCoord, MapCoord> prev;
  map<MapCoord, int> gdist;
  multimap<int, MapCoord> openlist;

  prev[start] = start;
  gdist[start] = 0;
  open[start] = HDist(start, end);
  openlist.insert(pair<int, MapCoord>(open[start], start));
  while(openlist.size() > 0 && closed.count(openlist.begin()->second) > 0) {
    openlist.erase(openlist.begin());
    }
  while(closed.count(end) == 0 && openlist.size() > 0) {
    MapCoord cur = openlist.begin()->second;
    openlist.erase(openlist.begin());
    open.erase(cur);
    closed.insert(cur);
    if(cur.x != end.x || cur.y != end.y || cur.z != end.z) {
      for(int zo = -1; zo < 2; zo++) {
	for(int yo = -1; yo < 2; yo++) {
	  for(int xo = -1; xo < 2; xo++) {
	    MapCoord tmp = { cur.x + xo, cur.y + yo, cur.z + zo };
	    if(tmp.z >= 0 && tmp.z < mapzs && tmp.y >= 0
			&& tmp.y < mapys && tmp.x >= 0 && tmp.x < mapxs
			&& closed.count(tmp) == 0) {
	      int rd = RDist(cur, tmp);
	      if(rd > 0) {
		int gd = gdist[cur] + rd;

		//Check and optimize for straighter paths
		map<int, int> dirs;
		int pdir = 4 * (1 + tmp.y - cur.y) + (1 + tmp.x - cur.x);
		dirs[pdir] = 0;
		MapCoord test = cur;
		while(test != start && tmp.z == cur.z) {// FIXME: Z Straight?
		  if(test.z != prev[test].z) break;	// FIXME: Z Straight?
		  int dir = 4 * (1 + test.y - prev[test].y)
				+ (1 + test.x - prev[test].x);
		  if(dir == pdir) dirs[dir] = 1;	//Twice+ in a row
		  else dirs[dir] = dirs[dir];		//Creates if missing

		  if(dirs.size() > 2) break;	//Too many dirs - Not straight
		  if(dirs.size() == 2) {
		    if(dirs.begin()->second > 0
			&& dirs.rbegin()->second > 0) {	// To Jagged
		      break;		//Not straight
		      }
		    int sep = abs(dirs.begin()->first - dirs.rbegin()->first);
		    if(sep != 1 && sep != 4) {		// Turns too Sharply
		      break;		//Not straight
		      }
		    }
		  test = prev[test];
		  pdir = dir;
		  }
		int td = gdist[test] + HDist(test, tmp) + HDist(tmp, end);

		if(open.count(tmp) == 0 || open[tmp] > td) {
// FIXME: How do I do this right?
//		  if(open.count(tmp) > 0) {
//		    openlist.erase(pair<int, MapCoord>(open[tmp], tmp));
//		    }
		  gdist[tmp] = gd;
		  open[tmp] = td;
		  prev[tmp] = cur;
		  openlist.insert(pair<int, MapCoord>(open[tmp], tmp));
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  vector<MapCoord> ret;
  if(closed.count(end) > 0) {	// Only return a path if I can get there
    MapCoord cur = end;
    while(prev[cur] != cur) {
      ret.push_back(cur);
      cur = prev[cur];
      }
    ret.push_back(cur);
    reverse(ret.begin(), ret.end());
    }
  return ret;
  }

vector<MapCoord> Percept::GetPath2x2(const MapCoord &start, const MapCoord &end) {
  vector<MapCoord> ret;
  MapCoord cur = start;
  ret.push_back(cur);
  while(cur.x != end.x || cur.y != end.y || cur.z != end.z) {
    if(cur.x < end.x) cur.x ++;
    else if(cur.x > end.x) cur.x --;
    if(cur.y < end.y) cur.y ++;
    else if(cur.y > end.y) cur.y --;
    if(cur.z < end.z) cur.z ++;
    else if(cur.z > end.z) cur.z --;
    ret.push_back(cur);
    }
  return ret;
  }

#define VDIST 20
void Percept::AddAction(int id, int time, int xp, int yp, int zp,
	Act a, int t1, int t2, int t3) {
  UnitAct act(id, time, xp, yp, zp, a, t1, t2, t3);
  my_units[id].push_back(act);

  //Handle Vision/Discovery	//FIXME: This is static (no obstructions yet)
  for(int z = -VDIST; z < +VDIST; ++z) {
    for(int y = -VDIST; y < +VDIST; ++y) {
      for(int x = -VDIST; x < +VDIST; ++x) {
	if((z+zp) >= 0 && (z+zp) < mapzs && (y+yp) >= 0 && (y+yp) < mapys
		&& (x+xp) >= 0 && (x+xp) < mapxs
		&& sqrt(z*z + y*y + x*x) <= VDIST) {
	  MapCoord pos = { x+xp, y+yp, z+zp };
	  multimap<MapCoord, MapObject>::iterator obj = objects.find(pos);
	  if(obj != objects.end()) {
	    for(; obj != objects.upper_bound(pos); ++obj) {
		// FIXME: Should be act's END time, not START time
	      obj->second.last_seen[unplayer[id]] = time;
	      }
	    }
	  }
	}
      }
    }
  }
