#ifndef MAP_H
#define MAP_H

#include <map>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"

class Map {
public:
  Map();
  ~Map();
  int Load(const string &filename);
  int Save(const string &filename);
  int Load(FILE *fl);
  int Save(FILE *fl);

  const Unit *PlayerUnit(int pl, int un);

private:
  void Clear();
  map<int, Unit *> units;
  vector< vector<int> > plunits;	// List of unit ids per player
  };

#endif // UNIT_H
