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

#include "map.h"
#include "unit.h"
#include "defs.h"

#define HEADER_STRING "LDO_GAMESAVE_FILE"
#define SAVEFILE_VERSION	0x00000002 // 0.0.0-r2

Map::Map() {
  }

Map::~Map() {
  Clear();
  }

static char buf[BUF_LEN];

int Map::Load(const string &filename) {
  FILE *fl = fopen(filename.c_str(), "r");
  if(!fl) return 0;

  Clear();
  int ret = Load(fl);

  fclose(fl);
  return ret;
  }

int Map::Load(FILE *fl) {
  unsigned int num;

  memset(buf, 0, BUF_LEN);
  if(fscanf(fl, "%s\n", buf) < 1) return 0;
  if(strcmp(buf, HEADER_STRING)) return 0;

  if(fscanf(fl, "0x%X\n", &num) < 1) return 0;
  if(num != SAVEFILE_VERSION) return 0;

  if(fscanf(fl, "%d\n", &num) < 1) return 0;

  plunits.resize(1);	//Temporary!  One player!

  Unit *unit;
  for(int n = 0; n < (int)(num); ++n) {
    unit = new Unit;
    if(!unit->Load(fl)) return 0;
    if(units.count(unit->id)) { // Duplicate unit id
      delete unit;
      return 0;
      }
    units[unit->id] = unit;
    plunits[0].push_back(unit->id);
    }

  return 1;
  }

int Map::Save(const string &filename) {
  FILE *fl = fopen(filename.c_str(), "w");
  if(!fl) return 0;

  int ret = Save(fl);

  fclose(fl);
  return ret;
  }

int Map::Save(FILE *fl) {
  if(fprintf(fl, "%s\n", HEADER_STRING) < (int)(strlen(HEADER_STRING))+1)
    return 0;

  if(fprintf(fl, "0x%.8X\n", SAVEFILE_VERSION) < 11) return 0;

  if(fprintf(fl, "%d\n", (int)(units.size())) < 2) return 0;

  map<int, Unit *>::iterator itrm = units.begin();
  for(; itrm != units.end(); ++itrm) {
    if(!itrm->second->Save(fl)) return 0;
    }

  return 1;
  }

const Unit *Map::PlayerUnit(int pl, int un) {
  if(pl >= (int)(plunits.size())) return NULL;
  if(un >= (int)(plunits[pl].size())) return NULL;
  if(units.count(plunits[pl][un]) == 0) return NULL;

  return units[plunits[pl][un]];
  }

void Map::Clear() {
  map<int, Unit *>::iterator itrm = units.begin();
  for(; itrm != units.end(); ++itrm) {
    if(itrm->second) delete itrm->second;
    }
  units.clear();
  plunits.clear();
  }
