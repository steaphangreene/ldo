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

#include "player.h"
#include "game.h"

extern Game *cur_game;

Player::Player(Game *gm, PlayerType tp, int num) {
  game = gm;
  type = tp;
  id = num;

  ready = false;
  pround = -1;

  cur_game->SetPercept(num, &percept);
  cur_game->SetOrders(num, &orders);
  }

Player::~Player() {
  }

bool Player::Ready() {
  return ready;
  }

bool Player::Run() {
  if(game->CurrentRound() - 1 != pround) {
    pround = game->CurrentRound() - 1;
    game->UpdatePercept(id, pround);
    ready = false;
    }
  return true;
  }
