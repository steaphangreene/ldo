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

#include "SDL.h"
#include "SDL_thread.h"

#include "game.h"
#include "unit.h"
#include "defs.h"

#include <set>
#include <map>
#include <vector>
#include <cmath>

#define HEADER_STRING "LDO_GAMESAVE_FILE"
#define SAVEFILE_VERSION 0x00000004  // 0.0.0-r4

static int oid = 1;

Game::Game() {
  status_mut = NULL;
  status_locked = false;
  threads_term = false;
}

Game::~Game() { Clear(); }

void Game::AttachPlayer(Player *pl) {
  // FIXME: Protect against duplicates!
  player.resize(pl->ID() + 1);
  player[pl->ID()] = pl;

  status_ready.resize(pl->ID() + 1);
}

bool Game::SetReady(int plnum, bool rdy) {
  bool ret = false;
  if (status_mut) SDL_mutexP(status_mut);

  if (status_locked) {
    ret = status_ready[plnum];
  } else {
    status_ready[plnum] = rdy;
    ret = rdy;
  }

  if (status_mut) SDL_mutexV(status_mut);
  return ret;
}

bool Game::Ready(int plnum) { return status_ready[plnum]; }

bool Game::AllReadyLock() {
  bool ret = true;
  if (status_mut) SDL_mutexP(status_mut);

  for (unsigned int plnum = 0; plnum < status_ready.size(); ++plnum) {
    ret = (ret && status_ready[plnum]);
  }
  if (ret) status_locked = true;

  if (status_mut) SDL_mutexV(status_mut);
  return ret;
}

bool Game::AllReady() {
  bool ret = true;
  if (status_mut) SDL_mutexP(status_mut);

  for (unsigned int plnum = 0; plnum < status_ready.size(); ++plnum) {
    ret = (ret && status_ready[plnum]);
  }

  if (status_mut) SDL_mutexV(status_mut);
  return ret;
}

void Game::ResetReady() {
  if (status_mut) SDL_mutexP(status_mut);

  for (unsigned int plnum = 0; plnum < status_ready.size(); ++plnum) {
    status_ready[plnum] = false;
  }
  status_locked = false;

  if (status_mut) SDL_mutexV(status_mut);
}

void Game::TermThreads() {
  if (threads_term) return;
  threads_term = true;
}

bool Game::ShouldTerm() { return threads_term; }

static char buf[BUF_LEN];

int Game::Load(const string &filename) {
  int ret = 0;
  FILE *fl = NULL;
  fl = fopen((filename + "/wglob.dat").c_str(), "rb");
  if (fl == NULL) fl = fopen((filename + "/WGLOB.DAT").c_str(), "rb");
  if (fl != NULL) {
    Clear();
    ret = LoadXCom(fl, filename);
    fclose(fl);
  } else {
    fl = fopen(filename.c_str(), "r");
    if (fl) {
      Clear();
      ret = Load(fl);
      fclose(fl);
    }
  }
  return ret;
}

int Game::Save(const string &filename) {
  FILE *fl = fopen(filename.c_str(), "w");
  if (!fl) return 0;

  int ret = Save(fl);

  fclose(fl);
  return ret;
}

int Game::Save(const vector<vector<int> > &vec, FILE *fl) {
  if (fprintf(fl, "%d\n", (Sint32)(vec.size())) < 2) return 0;
  for (unsigned int side = 0; side < vec.size(); ++side) {
    if (fprintf(fl, "%d\n", (Sint32)(vec[side].size())) < 2) return 0;
    for (unsigned int pl = 0; pl < vec[side].size(); ++pl) {
      if (fprintf(fl, "%d;", vec[side][pl]) < 2) return 0;
    }
    if (fprintf(fl, "\n") < 1) return 0;
  }
  return 1;
}

int Game::Load(vector<vector<int> > &vec, FILE *fl) {
  vec.clear();
  unsigned int num;
  if (fscanf(fl, "%d\n", &num) < 1) return 0;
  vec.resize(num);
  for (unsigned int pl = 0; pl < vec.size(); ++pl) {
    if (fscanf(fl, "%d\n", &num) < 1) return 0;
    vec[pl].resize(num);
    for (unsigned int squad = 0; squad < vec[pl].size(); ++squad) {
      if (fscanf(fl, "%d;", &num) < 1) return 0;
      vec[pl][squad] = num;
    }
    fscanf(fl, "\n");
  }
  return 1;
}

int Game::LoadXCom(FILE *fl, const string &dir) {
  bool tftd = false;  // Is this X-Com 2 (Terror from the Deep)?
  Uint8 buf[64];
  map<Uint8, float> height;  // All Non-Default Height Objects
  height[32] = 0.0;          // Broken Fence
  height[130] = 2.25;        // Top of Ramp
  height[131] = 1.5;         // Middle of Ramp
  height[132] = 0.75;        // Bottom of Ramp

  fread(buf, 2, 22, fl);

  master.mapxs = (int(Sint8(buf[5])) << 8) + int(buf[4]);
  master.mapys = (int(Sint8(buf[7])) << 8) + int(buf[6]);
  master.mapzs = (int(Sint8(buf[9])) << 8) + int(buf[8]);

  master.mapname = "X-Com Map";
  master.mapdesc = "Loaded from an X-Com tactical save.";
  master.round = 1;  // FIXME, load from map
  int ttype = 0;

  FILE *mis = fopen((dir + "/misdata.dat").c_str(), "rb");
  if (mis == NULL) mis = fopen((dir + "/MISDATA.DAT").c_str(), "rb");
  if (mis) {
    fseek(mis, 0, SEEK_END);
    if (ftell(mis) == 172) {
      tftd = true;  // This IS X-Com 2 (Terror from the Deep)
      ttype = 10;
    }
    fclose(mis);
  }

  FILE *geo = fopen((dir + "/geodata.dat").c_str(), "rb");
  if (geo == NULL) geo = fopen((dir + "/GEODATA.DAT").c_str(), "rb");
  if (geo) {
    Uint8 geo_data[92];
    fread(geo_data, 1, 92, geo);
    fclose(geo);
    ttype += (int(Sint8(geo_data[11])) << 8) + int(geo_data[10]);
  }

  Uint8 see_data[master.mapzs][master.mapys][master.mapxs];
  memset(see_data, 7, master.mapzs * master.mapys * master.mapxs);
  FILE *seemap = fopen((dir + "/seemap.dat").c_str(), "rb");
  if (seemap == NULL) seemap = fopen((dir + "/SEEMAP.DAT").c_str(), "rb");
  if (seemap) {
    fread(see_data, master.mapxs, master.mapys * master.mapzs, seemap);
    fclose(seemap);
  }

  map<MapCoord, int> burn_data;
  FILE *effects = fopen((dir + "/smokref.dat").c_str(), "rb");
  if (effects == NULL) effects = fopen((dir + "/SMOKREF.DAT").c_str(), "rb");
  if (effects) {
    Uint8 effect_data[400][9];
    fread(effect_data, 9, 400, effects);
    fclose(effects);
    for (int effect = 0; effect < 400; ++effect) {
      if (effect_data[effect][6] > 0) {  // FIXME: Is this really the test?
        int x, y, z;
        x = int(effect_data[effect][1]);
        y = master.mapys - 1 - int(effect_data[effect][0]);
        z = master.mapzs - 1 - int(effect_data[effect][2]);
        MapCoord pos = {x, y, z};
        if (effect_data[effect][6] == 2) {  // Smoke
          burn_data[pos] = effect_data[effect][3];
        } else {  // Fire
          burn_data[pos] = 64 + effect_data[effect][4];
        }
      }
    }
  }

  FILE *map = fopen((dir + "/map.dat").c_str(), "rb");
  if (map == NULL) map = fopen((dir + "/MAP.DAT").c_str(), "rb");
  if (map) {
    Uint8 map_data[master.mapzs][master.mapys][master.mapxs][4];
    fread(map_data, 4, master.mapzs * master.mapxs * master.mapys, map);
    fclose(map);
    int x, y, z;
    for (z = 0; z < master.mapzs; ++z) {
      for (y = 0; y < master.mapys; ++y) {
        for (x = 0; x < master.mapxs; ++x) {
          if (map_data[z][y][x][0] > 0) {
            MapCoord pos = {x, master.mapys - 1 - y, master.mapzs - 1 - z};
            MapObject obj = {GROUND_FLOOR, oid++,
                             0x100 * ttype + map_data[z][y][x][0], 0.0};
            if ((see_data[z][y][x] & 0x04) > 0) {
              obj.seen[0].insert(pair<Uint32, Uint32>(0, 0));
            }
            if (burn_data.count(pos) > 0) {
              obj.burn[0] = burn_data[pos];
            }
            if (height.count(obj.which) > 0) obj.height = height[obj.which];
            master.objects.insert(pair<MapCoord, MapObject>(pos, obj));
          }
          if (map_data[z][y][x][1] > 0) {
            MapCoord pos = {x, master.mapys - 1 - y, master.mapzs - 1 - z};
            MapObject obj = {WALL_WEST, oid++,
                             0x100 * ttype + map_data[z][y][x][1], 3.0};
            if ((see_data[z][y][x] & 0x01) > 0) {
              obj.seen[0].insert(pair<Uint32, Uint32>(0, 0));
            }
            if (burn_data.count(pos) > 0) {
              obj.burn[0] = burn_data[pos];
            }
            if (height.count(obj.which) > 0) obj.height = height[obj.which];
            master.objects.insert(pair<MapCoord, MapObject>(pos, obj));
          }
          if (map_data[z][y][x][2] > 0) {
            MapCoord pos = {x, master.mapys - y, master.mapzs - 1 - z};
            MapObject obj = {WALL_NORTH, oid++,
                             0x100 * ttype + map_data[z][y][x][2], 3.0};
            if ((see_data[z][y][x] & 0x02) > 0) {
              obj.seen[0].insert(pair<Uint32, Uint32>(0, 0));
            }
            if (burn_data.count(pos) > 0) {
              obj.burn[0] = burn_data[pos];
            }
            if (height.count(obj.which) > 0) obj.height = height[obj.which];
            master.objects.insert(pair<MapCoord, MapObject>(pos, obj));
          }
          if (map_data[z][y][x][3] > 0) {
            MapCoord pos = {x, master.mapys - 1 - y, master.mapzs - 1 - z};
            MapObject obj = {OBJECT_MISC, oid++,
                             0x100 * ttype + map_data[z][y][x][3], 0.8};
            if ((see_data[z][y][x] & 0x04) > 0) {
              obj.seen[0].insert(pair<Uint32, Uint32>(0, 0));
            }
            if (burn_data.count(pos) > 0) {
              obj.burn[0] = burn_data[pos];
            }
            if (height.count(obj.which) > 0) obj.height = height[obj.which];
            master.objects.insert(pair<MapCoord, MapObject>(pos, obj));
          }
        }
      }
    }
  }

  int num_units = 80, recsize = 124;
  FILE *unitref = fopen((dir + "/unitref.dat").c_str(), "rb");
  if (unitref == NULL) unitref = fopen((dir + "/UNITREF.DAT").c_str(), "rb");
  if (unitref) {
    if (tftd) recsize = 132;
    fseek(unitref, 0, SEEK_END);
    num_units = ftell(unitref) / recsize;
    fseek(unitref, 0, SEEK_SET);
  }
  Uint8 unitref_data[num_units][recsize];
  if (unitref) {
    fread(unitref_data, num_units, recsize, unitref);
    fclose(unitref);
  }
  sides.resize(3);
  sides[0].push_back(0);
  sides[1].push_back(1);
  sides[2].push_back(2);
  plsquads.resize(3);
  plsquads[0].push_back(0);
  plsquads[1].push_back(1);
  plsquads[2].push_back(2);
  squnits.resize(3);
  FILE *unitfl = fopen((dir + "/unitpos.dat").c_str(), "rb");
  if (unitfl == NULL) unitfl = fopen((dir + "/UNITPOS.DAT").c_str(), "rb");
  if (unitfl) {
    Uint8 unit_data[80][14];
    fread(unit_data, 14, 80, unitfl);
    fclose(unitfl);
    int sidecount[3] = {0, 0, 0};
    for (int unit = 0; unit < 80;
         ++unit) {  // FIXME: How do I tell what's real?
      if ((unit_data[unit][1] | unit_data[unit][0] | unit_data[unit][2]) != 0) {
        int unit_id = (int)(units.size() + 1);
        sidecount[unit_data[unit][9]]++;
        squnits[unit_data[unit][9]].push_back(unit_id);

        Unit *unit_ptr;
        int x, y, z;
        x = int(unit_data[unit][1]);
        y = master.mapys - 1 - int(unit_data[unit][0]);
        z = master.mapzs - 1 - int(unit_data[unit][2]);
        float ang = 90.0 - 45.0 * unitref_data[unit][10];
        if (ang < 0.0) ang += 360.0;
        unit_ptr = new Unit;
        unit_ptr->id = unit_id;
        unit_ptr->troop = unit_data[unit][9];
        unit_ptr->name = (char *)(unitref_data[unit] + 86);
        units[unit_id] = unit_ptr;
        master.unplayer[unit_id] = unit_data[unit][9];
        master.AddAction(unit_id, 0, 0, x, y, z, ang, ACT_START);
        master.AddAction(unit_id, 0, 0, x, y, z, ang, ACT_EQUIP, 0, 1);
        //	printf("Unit at %dx%dx%d\t[%.2X %.2X %.2X %.2X %.2X %.2X %.2X
        //%.2X %.2X %.2X %.2X]\n",
        //		unit_data[unit][1], unit_data[unit][0],
        // unit_data[unit][2],
        //		unit_data[unit][3], unit_data[unit][4],
        // unit_data[unit][5],
        //		unit_data[unit][6], unit_data[unit][7],
        // unit_data[unit][8],
        //		unit_data[unit][9], unit_data[unit][10],
        // unit_data[unit][11],
        //		unit_data[unit][12], unit_data[unit][13]
        //		);
      }
    }
    if (sidecount[2] < 1) {
      sides.pop_back();
      plsquads.pop_back();
    }
  }
  return true;
}

int Game::Load(FILE *fl) {
  unsigned int num, ver;
  master.round = 1;

  // Temporary, flat map with no features
  master.mapxs = 50;
  master.mapys = 50;
  master.mapzs = 1;
  for (int y = 0; y < 50; ++y) {
    for (int x = 0; x < 50; ++x) {
      MapCoord pos = {x, y, 0};
      MapObject obj = {GROUND_FLOOR, oid++, 1 + x + y, 0.0};
      master.objects.insert(pair<MapCoord, MapObject>(pos, obj));
    }
  }

  memset(buf, 0, BUF_LEN);
  if (fscanf(fl, "%s\n", buf) < 1) return 0;
  if (strcmp(buf, HEADER_STRING)) return 0;

  if (fscanf(fl, "0x%X\n", &ver) < 1) return 0;
  if (ver != SAVEFILE_VERSION) return 0;

  memset(buf, 0, BUF_LEN);
  if (fscanf(fl, "%[^\n;];\n", buf) < 1) return 0;
  master.mapname = buf;

  memset(buf, 0, BUF_LEN);
  if (fscanf(fl, "%[^\n;];\n", buf) < 1) return 0;
  master.mapdesc = buf;

  if (!Load(sides, fl)) return 0;
  if (!Load(plsquads, fl)) return 0;
  if (!Load(squnits, fl)) return 0;

  Uint32 id = 1;
  for (Uint32 pl = 0; pl < plsquads.size(); ++pl) {
    for (Uint32 sq = 0; sq < plsquads[pl].size(); ++sq) {
      for (Uint32 un = 0; un < squnits[plsquads[pl][sq]].size(); ++un) {
        master.unplayer[id] = pl;
        ++id;
      }
    }
  }

  Unit *unit_ptr;
  if (fscanf(fl, "%d\n", &num) < 1) return 0;
  for (unsigned int unit = 0; unit < num; ++unit) {
    unit_ptr = new Unit;
    if (!unit_ptr->Load(fl, ver)) return 0;
    if (units.count(unit_ptr->id)) {  // Duplicate unit id
      delete unit_ptr;
      return 0;
    }
    units[unit_ptr->id] = unit_ptr;

    // TODO: Use REAL starting locations from map
    // TODO: Do a Real Initial Equip of Items Here
    // TODO: Equip Requested EQUIP Items
    master.AddAction(unit_ptr->id, 0, 0, 20 + unit, 32, 0, 0.0, ACT_START);
    master.AddAction(unit_ptr->id, 0, 0, 20 + unit, 32, 0, 0.0, ACT_EQUIP, 0,
                     1);
  }
  return 1;
}

int Game::Save(FILE *fl) {
  if (fprintf(fl, "%s\n", HEADER_STRING) < (int)(strlen(HEADER_STRING)) + 1)
    return 0;

  if (fprintf(fl, "0x%.8X\n", SAVEFILE_VERSION) < 11) return 0;

  if (fprintf(fl, "%s;\n", master.mapname.c_str()) <
      (int)(master.mapname.length()))
    return 0;

  if (fprintf(fl, "%s;\n", master.mapdesc.c_str()) <
      (int)(master.mapdesc.length()))
    return 0;

  if (!Save(sides, fl)) return 0;
  if (!Save(plsquads, fl)) return 0;
  if (!Save(squnits, fl)) return 0;

  if (fprintf(fl, "%d\n", (int)(units.size())) < 2) return 0;
  map<int, Unit *>::iterator itrm = units.begin();
  for (; itrm != units.end(); ++itrm) {
    if (!itrm->second->Save(fl)) return 0;
  }

  return 1;
}

const Unit *Game::UnitRef(int id) { return units[id]; }

void Game::Clear() {
  ResetReady();

  map<int, Unit *>::iterator itrm = units.begin();
  for (; itrm != units.end(); ++itrm) {
    if (itrm->second) delete itrm->second;
  }
  units.clear();

  vector<Player *>::const_iterator itrp = player.begin();
  for (; itrp != player.end(); ++itrp) {
    delete (*itrp);
  }
  player.clear();

  master.Clear();
  plsquads.clear();
  squnits.clear();
  percept.clear();
  orders.clear();
  sides.clear();

  master.round = 1;
}

void Game::SetOrders(int plnum, Orders *ord) {
  if (orders.count(plnum)) {
    fprintf(stderr, "ERROR: Multiple orders requested for one player!\n");
    exit(1);
  }
  orders[plnum] = ord;
}

void Game::SetPercept(int plnum, Percept *prcpt) {
  if (percept.count(plnum)) {
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

bool Game::PerceptUpToDate(int plnum) {
  return (percept[plnum]->round == master.round);
}

void Game::UpdatePercept(int plnum) {
  percept[plnum]->Clear();
  map<int, vector<UnitAct> >::const_iterator itr = master.my_units.begin();
  for (; itr != master.my_units.end(); ++itr) {
    //    fprintf(stderr, "Sending %d action %d for %d (Round %d)\n",
    //	plnum, itr->act, itr->id, rnd);
    if (plnum == master.unplayer[itr->first]) {
      percept[plnum]->my_units.insert(*itr);
    } else {
      percept[plnum]->other_units.insert(*itr);
    }
  }
  // Temporary!
  multimap<MapCoord, MapObject>::const_iterator obj = master.objects.begin();
  for (; obj != master.objects.end(); ++obj) {
    if (obj->second.seen.count(plnum)) {
      multimap<MapCoord, MapObject>::iterator nobj;
      nobj = percept[plnum]->objects.insert(*obj);

      // Only give "seen" data for the particular player
      nobj->second.seen.erase(nobj->second.seen.begin(),
                              nobj->second.seen.find(plnum));
      nobj->second.seen.erase(nobj->second.seen.upper_bound(plnum),
                              nobj->second.seen.end());
    }
  }

  // Temporary, calculate based on discovery
  percept[plnum]->mapxs = master.mapxs;
  percept[plnum]->mapys = master.mapys;
  percept[plnum]->mapzs = master.mapzs;
  percept[plnum]->round = master.round;
}

// Thread Stuff
static int player_thread_func(void *arg) {
  int ret = ((Player *)(arg))->Run();
  return ret;
}

static int game_thread_func(void *arg) {
  int ret = ((Game *)(arg))->ThreadHandler();
  return ret;
}

PlayResult Game::Play() {
  Player *localp = NULL;
  status_mut = SDL_CreateMutex();
  threads_term = false;

  unsigned int n = 1;  // Reserve a place for ThreadHandler thread
  thread.resize(player.size() + 1);
  vector<Player *>::const_iterator itrp = player.begin();
  for (; itrp != player.end(); ++itrp) {
    if ((*itrp)->Type() != PLAYER_LOCAL) {
      thread[n] =
          SDL_CreateThread(player_thread_func, "player", (void *)(*itrp));
      ++n;
    } else {
      if (localp != NULL) {
        fprintf(stderr, "ERROR: Multiple local players defined!\n");
        exit(1);
      }
      localp = (*itrp);
    }
  }

  // I can't be the ThreadHandler due to SDL/OpenGL limitations
  thread[0] = SDL_CreateThread(game_thread_func, "game", (void *)(this));

  // I have to be the local player due to SDL/OpenGL limitations
  localp->Run();

  for (n = 0; n < thread.size(); ++n) {
    SDL_WaitThread(thread[n], NULL);
  }
  SDL_DestroyMutex(status_mut);
  status_mut = NULL;

  return PLAY_FINISHED;
}

int Game::ThreadHandler() {
  int ticker = 0;
  while (1) {
    while (ticker < 4 || (!AllReadyLock())) {  // One second delay (Temporary)
      SDL_Delay(250);  // Check for done 4 times per second (Temporary)

      if (threads_term) break;

      bool ret = AllReady();
      if (ret)
        ++ticker;
      else
        ticker = 0;
    }

    if (threads_term) break;

    ResolveRound();
    ResetReady();
    ticker = 0;
  }

  //  fprintf(stderr, "DEBUG: Exiting ThreadHandler()\n");
  TermThreads();  // Tell everyone else to exit too

  return 0;
}

void Game::ResolveRound() {
  // FIXME: Real Spread/Strenghten of Smoke and Fire
  map<MapCoord, MapObject>::iterator obj = master.objects.begin();
  for (; obj != master.objects.end(); ++obj) {
    if (!(obj->second.burn.empty())) {  // Strenghten/Spread/Dissipate
      Uint8 burn = (obj->second.burn.rbegin()->second);
      if (burn > 0) {
        burn--;
        if (burn == 63) burn = 15;  // Fire just went out.
        obj->second.burn[master.round + 1] = burn;
      }
    }
  }

  multiset<UnitAct> toact;
  GetActions(toact);
  CommitActions(toact);

  map<int, Unit *>::iterator unit = units.begin();
  for (; unit != units.end(); ++unit) {
    if (master.my_units[unit->second->id].back().act != ACT_FALL) {
      master.FillActionsTo(unit->first, master.round * 3000);
    }
  }

  ++(master.round);
}

void Game::CommitActions(const multiset<UnitAct> &toact) {
  // FIXME: This is where the game system goes!
  multiset<UnitAct>::const_iterator act = toact.begin();
  for (; act != toact.end(); ++act) {
    if (master.my_units[act->id].back().act == ACT_FALL) {
      continue;  // The dead can't act
    }
    master.AddAction(*act);
  }
}

void Game::GetActions(multiset<UnitAct> &toact) {
  set<int> ordered;
  vector<Player *>::const_iterator itrp = player.begin();
  for (; itrp != player.end(); ++itrp) {
    int pnum = (*itrp)->ID();
    if (orders.count(pnum) < 1) {
      fprintf(stderr, "ERROR: Player %d's orders got lost!\n", pnum);
      TermThreads();
    } else {
      map<UnitOrder, bool>::const_iterator ordit = orders[pnum]->orders.begin();
      for (; ordit != orders[pnum]->orders.end(); ++ordit) {
        //	fprintf(stderr, "ORDER[Player%d], %d do %d at %d to (%d,%d)\n",
        // pnum,
        //		order->id, order->order, order->time,
        //		order->targ1, order->targ2);
        const UnitOrder *order = &(ordit->first);
        if (master.my_units[order->id].back().act == ACT_FALL) {
          continue;
        }

        int x = 0, y = 0, z = 0;
        master.GetPos(order->id, x, y, z);
        Uint32 offset = (rand() % 500) + 500;
        switch (order->order) {
          case (ORDER_RUN):
          case (ORDER_MOVE): {
            Act act = ACT_MOVE;
            Uint32 steps[4] = {0, 333, 471, 577};
            if (order->order == ORDER_RUN) {
              act = ACT_RUN;
              steps[1] = 167;
              steps[2] = 236;
              steps[3] = 289;
            }
            MapCoord start = {x, y, z};
            MapCoord end = {order->targ1, order->targ2, order->targ3};
            vector<MapCoord> path = master.GetPath(start, end);
            if (path.size() > 1) {
              vector<MapCoord>::const_iterator pt = path.begin();
              vector<MapCoord>::const_iterator lpt = pt;
              ++pt;
              for (; pt != path.end(); ++pt) {
                Uint32 step = steps[abs(pt->x - lpt->x) + abs(pt->y - lpt->y) +
                                    abs(pt->z - lpt->z)];
                float ang =
                    180.0 * atan2f(pt->y - lpt->y, pt->x - lpt->x) / M_PI;
                offset += step;
                if (offset > 3000) break;  // One Round at a Time!
                toact.insert(UnitAct(
                    order->id, (master.round - 1) * 3000 + order->time + offset,
                    step, pt->x, pt->y, pt->z, ang, act, lpt->x, lpt->y,
                    lpt->z));
                lpt = pt;
              }
              if (pt == path.end()) orders[pnum]->Completed(*order);
            } else {
              orders[pnum]->Completed(*order);
            }
            ordered.insert(order->id);
          } break;
          case (ORDER_EQUIP): {
            if (master.round != 1 || order->time != 0) {  // Not Initial EQUIP
              if (ordered.count(order->id) <= 0) {
                toact.insert(UnitAct(
                    order->id, (master.round - 1) * 3000 + order->time + offset,
                    0, x, y, z, 0.0, ACT_EQUIP, order->targ1, order->targ2,
                    order->targ3));
                ordered.insert(order->id);
              }
            } else {  // Initial Equip is Free
              if (ordered.count(order->id) <= 0) {
                toact.insert(UnitAct(order->id, 0, 0, x, y, z, 0.0, ACT_EQUIP,
                                     order->targ1, order->targ2, order->targ3));
                ordered.insert(order->id);
              }
            }
            orders[pnum]->Completed(*order);
          } break;
          case (ORDER_SHOOT): {
            int hit = 0;
            if (order->targ1 < 0) {
              hit = order->targ2;
            } else {
              hit = master.UnitAt(order->targ1, order->targ2, order->targ3);
            }
            if (hit > 0) {
              int tx, ty, tz;
              float ang;
              master.GetPos(hit, tx, ty, tz, ang);
              toact.insert(UnitAct(
                  hit, (master.round - 1) * 3000 + order->time + 350 + offset,
                  100, tx, ty, tz, ang, ACT_FALL));
              ordered.insert(hit);

              ang = 180.0 * atan2f(tx - x, ty - y);
              toact.insert(UnitAct(order->id, (master.round - 1) * 3000 +
                                                  order->time + offset + 100,
                                   100, x, y, z, ang, ACT_SHOOT, tx, ty, tz));
            } else {
              float ang = 180.0 * atan2f(order->targ2 - x, order->targ1 - y);
              toact.insert(UnitAct(order->id, (master.round - 1) * 3000 +
                                                  order->time + offset + 100,
                                   100, x, y, z, ang, ACT_SHOOT, order->targ1,
                                   order->targ2, order->targ3));
            }
            orders[pnum]->Completed(*order);
            ordered.insert(order->id);
          } break;
          default: {
            fprintf(stderr, "WARNING: Got unknown ORDER from Player%d\n", pnum);
          } break;
        }
      }
    }
    orders[pnum]->ClearCompleted();
  }
}
