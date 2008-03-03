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

#include "SDL.h"
#include "SDL_thread.h"

#include "game.h"
#include "unit.h"
#include "defs.h"

#define HEADER_STRING "LDO_GAMESAVE_FILE"
#define SAVEFILE_VERSION	0x00000004 // 0.0.0-r4

Game::Game() {
  status_mut = NULL;
  status_locked = false;
  threads_term = false;
  }

Game::~Game() {
  Clear();
  }

void Game::AttachPlayer(Player *pl) {
  //FIXME: Protect against duplicates!
  player.resize(pl->ID() + 1);
  player[pl->ID()] = pl;

  status_ready.resize(pl->ID() + 1);
  }

bool Game::SetReady(int plnum, bool rdy) {
  bool ret = false;
  if(status_mut) SDL_mutexP(status_mut);

  if(status_locked) {
    ret = status_ready[plnum];
    }
  else {
    status_ready[plnum] = rdy;
    ret = rdy;
    }

  if(status_mut) SDL_mutexV(status_mut);
  return ret;
  }

bool Game::Ready(int plnum) {
  return status_ready[plnum];
  }

bool Game::AllReadyLock() {
  bool ret = true;
  if(status_mut) SDL_mutexP(status_mut);

  for(unsigned int plnum=0; plnum < status_ready.size(); ++plnum) {
    ret = (ret && status_ready[plnum]);
    }
  if(ret) status_locked = true;

  if(status_mut) SDL_mutexV(status_mut);
  return ret;
  }

bool Game::AllReady() {
  bool ret = true;
  if(status_mut) SDL_mutexP(status_mut);

  for(unsigned int plnum=0; plnum < status_ready.size(); ++plnum) {
    ret = (ret && status_ready[plnum]);
    }

  if(status_mut) SDL_mutexV(status_mut);
  return ret;
  }

void Game::ResetReady() {
  if(status_mut) SDL_mutexP(status_mut);

  for(unsigned int plnum=0; plnum < status_ready.size(); ++plnum) {
    status_ready[plnum] = false;
    }
  status_locked = false;

  if(status_mut) SDL_mutexV(status_mut);
  }

void Game::TermThreads() {
  if(threads_term) return;
  threads_term = true;
  }

bool Game::ShouldTerm() {
  return threads_term;
  }

static char buf[BUF_LEN];

int Game::Load(const string &filename) {
  int ret = 0;
  FILE *fl = NULL;
  fl = fopen((filename + "/wglob.dat").c_str(), "rb");
  if(fl != NULL) {
    Clear();
    ret = LoadXCom(fl, filename);
    fclose(fl);
    }
  else {
    fl = fopen(filename.c_str(), "r");
    if(fl) {
      Clear();
      ret = Load(fl);
      fclose(fl);
      }
    }
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
  if(fprintf(fl, "%d\n", (Sint32)(vec.size())) < 2) return 0;
  for(unsigned int side = 0; side < vec.size(); ++side) {
    if(fprintf(fl, "%d\n", (Sint32)(vec[side].size())) < 2) return 0;
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

int Game::LoadXCom(FILE *fl, const string &dir) {
  Uint8 buf[64];

  fread(buf, 2, 22, fl);

  mapxs = (int(Sint8(buf[5])) << 8) + int(buf[4]);
  mapys = (int(Sint8(buf[7])) << 8) + int(buf[6]);
  mapzs = (int(Sint8(buf[9])) << 8) + int(buf[8]);
//  printf("%dx%dx%d\n", mapxs, mapys, mapzs);

  FILE *map = fopen((dir + "/map.dat").c_str(), "rb");
  if(map) {
    Uint8 map_data[mapzs][mapys][mapxs][4];
    fread(map_data, 4, 4*mapxs*mapys, map);
    int x, y, z;
    for(z = 0; z < mapzs; ++z) {
      for(y = 0; y < mapys; ++y) {
	for(x = 0; x < mapxs; ++x) {
	  if(map_data[z][y][x][0] > 0) {
	    MapObject obj = { GROUND_FLOOR, x, mapys-1-y, mapzs-1-z };
	    master.objects.push_back(obj);
	    }
	  if(map_data[z][y][x][1] > 0) {
	    MapObject obj = { WALL_NORTHSOUTH, x, mapys-1-y, mapzs-1-z };
	    master.objects.push_back(obj);
	    }
	  if(map_data[z][y][x][2] > 0) {
	    MapObject obj = { WALL_EASTWEST, x, mapys-y, mapzs-1-z };
	    master.objects.push_back(obj);
	    }
	  if(map_data[z][y][x][3] > 0) {
	    MapObject obj = { OBJECT_MISC, x, mapys-1-y, mapzs-1-z };
	    master.objects.push_back(obj);
	    }
	  }
	}
      }
    fclose(map);
    }
  return Load("temp.map");	// TEMPORARY!
  }

int Game::Load(FILE *fl) {
  unsigned int num, ver;
  round = 1;

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
  if(fscanf(fl, "%d\n", &num) < 1) return 0;
  for(unsigned int unit = 0; unit < num; ++unit) {
    unit_ptr = new Unit;
    if(!unit_ptr->Load(fl, ver)) return 0;
    if(units.count(unit_ptr->id)) { // Duplicate unit id
      delete unit_ptr;
      return 0;
      }
    units[unit_ptr->id] = unit_ptr;

    //TODO: Use REAL starting locations from map
    //TODO: Do a Real Initial Equip of Items Here
    //TODO: Equip Requested EQUIP Items
    master.my_units[unit_ptr->id].push_back(UnitAct(
	unit_ptr->id, 0, 20+unit, 32, ACT_START));
    master.my_units[unit_ptr->id].push_back(UnitAct(
	unit_ptr->id, 0, 20+unit, 32, ACT_EQUIP, 0, 1));
    }

  Uint32 id = 1;
  for(Uint32 pl=0; pl < plsquads.size(); ++pl) {
    for(Uint32 sq=0; sq < plsquads[pl].size(); ++sq) {
      for(Uint32 un=0; un < squnits[plsquads[pl][sq]].size(); ++un) {
	unplayer[id] = pl;
	++id;
	}
      }
    }

  //Temporary, flat map with no features
  for(int y=0; y < 50; ++y) {
    for(int x=0; x < 50; ++x) {
      MapObject obj = { GROUND_FLOOR, x, y, 0 };
      master.objects.push_back(obj);
      }
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

  master.Clear();
  plsquads.clear();
  squnits.clear();
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
//  for(Uint32 sq=0; sq < plsquads[plnum].size(); ++sq) {
//    for(Uint32 un=0; un < squnits[plsquads[plnum][sq]].size(); ++un) {
//      prcpt->my_units.insert(squnits[plsquads[plnum][sq]][un]);
//      }
//    }
  percept[plnum] = prcpt;
  }

void Game::UpdatePercept(int plnum, unsigned int rnd) {
  if(rnd < 1 || rnd > round) {
    fprintf(stderr, "ERROR: Percept requested in future or pre-start!\n");
    exit(1);
    }

//  if(rnd >= round) {	// Current (Unresolved) Round
//    rnd = round - 1;	//Temporary?
//    }

  percept[plnum]->Clear();
  map<int, vector<UnitAct> >::const_iterator itr = master.my_units.begin();
  for(; itr != master.my_units.end(); ++itr) {
//    fprintf(stderr, "Sending %d action %d for %d (Round %d)\n",
//	plnum, itr->act, itr->id, rnd);
    if(plnum == PlayerIDForUnit(itr->first)) {
      percept[plnum]->my_units.insert(*itr);
      }
    else {
      percept[plnum]->other_units.insert(*itr);
      }
    }
  //Temporary!
  percept[plnum]->objects = master.objects;
  }

//Thread Stuff
static int player_thread_func(void *arg) {
  int ret = ((Player*)(arg))->Run();
  return ret;
  }

static int game_thread_func(void *arg) {
  int ret = ((Game*)(arg))->ThreadHandler();
  return ret;
  }

PlayResult Game::Play() {
  Player *localp = NULL;
  status_mut = SDL_CreateMutex();
  threads_term = false;

  unsigned int n = 1;		//Reserve a place for ThreadHandler thread
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

  for(n=0; n < thread.size(); ++n) {
    SDL_WaitThread(thread[n], NULL);
    }
  SDL_DestroyMutex(status_mut);
  status_mut = NULL;

  return PLAY_FINISHED;
  }

int Game::ThreadHandler() {
  int ticker = 0;
  while(1) {
    while(ticker < 4 || (!AllReadyLock())) {	// One second delay (Temporary)
      SDL_Delay(250);	// Check for done 4 times per second (Temporary)

      if(threads_term) break;

      bool ret = AllReady();
      if(ret) ++ticker;
      else ticker = 0;
      }

    if(threads_term) break;

    ResolveRound();
    ResetReady();
    ticker = 0;
    }

//  fprintf(stderr, "DEBUG: Exiting ThreadHandler()\n");
  TermThreads();  // Tell everyone else to exit too

  return 0;
  }

void Game::ResolveRound() {
  round++;

  set<int> ordered;
  vector<Player *>::const_iterator itrp = player.begin();
  for(; itrp != player.end(); ++itrp) {
    int pnum = (*itrp)->ID();
    if(orders.count(pnum) < 1) {
      fprintf(stderr, "ERROR: Player %d's orders got lost!\n", pnum);
      TermThreads();
      }
    else {
      vector<UnitOrder>::const_iterator order = orders[pnum]->orders.begin();
      for(; order != orders[pnum]->orders.end(); ++order) {
//	fprintf(stderr, "ORDER[Player%d], %d do %d at %d to (%d,%d)\n", pnum,
//		order->id, order->order, order->time,
//		order->targ1, order->targ2);
	if(master.my_units[order->id].back().act == ACT_FALL) {
	  continue;
	  }

	int x=0, y=0;
	master.GetPos(order->id, x, y);
	int offset = (rand() % 500) + 500;
	switch(order->order) {
	  case(ORDER_MOVE): {
	    master.my_units[order->id].push_back(UnitAct(order->id,
		(CurrentRound() - 2) * 3000 + order->time + offset,
		order->targ1, order->targ2, ACT_MOVE, x, y));
	    ordered.insert(order->id);
	    }break;
	  case(ORDER_RUN): {
	    master.my_units[order->id].push_back(UnitAct(order->id,
		(CurrentRound() - 2) * 3000 + order->time + offset,
		order->targ1, order->targ2, ACT_RUN, x, y));
	    ordered.insert(order->id);
	    }break;
	  case(ORDER_EQUIP): {
	    if(round != 1 || order->time != 0) {	// Not Initial EQUIP
	      if(ordered.count(order->id) <= 0) {
		master.my_units[order->id].push_back(UnitAct(order->id,
			(CurrentRound() - 2) * 3000 + order->time + offset,
			x, y, ACT_EQUIP, order->targ1, order->targ2));
		ordered.insert(order->id);
		}
	      }
	    else {				// Initial Equip is Free
	      if(ordered.count(order->id) <= 0) {
		master.my_units[order->id].push_back(UnitAct(order->id, 0,
			x, y, ACT_EQUIP, order->targ1, order->targ2));
		ordered.insert(order->id);
		}
	      }
	    }break;
	  case(ORDER_SHOOT): {
	    int hit = 0;
	    if(order->targ1 < 0) {
	      hit = order->targ2;
	      }
	    else {
	      hit = master.UnitAt(order->targ1, order->targ2);
	      }
	    if(hit > 0) {
	      int tx, ty;
	      master.GetPos(hit, tx, ty);
	      master.my_units[hit].push_back(UnitAct(hit,
		(CurrentRound() - 2) * 3000 + order->time + 250 + offset,
		tx, ty, ACT_FALL));
	      ordered.insert(hit);

	      master.my_units[order->id].push_back(UnitAct(order->id,
		(CurrentRound() - 2) * 3000 + order->time + offset, x, y,
		ACT_SHOOT, tx, ty));
	      }
	    else {
	      master.my_units[order->id].push_back(UnitAct(order->id,
		(CurrentRound() - 2) * 3000 + order->time + offset, x, y,
		ACT_SHOOT, order->targ1, order->targ2));
	      }
	    ordered.insert(order->id);
	    }break;
	  default: {
	    fprintf(stderr, "WARNING: Got unknown ORDER from Player%d\n", pnum);
	    }break;
	  }
	}
      }
    orders[pnum]->Clear();
    }

//  //Create STAND orders for all units not given other orders
//  set<int>::const_iterator unit = master.my_units.begin();
//  for(; unit != master.my_units.end(); ++unit) {
//    if(ordered.count(*unit) == 0) {
//      vector<UnitAct>::const_iterator prev =
//		master.my_units[master.my_units.size() - 2].my_acts.begin();
//      for(; prev != master.my_units[master.my_units.size() - 2].my_acts.end(); ++prev) {
//	if(prev->id == (*unit)) {
//	  master.my_units[order->id].push_back(UnitAct((*unit),
//		(CurrentRound() - 2) * 3000, prev->x, prev->y,
//		ACT_STAND, prev->x, prev->y));
//	  }
//	}
//      }
//    }
  }
