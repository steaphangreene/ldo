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

#ifndef MAP_H
#define MAP_H

#include <map>
#include <string>
#include <vector>
#include <cstdio>
using namespace std;

#include "unit.h"
#include "percept.h"
#include "player.h"

enum PlayResult {
  PLAY_ERROR,		//Something broke
  PLAY_FINISHED,	//Game's over, show results.
  PLAY_CONFIG,		//Game's not over, run configuration.
  PLAY_SAVE,		//Game's not over, save and continue game.
  PLAY_MAX
  };

class Game {
public:
  Game();
  ~Game();
  void AttachPlayer(Player *pl);

  int Load(const string &filename);
  int Save(const string &filename);
  int Load(FILE *fl);
  int Save(FILE *fl);
  static int Load(vector< vector<int> > &vec, FILE *fl);
  static int Save(const vector< vector<int> > &vec, FILE *fl);

  const Unit *PlayerUnit(int pl, int sq, int un);	//Temporary!

  const string &MapName() { return mapname; };
  const string &MapDesc() { return mapdesc; };

  void SetPercept(int plnum, Percept *prcpt);
  void UpdatePercept(int plnum, int rnd);

  int CurrentRound() { return master.size(); }
  PlayResult Play();

private:
  void Clear();

  int mapxs, mapys;
  string mapname, mapdesc;

  vector< vector<int> > sides;		// List of all player ids per side

  vector< vector<int> > plsquads;	// List of squad ids per player

  vector< vector<int> > squnits;	// List of unit ids per squad

  map<int, Unit *> units;		// Actual unit container

  vector<Percept> master;		// Master game percepts (for each turn)
  map<int, Percept *> percept;		// Percept for each player

  vector<Player *> player;		//Current ordered list of players
  };

#endif // UNIT_H

