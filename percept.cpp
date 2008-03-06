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

int Percept::UnitPresent(int xc, int yc, int &id) {
  map<int, vector<UnitAct> >::iterator unit;
  unit = my_units.begin();
  for(; unit != my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->x == xc && act->y == yc) {
	id = act->id;
	return 1;
	}
      }
    }
  unit = other_units.begin();
  for(; unit != other_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->x == xc && act->y == yc) {
	id = act->id;
	return -1;
	}
      }
    }
  return 0;     //Nothing there
  }

int Percept::UnitAt(int xc, int yc) {
  map<int, vector<UnitAct> >::iterator unit;
  unit = my_units.begin();
  for(; unit != my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->x == xc && act->y == yc) {
	return act->id;
	}
      }
    }
  unit = other_units.begin();
  for(; unit != other_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->x == xc && act->y == yc) {
	return act->id;
	}
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

