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

#include "../simplegui/simplegui.h"
#include "renderer.h"

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

Player_Local::Player_Local(Game *gm, PlayerType tp, int num)
	: Player(gm, tp, num) {
  gui = SimpleGUI::CurrentGUI(); //Yes, this is ok, it's static!
  if(!gui) {
    //FIXME: Initialize GUI myself if it's not already done for me!
    }
  phase = -1;

  SDL_Surface *but_normal, *but_pressed, *but_disabled, *but_activated;
  but_normal = SDL_LoadBMP("buttontex_normal.bmp");
  but_pressed = SDL_LoadBMP("buttontex_pressed.bmp");
  but_disabled = SDL_LoadBMP("buttontex_disabled.bmp");
  but_activated = SDL_LoadBMP("buttontex_activated.bmp");
  equip_bg = SDL_LoadBMP("equip_bg.bmp");

  //Define base GUI for Equip phase
  wind[0] = new SG_Table(16, 9, 0.0, 0.0);
  ecancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  wind[0]->AddWidget(ecancelb, 12, 0, 2, 1);
  edoneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  wind[0]->AddWidget(edoneb, 14, 0, 2, 1);
  ednd = NULL;

  //Define base GUI for Replay phase
  wind[1] = new SG_Table(6, 7, 0.0625, 0.125);
  roptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[1]->AddWidget(roptb, 0, 6);
  rdoneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  wind[1]->AddWidget(rdoneb, 5, 6);

  //Define base GUI for Declare phase
  wind[2] = new SG_Table(6, 7, 0.0625, 0.125);
  doptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[2]->AddWidget(doptb, 0, 6);
  ddoneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  wind[2]->AddWidget(ddoneb, 5, 6);
  }

Player_Local::~Player_Local() {
  }

bool Player_Local::Run() {
  Player::Run();	//Start with the basics

  //Temporary - for structure testing!
  fprintf(stderr, "Player_Local Running\n");

  if(phase == -1) {	//Temporary (should check for ACT_EQUIP)!
    if(pround == 0) phase = 0;	//EQUIP
    else phase = 1;		//REPLAY
    }

  gui->MasterWidget()->AddWidget(wind[phase]);

  int ret = 0;
  SDL_Event event;
  while(ret == 0) {
    while(SDL_PollEvent(&event)) {
      if(!gui->ProcessEvent(&event)) continue;
      if(event.type == SDL_KEYDOWN) {
	ret = 1;
	}
      else if(event.type == SDL_SG_EVENT) {
	ret = 1;
	}
      }

//    if(phase == 0) HandleEquip();
//    else if(phase == 1) HandleReplay();
//    else if(phase == 2) HandleDeclare();

    start_scene();
    gui->RenderStart(SDL_GetTicks());

    gui->RenderFinish(SDL_GetTicks());
    finish_scene();
    }

  gui->MasterWidget()->RemoveWidget(wind[phase]);

  ready = true;
  return ret;
  }
