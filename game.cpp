#include "map.h"
#include "unit.h"
#include "defs.h"

#define MAPFILE_VERSION	0x00000001 // 0.0.0-r1

Map::Map() {
  }

Map::~Map() {
  Clear();
  }

//static char buf[BUF_LEN];

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

  if(fscanf(fl, "0x%X\n", &num) < 1) return 0;
  if(num != MAPFILE_VERSION) return 0;

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
  if(fprintf(fl, "0x%.8X\n", MAPFILE_VERSION) < 11) return 0;

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
