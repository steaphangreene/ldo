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

#include <SDL/SDL_thread.h>

#include "game.h"
#include "unit.h"
#include "defs.h"

#define HEADER_STRING "LDO_GAMESAVE_FILE"
#define SAVEFILE_VERSION	0x00000004 // 0.0.0-r4

Game::Game() {
  }

Game::~Game() {
  Clear();
  }

void Game::AttachPlayer(Player *pl) {
  //FIXME: Protect against duplicates!
  player.resize(pl->ID() + 1);
  player[pl->ID()] = pl;
  }

static char buf[BUF_LEN];

int Game::Load(const string &filename) {
  FILE *fl = fopen(filename.c_str(), "r");
  if(!fl) return 0;

  Clear();
  int ret = Load(fl);

  fclose(fl);
  return ret;
  }

int Game::Save(const string &filename) {
  FILE *fl = fopen(filename.c_str(), "w");
  if(!fl) return 0;

  int ret = Save(fl);

  fclose(fl);
  return ret;
  }

int Game::Save(const vector< vector<int> > &vec, FILE *fl) {
  if(fprintf(fl, "%d\n", vec.size()) < 2) return 0;
  for(unsigned int side = 0; side < vec.size(); ++side) {
    if(fprintf(fl, "%d\n", vec[side].size()) < 2) return 0;
    for(unsigned int pl = 0; pl < vec[side].size(); ++pl) {
      if(fprintf(fl, "%d;", vec[side][pl]) < 2) return 0;
      }
    if(fprintf(fl, "\n") < 1) return 0;
    }
  return 1;
  }

int Game::Load(vector< vector<int> > &vec, FILE *fl) {
  vec.clear();
  unsigned int num;
  if(fscanf(fl, "%d\n", &num) < 1) return 0;
  vec.resize(num);
  for(unsigned int pl=0; pl < vec.size(); ++pl) {
    if(fscanf(fl, "%d\n", &num) < 1) return 0;
    vec[pl].resize(num);
    for(unsigned int squad=0; squad < vec[pl].size(); ++squad) {
      if(fscanf(fl, "%d;", &num) < 1) return 0;
      vec[pl][squad] = num;
      }
    fscanf(fl, "\n");
    }
  return 1;
  }

int Game::Load(FILE *fl) {
  unsigned int num, ver;
  master.resize(1);

  memset(buf, 0, BUF_LEN);
  if(fscanf(fl, "%s\n", buf) < 1) return 0;
  if(strcmp(buf, HEADER_STRING)) return 0;

  if(fscanf(fl, "0x%X\n", &ver) < 1) return 0;
  if(ver != SAVEFILE_VERSION) return 0;

  memset(buf, 0, BUF_LEN);
  if(fscanf(fl, "%[^\n;];\n", buf) < 1) return 0;
  mapname = buf;

  memset(buf, 0, BUF_LEN);
  if(fscanf(fl, "%[^\n;];\n", buf) < 1) return 0;
  mapdesc = buf;

  if(!Load(sides, fl)) return 0;
  if(!Load(plsquads, fl)) return 0;
  if(!Load(squnits, fl)) return 0;

  Unit *unit_ptr;
  map<int, int> tr2un;  //Map troop id to unit id for initial ACT_EQUIP
  if(fscanf(fl, "%d\n", &num) < 1) return 0;
  for(unsigned int unit = 0; unit < num; ++unit) {
    unit_ptr = new Unit;
    if(!unit_ptr->Load(fl, ver)) return 0;
    if(units.count(unit_ptr->id)) { // Duplicate unit id
      delete unit_ptr;
      return 0;
      }
    units[unit_ptr->id] = unit_ptr;
    master[0].my_units.insert(unit_ptr->id);

    if(tr2un.count(unit_ptr->troop) == 0) {
      tr2un[unit_ptr->troop] = unit_ptr->id;
      }
    int tr = tr2un[unit_ptr->troop];

    master[0].my_acts.push_back(UnitAct(unit_ptr->id, 0, ACT_EQUIP, tr));
    }
  return 1;
  }

int Game::Save(FILE *fl) {
  if(fprintf(fl, "%s\n", HEADER_STRING) < (int)(strlen(HEADER_STRING))+1)
    return 0;

  if(fprintf(fl, "0x%.8X\n", SAVEFILE_VERSION) < 11) return 0;

  if(fprintf(fl, "%s;\n", mapname.c_str()) < (int)(mapname.length())) return 0;

  if(fprintf(fl, "%s;\n", mapdesc.c_str()) < (int)(mapdesc.length())) return 0;

  if(!Save(sides, fl)) return 0;
  if(!Save(plsquads, fl)) return 0;
  if(!Save(squnits, fl)) return 0;

  if(fprintf(fl, "%d\n", (int)(units.size())) < 2) return 0;
  map<int, Unit *>::iterator itrm = units.begin();
  for(; itrm != units.end(); ++itrm) {
    if(!itrm->second->Save(fl)) return 0;
    }

  return 1;
  }

const Unit *Game::UnitRef(int id) {
  return units[id];
  }

void Game::Clear() {
  map<int, Unit *>::iterator itrm = units.begin();
  for(; itrm != units.end(); ++itrm) {
    if(itrm->second) delete itrm->second;
    }
  units.clear();

  vector<Player *>::const_iterator itrp = player.begin();
  for(; itrp != player.end(); ++itrp) {
    delete (*itrp);
    }
  player.clear();

  plsquads.clear();
  squnits.clear();
  master.clear();
  percept.clear();
  orders.clear();
  }

void Game::SetOrders(int plnum, Orders *ord) {
  if(orders.count(plnum)) {
    fprintf(stderr, "ERROR: Multiple orders requested for one player!\n");
    exit(1);
    }
  orders[plnum] = ord;
  }

void Game::SetPercept(int plnum, Percept *prcpt) {
  if(percept.count(plnum)) {
    fprintf(stderr, "ERROR: Multiple percepts requested for one player!\n");
    exit(1);
    }
  percept[plnum] = prcpt;
  }

void Game::UpdatePercept(int plnum, int rnd) {
  if(rnd < 0 || rnd >= (int)(percept.size())) {
    fprintf(stderr, "ERROR: Percept requested in future or pre-start!\n");
    exit(1);
    }
  if(rnd == (int)(percept.size())) {
    // Current (Unresolved) Round
    *(percept[plnum]) = master[rnd];	//Temporary!
    }
  else {
    // Old (Resolved) Round
    *(percept[plnum]) = master[rnd];	//Temporary!
    }
  }




//Thread Stuff
static int player_thread_func(void *arg) {
  return ((Player*)(arg))->Run();
  }

static int game_thread_func(void *arg) {
  return ((Game*)(arg))->ThreadHandler();
  }

static vector<SDL_Thread *> thread;

PlayResult Game::Play() {
  Player *localp = NULL;

  int n = 1;		//Reserve a place for ThreadHandler thread
  thread.resize(player.size());
  vector<Player *>::const_iterator itrp = player.begin();
  for(; itrp != player.end(); ++itrp) {
    if((*itrp)->Type() != PLAYER_LOCAL) {
      thread[n] = SDL_CreateThread(player_thread_func, (void*)(*itrp));
      ++n;
      }
    else {
      if(localp != NULL) {
	fprintf(stderr, "ERROR: Multiple local players defined!\n");
	exit(1);
	}
      localp = (*itrp);
      }
    }

  //I can't be the ThreadHandler due to SDL/OpenGL limitations
  thread[0] = SDL_CreateThread(game_thread_func, (void*)(this));

  //I have to be the local player due to SDL/OpenGL limitations
  localp->Run();

  return PLAY_FINISHED;
  }

int Game::ThreadHandler() {
  bool ret = false;

  while(!ret) {
    ret = true;	//Initialize it to "ready"

    vector<Player *>::const_iterator itrp = player.begin();
    for(; itrp != player.end(); ++itrp) {
      ret = (ret && (*itrp)->Ready());
      }

    SDL_Delay(250);	// Check for done 4 times per second
    }

  return 0;
  }
