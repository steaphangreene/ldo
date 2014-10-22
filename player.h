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

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
using namespace std;

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
  Player(Game *gm, PlayerType tp, int num, int c = 0);
  virtual ~Player();

  void SetColor(int c) { color = c; };

  PlayerType Type() { return type; };
  int ID() { return id; };
  int Color() { return color; };

  virtual bool Run();

 protected:
  int id;
  PlayerType type;
  Percept percept;
  Orders orders;
  int color;
  Game *game;
};

#endif  // PLAYER_H
