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
    if(!(game->PerceptUpToDate(id))) {
      game->UpdatePercept(id);
      //SDL_Delay(10);

      if(percept.round == 0) {
	map<int, vector<UnitAct> >::iterator unit = percept.my_units.begin();
	for(; unit != percept.my_units.end(); ++unit) {
	  if(unit->second.back().act == ACT_EQUIP) {
	    orders.AddOrder(unit->first, 0, ORDER_EQUIP);
	    }
	  //SDL_Delay(10);
	  }
	}
      else if(percept.round >= 1 && percept.round < 5) { // Everyone Run South
	map<int, vector<UnitAct> >::iterator unit = percept.my_units.begin();
	for(; unit != percept.my_units.end(); ++unit) {
	  orders.AddOrder(unit->first, 0, ORDER_RUN,
		unit->second.back().x, unit->second.back().y - 6);
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
