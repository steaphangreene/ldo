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

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
using namespace std;

#include <SDL/SDL.h>

#include "../simplegui/simplegui.h"

#include "percept.h"
#include "orders.h"

class Game;

enum PlayerType {
  PLAYER_GONE,
  PLAYER_SPECTATOR,
  PLAYER_LOCAL,
  PLAYER_AI,
  PLAYER_REMOTE,
  PLAYER_MAX
  };

class Player {
public:
  Player(Game *gm, PlayerType tp, int num);
  virtual ~Player();

  bool Ready();
  PlayerType Type() { return type; };
  int ID() { return id; };

  virtual bool Run();

protected:
  int id;
  PlayerType type;
  bool ready;
  Percept percept;
  Orders orders;
  int pround;
  Game *game;
  };

class Sound;

enum Phase {
  PHASE_NONE,
  PHASE_EQUIP,
  PHASE_REPLAY,
  PHASE_DECLARE,
  PHASE_MAX,
  PHASE_OPTION_EQUIP,
  PHASE_OPTION_REPLAY,
  PHASE_OPTION_DECLARE,
  PHASE_OPTION_MAX
  };
#define PHASE_OPTION (PHASE_OPTION_MAX - PHASE_MAX)

class Player_Local : public Player {
public:
  Player_Local(Game *gm, PlayerType tp, int num);
  virtual ~Player_Local();
  virtual bool Run();

protected:
  void UpdateEquipIDs();

  Phase phase;

  SimpleGUI *gui;

  SG_Table *wind[PHASE_MAX];	//Screens for each phase
  int drkred;			//Colors

  SG_MultiTab *ednd;		//Widgets for Equip phase
  SG_TextArea *estats;
  SG_Button *ecancelb, *edoneb;
  vector<int> eqid;		//Data for Equip phase

				//Textures for Equip phase
  SDL_Surface *but_normal, *but_pressed, *but_disabled, *but_activated;
  SDL_Surface *gun_icon, *gren_icon;
  SDL_Surface *equip_bg;


  SG_Button *roptb, *rdoneb;	//Widgets for Replay phase

  SG_Button *doptb, *ddoneb;	//Widgets for Declare phase

  int music;			//Background Music (Temporary)
  Sound *cur_music;		//Currently Playing Music (Temporary)
  };

#endif // PLAYER_H
