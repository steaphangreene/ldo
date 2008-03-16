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

#include "player_ai.h"
#include "game.h"

Player_AI::Player_AI(Game *gm, PlayerType tp, int num, int c)
	: Player(gm, tp, num, c) {
  }

Player_AI::~Player_AI() {
  }

bool Player_AI::Run() {
  Player::Run();	// Start with the basics

  int exiting = 0;
  while(exiting == 0) {
    if(pround != percept.round) {
      pround = percept.round;
      game->UpdatePercept(id, pround);
      //SDL_Delay(10);

      if(pround == 0) {
	map<int, vector<UnitAct> >::iterator unit = percept.my_units.begin();
	for(; unit != percept.my_units.end(); ++unit) {
	  if(unit->second.back().act == ACT_EQUIP) {
	    orders.orders.push_back(UnitOrder(unit->first, 0, ORDER_EQUIP));
	    }
	  //SDL_Delay(10);
	  }
	}
      else if(pround >= 1 && pround < 5) {	// Everyone Run South
	map<int, vector<UnitAct> >::iterator unit = percept.my_units.begin();
	for(; unit != percept.my_units.end(); ++unit) {
	  orders.orders.push_back(UnitOrder(unit->first, 0, ORDER_RUN,
		unit->second.back().x, unit->second.back().y - 6));
	  //SDL_Delay(10);
	  }
	}

      game->SetReady(id, true);
      //SDL_Delay(10);
      }
    if(game->ShouldTerm()) exiting = 1;
    SDL_Delay(10);
    }

  game->TermThreads();	// Tell everyone else to exit too

  return exiting;
  }
