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

#include "player_local.h"
#include "renderer.h"

#include "game.h"

#include "m41.h"
#include "mark2.h"
#define TGA_COLFIELDS SG_COL_U32B3, SG_COL_U32B2, SG_COL_U32B1, SG_COL_U32B4

extern Game *cur_game;

Player_Local::Player_Local(Game *gm, PlayerType tp, int num)
	: Player(gm, tp, num) {
  gui = SimpleGUI::CurrentGUI(); //Yes, this is ok, it's static!
  if(!gui) {
    //FIXME: Initialize GUI myself if it's not already done for me!
    }

  music = audio_loadmusic("music/iconoclasm.wav");
  cur_music = NULL;

  phase = PHASE_NONE;
  popphase = POPPHASE_NONE;

  drkred = gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);

  but_normal = SDL_LoadBMP("buttontex_normal.bmp");
  but_pressed = SDL_LoadBMP("buttontex_pressed.bmp");
  but_disabled = SDL_LoadBMP("buttontex_disabled.bmp");
  but_activated = SDL_LoadBMP("buttontex_activated.bmp");
  gun_icon = SDL_CreateRGBSurfaceFrom(m41, 170, 256, 32, 170*4, TGA_COLFIELDS);
  gren_icon = SDL_CreateRGBSurfaceFrom(mark2, 256, 256, 32, 256*4, TGA_COLFIELDS);
  equip_bg = SDL_LoadBMP("equip_bg.bmp");

  //Define base GUI for Equip phase
  wind[PHASE_EQUIP] = new SG_Table(16, 9, 0.0, 0.0);
  ecancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  wind[PHASE_EQUIP]->AddWidget(ecancelb, 12, 0, 2, 1);
  edoneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  wind[PHASE_EQUIP]->AddWidget(edoneb, 14, 0, 2, 1);
  estats = new SG_TextArea("", drkred);
  wind[PHASE_EQUIP]->AddWidget(estats, 12, 1, 4, 1);
  ednd = NULL;

  //Define base GUI for Replay phase
  wind[PHASE_REPLAY] = new SG_Table(6, 7, 0.0625, 0.125);
  roptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[PHASE_REPLAY]->AddWidget(roptb, 0, 6);
  rdoneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  wind[PHASE_REPLAY]->AddWidget(rdoneb, 5, 6);
  wind[PHASE_REPLAY]->AddWidget(new SG_TextArea("Play/Replay Turn", drkred), 1, 3, 4, 1);

  //Define base GUI for Declare phase
  wind[PHASE_DECLARE] = new SG_Table(6, 7, 0.0625, 0.125);
  doptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[PHASE_DECLARE]->AddWidget(doptb, 0, 6);
  ddoneb = new SG_StickyButton("Ready", but_normal, but_disabled, but_pressed, but_activated);
  wind[PHASE_DECLARE]->AddWidget(ddoneb, 5, 6);
  wind[PHASE_DECLARE]->AddWidget(new SG_TextArea("Define Next Turn", drkred), 1, 3, 4, 1);
  }

Player_Local::~Player_Local() {
  }

bool Player_Local::Run() {
  Player::Run();	// Start with the basics

  if(!cur_music) cur_music = audio_loop(music, 16, 0);

  UpdateEquipIDs();	// See if we need to do the Equip thing

  gui->MasterWidget()->AddWidget(wind[phase]);

  int ret = 0;
  SDL_Event event;
  while(ret == 0) {
    while(SDL_PollEvent(&event)) {
      if(!gui->ProcessEvent(&event)) continue;
      if(event.type == SDL_KEYDOWN) {
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  ret = 1;
	  }
	}
      else if(event.type == SDL_SG_EVENT) {
	if(event.user.code == SG_EVENT_BUTTONCLICK) {
	  if(event.user.data1 == (void*)edoneb) {

	    //FIXME: Actually get the EQUIP setup from DNDBoxes Widgets

	    vector<int>::iterator id = eqid.begin();
	    for(; id != eqid.end(); ++id) {
	      orders.orders.push_back(UnitOrder(*id, 0, ORDER_EQUIP));
	      }

	    gui->MasterWidget()->RemoveWidget(wind[phase]);
	    UpdateEquipIDs();	// See if we still need to do the Equip thing
	    gui->MasterWidget()->AddWidget(wind[phase]);
	    }
	  else if(event.user.data1 == (void*)rdoneb) {
	    gui->MasterWidget()->RemoveWidget(wind[phase]);
	    phase = PHASE_DECLARE; //Move on to declaring orders for next turn
	    gui->MasterWidget()->AddWidget(wind[phase]);
	    }
	  }
	if(event.user.code == SG_EVENT_STICKYON) {
	  if(event.user.data1 == (void*)ddoneb) {
	    //FIXME: Actually submit the orders and wait for new percept

	    game->SetReady(id, true);

	    gui->MasterWidget()->RemoveWidget(wind[phase]);
	    ddoneb->TurnOff();	// Make sure "Ready" isn't checked next time
	    UpdateEquipIDs();	// See if we need to do the Equip thing again
	    gui->MasterWidget()->AddWidget(wind[phase]);
	    }
	  else {
	    ret = 1;  //Return
	    }
	  }
	else if(event.user.code == SG_EVENT_SELECT) {
	  const Unit *u = cur_game->UnitRef(eqid[*((int*)(event.user.data2))]);
	  if(u) estats->SetText(u->name);
	  }
	}
      }

    start_scene();
    gui->RenderStart(SDL_GetTicks());

    gui->RenderFinish(SDL_GetTicks());
    finish_scene();
    }

  gui->MasterWidget()->RemoveWidget(wind[phase]);

  if(cur_music) audio_stop(cur_music);
  cur_music = NULL;

  return ret;
  }

void Player_Local::UpdateEquipIDs() {
  set<int> eqtmp;	//Temporary set of ids for eq
  eqid.clear();

  vector<UnitAct>::iterator act = percept.my_acts.begin();
  for(; act != percept.my_acts.end(); ++act) {
    if(act->act == ACT_EQUIP) eqtmp.insert(act->id);
    }

  vector<UnitOrder>::iterator order = orders.orders.begin();
  for(; order != orders.orders.end(); ++order) {
    if(order->order == ORDER_EQUIP) eqtmp.erase(order->id);
    }

  int targ = -1;	//Each troop/group equips SEPARATELY!
  act = percept.my_acts.begin();
  for(; act != percept.my_acts.end(); ++act) {
    if(act->act == ACT_EQUIP && eqtmp.count(act->id)
	&& (targ == -1 || targ == act->targ1)) {
      targ = act->targ1;
      eqid.push_back(act->id);
      }
    }

  vector<string> troops;
  vector<SG_Alignment *> dnds;
  vector<int>::iterator id = eqid.begin();
  for(; id != eqid.end(); ++id) {
    troops.push_back(game->UnitRef(*id)->name);

    if(ednd != NULL) {
      wind[PHASE_EQUIP]->RemoveWidget(ednd);
      delete ednd;
      ednd = NULL;
      }

    SG_DNDBoxes *dnd = new SG_DNDBoxes(18, 12);
    dnd->Include(1, 1, 2, 1);
    dnd->Include(7, 1, 2, 1);
    dnd->Include(11, 2, 3, 3);
    dnd->Include(0, 3, 2, 3);
    dnd->Include(8, 3, 2, 3);
    dnd->Include(0, 7, 2, 1);
    dnd->Include(8, 7, 2, 1);
    dnd->Include(11, 6, 4, 1);
    dnd->Include(11, 7);
    dnd->Include(14, 7);
    dnd->Include(0, 9, 18, 3);

      // Hardcoded loadout for now - Temporary!
    if(troops.size() != 2) dnd->AddItem(gun_icon, 8, 3, 2, 3);
    if(troops.size() != 1) dnd->AddItem(gren_icon, 11, 7);

    dnd->SetBackground(new SG_Panel(equip_bg));

    dnds.push_back(dnd);
    }

  if(troops.size() > 0) {
    phase = PHASE_EQUIP;
    if(ednd == NULL) {
      ednd = new SG_MultiTab(troops, dnds, 9,
	but_normal, but_disabled, but_pressed, but_activated);
      wind[PHASE_EQUIP]->AddWidget(ednd, 0, 0, 12, 9);
      estats->SetText(troops[0]);
      }
    }
  else {
    if(ednd != NULL) {
      wind[PHASE_EQUIP]->RemoveWidget(ednd);
      delete ednd;
      ednd = NULL;
      }
    phase = PHASE_REPLAY;
    }
  }
