#ifndef MAP_H
#define MAP_H

#include <map>
#include <vector>
using namespace std;

#include "unit.h"

class Map {
public:
  Map(const string &filename);
  void Load(const string &filename);
  void Save(const string &filename);

private:
  map<int, Unit> units;
  vector< vector<int> > plunits;	// List of unit ids per player
  };

#endif // UNIT_H
