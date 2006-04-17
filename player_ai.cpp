// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2006 Steaphan Greene <stea@cs.binghamton.edu>
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

#include "player_ai.h"
#include "game.h"

Player_AI::Player_AI(Game *gm, PlayerType tp, int num)
	: Player(gm, tp, num) {
  }

Player_AI::~Player_AI() {
  }

bool Player_AI::Run() {
  Player::Run();	// Start with the basics

  int exiting = 0;
  while(exiting == 0) {
    if(pround != game->CurrentRound() - 1) {
      pround = game->CurrentRound() - 1;
      game->UpdatePercept(id, pround);
      SDL_Delay(10);

      if(pround == 0) {
	vector<UnitAct>::iterator act = percept.my_acts.begin();
	for(; act != percept.my_acts.end(); ++act) {
	  if(act->act == ACT_EQUIP) {
	    orders.orders.push_back(UnitOrder(act->id, 0, ORDER_EQUIP));
	    }
	  SDL_Delay(10);
	  }
	}

      game->SetReady(id, true);
      SDL_Delay(10);
      }
    if(game->ShouldTerm()) exiting = 1;
    SDL_Delay(10);
    }

  game->TermThreads();	// Tell everyone else to exit too

  return exiting;
  }