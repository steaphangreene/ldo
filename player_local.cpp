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

double cur_zoom = 4.0;

#include "player_local.h"
#include "renderer.h"

#include "game.h"

#include "m41.h"
#include "mark2.h"
#define TGA_COLFIELDS SG_COL_U32B3, SG_COL_U32B2, SG_COL_U32B1, SG_COL_U32B4

Player_Local::Player_Local(Game *gm, PlayerType tp, int num)
	: Player(gm, tp, num) {
  gui = SimpleGUI::CurrentGUI(); //Yes, this is ok, it's static!
  if(!gui) {
    //FIXME: Initialize GUI myself if it's not already done for me!
    }

  world = new World(&percept, &orders);

  music = audio_loadmusic("music/iconoclasm.wav");
  cur_music = NULL;

  phase = PHASE_NONE;
  popphase = POPPHASE_NONE;
  nextphase = PHASE_NONE;
  nextpopphase = POPPHASE_NONE;

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
  wind[PHASE_REPLAY]->SetBackground(
	new SG_PassThrough(SG_PT_CLICK, SG_PT_MENU, SG_PT_MENU));
  roptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[PHASE_REPLAY]->AddWidget(roptb, 0, 6);
  rdoneb = new SG_Button("Ok", but_normal, but_disabled, but_pressed);
  wind[PHASE_REPLAY]->AddWidget(rdoneb, 5, 6);
  rtext = new SG_TransLabel("Play/Replay Turn", drkred);
  rtext->SetFontSize(30);
  wind[PHASE_REPLAY]->AddWidget(rtext, 1, 3, 4, 1);
  rstamp = new SG_TransLabel("<Time Offset>", drkred);
  rstamp->SetFontSize(20);
  wind[PHASE_REPLAY]->AddWidget(rstamp, 2, 4, 2, 1);
  vector<string> conts;			//Temporary - until textures
  conts.push_back("<<");
  conts.push_back("<");
  conts.push_back("<|");
  conts.push_back("||");
  conts.push_back("|>");
  conts.push_back(">");
  conts.push_back(">>");
  rcontrols = new SG_Tabs(conts, SG_AUTOSIZE, 1);
  rcontrols->SetBorder(0.0625, 0.0);	//Temporary - until textures
  wind[PHASE_REPLAY]->AddWidget(rcontrols, 2, 6, 2, 1);

  //Define base GUI for Declare phase
  wind[PHASE_DECLARE] = new SG_Table(6, 7, 0.0625, 0.125);
  wind[PHASE_DECLARE]->SetBackground(
	new SG_PassThrough(SG_PT_CLICK, SG_PT_MENU, SG_PT_MENU));
  doptb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  wind[PHASE_DECLARE]->AddWidget(doptb, 0, 6);
  ddoneb = new SG_StickyButton("Ready", but_normal, but_disabled, but_pressed, but_activated);
  wind[PHASE_DECLARE]->AddWidget(ddoneb, 5, 6);
  dtext = new SG_TransLabel("Declare Next Turn (#1)", drkred);
  dtext->SetFontSize(30);
  wind[PHASE_DECLARE]->AddWidget(dtext, 1, 3, 4, 1);

  gui_mut = SDL_CreateMutex();
  off_mut = SDL_CreateMutex();
  }

Player_Local::~Player_Local() {
  SDL_DestroyMutex(off_mut);
  SDL_DestroyMutex(gui_mut);
  delete world;
  }

int Player_Local::event_thread_func(void *arg) {
  return ((Player_Local *)(arg))->EventHandler();
  }

int Player_Local::EventHandler() {
  SDL_Event event;
  while(exiting == 0) {
    while(exiting == 0 && SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) {
      SDL_mutexP(gui_mut);
      int handle = gui->ProcessEvent(&event);
      SDL_mutexV(gui_mut);
      if(!handle) {
	continue;
	}

      if(event.type == SDL_QUIT) {
	exiting = 1;
	}
      else if(event.type == SDL_KEYDOWN) {
	if(event.key.keysym.sym == SDLK_ESCAPE) {
	  exiting = 1;
	  }
	}
      else if(event.type == SDL_SG_EVENT) {
	if(event.user.code == SG_EVENT_BUTTONCLICK) {
	  if(event.user.data1 == (void*)edoneb) {

	  //FIXME: Actually get the EQUIP setup from DNDBoxes Widgets

	    vector<int>::iterator id = eqid.begin();
	    for(; id != eqid.end(); ++id) {
		//FIXME: Could ALLOC in this thread (Could be BAD in Windows!)!
	      orders.orders.push_back(UnitOrder(*id, 0, ORDER_EQUIP));
	      }
	    SDL_mutexP(off_mut);
	    nextphase = PHASE_REPLAY;
	    last_time = SDL_GetTicks();
	    last_offset = 0;
	    playback_speed = 5; //Default is play
	    SDL_mutexP(gui_mut);
	    rcontrols->Set(playback_speed);
	    SDL_mutexV(gui_mut);
	    SDL_mutexV(off_mut);
	    }
	  else if(event.user.data1 == (void*)rdoneb) {
	    nextphase = PHASE_DECLARE; //Move on to declaring orders for next turn
	    }
	  }

	else if(event.user.code == SG_EVENT_STICKYON) {
	  if(event.user.data1 == (void*)ddoneb) {
	    if(!game->SetReady(id, true)) {
	      SDL_mutexP(gui_mut);
	      ddoneb->TurnOff(); // Reject this attempt
	      SDL_mutexV(gui_mut);
	      }
	    }
	  else {
	    exiting = 1;  //Return
	    }
	  }
	else if(event.user.code == SG_EVENT_STICKYOFF) {
	  if(event.user.data1 == (void*)ddoneb) {
	    if(game->SetReady(id, false)) {
	      SDL_mutexP(gui_mut);
	      ddoneb->TurnOn(); // Reject this attempt
	      SDL_mutexV(gui_mut);
	      }
	    }
	  else {
	    exiting = 1;  //Return
	    }
	  }
	else if(event.user.code == SG_EVENT_SELECT) {
	  if(event.user.data1 == (void*)(ednd)) {
	    const Unit *u = game->UnitRef(eqid[*((int*)(event.user.data2))]);
	    if(u != NULL) {
	      SDL_mutexP(gui_mut);
	      estats->SetText(u->name);
	      SDL_mutexV(gui_mut);
	      }
	    }
	  else if(event.user.data1 == (void*)(rcontrols)) {
	    SDL_mutexP(off_mut);
	    Uint32 cur_time = SDL_GetTicks();
	    if(offset == 3000 && playback_speed == 3	//Re-play
		&& *((int*)(event.user.data2)) > 3) {
	      offset = 0;
	      }
	    else if(offset == 0 && playback_speed == 3	//Re-reverse
		&& *((int*)(event.user.data2)) < 3) {
	      offset = 3000;
	      }
	    else {
	      CalcOffset(cur_time);
	      }
	    playback_speed = *((int*)(event.user.data2));
	    last_time = cur_time;
	    last_offset = offset;
	    SDL_mutexV(off_mut);
	    }
	  }
	else if(event.user.code == SG_EVENT_SCROLLUP) {
	  cur_zoom -= 0.0625;
	  if(cur_zoom < 1.0) cur_zoom = 1.0;
	  }
	else if(event.user.code == SG_EVENT_SCROLLDOWN) {
	  cur_zoom += 0.0625;
	  if(cur_zoom > 4.0) cur_zoom = 4.0;
	  }
	}
      }
    SDL_Delay(10); // Weak, yes, but this is how SDL does it too. :(
    }
  return exiting;
  }

static char buf[256];

bool Player_Local::Run() {
  Player::Run();	// Start with the basics

  if(!cur_music) cur_music = audio_loop(music, 16, 0);

  UpdateEquipIDs();	// See if we need to do the Equip thing

  gui->MasterWidget()->AddWidget(wind[phase]);

  exiting = 0;
  SDL_Thread *th = SDL_CreateThread(event_thread_func, (void*)(this));

  while(exiting == 0) {
    SDL_PumpEvents();
    if(pround != game->CurrentRound() - 1) {
      pround = game->CurrentRound() - 1;
      game->UpdatePercept(id, pround);

      SDL_mutexP(gui_mut);

      ddoneb->TurnOff(); // Make sure "Ready" isn't checked next time

      UpdateEquipIDs();	 // See if we need to do the Equip thing again

      sprintf(buf, "Declare Next Turn (#%d)%c", game->CurrentRound(), 0);
      dtext->SetText(buf);
      sprintf(buf, "Play/Replay Turn (#%d)%c", game->CurrentRound()-1, 0);
      rtext->SetText(buf);

      SDL_mutexV(gui_mut);
      }

    if(phase != nextphase) {
      SDL_mutexP(gui_mut);
      if(nextphase == PHASE_REPLAY) UpdateEquipIDs();
      gui->MasterWidget()->RemoveWidget(wind[phase]);
      phase = nextphase;
      gui->MasterWidget()->AddWidget(wind[phase]);
      SDL_mutexV(gui_mut);
      }
    Uint32 cur_time = SDL_GetTicks();

    if(phase == PHASE_REPLAY) {
      SDL_mutexP(off_mut);
      CalcOffset(cur_time);
      sprintf(buf, "%d.%.3d seconds%c", offset / 1000, offset % 1000, 0);
      SDL_mutexV(off_mut);
      rstamp->SetText(buf);
      }

    SDL_PumpEvents();
    start_scene(cur_zoom);

    SDL_mutexP(gui_mut);
    gui->RenderStart(cur_time);
    SDL_mutexV(gui_mut);

    SDL_PumpEvents();
    if(phase == PHASE_DECLARE) {
      world->Render();
      }
    else if(phase == PHASE_REPLAY) {
      world->Render(offset);
      }

    SDL_PumpEvents();
    SDL_mutexP(gui_mut);
    gui->RenderFinish(cur_time);
    SDL_mutexV(gui_mut);

    SDL_PumpEvents();
    finish_scene();
    }

  SDL_WaitThread(th, NULL);

  gui->MasterWidget()->RemoveWidget(wind[phase]);

  if(cur_music) audio_stop(cur_music);
  cur_music = NULL;

  game->TermThreads();	// Tell everyone else to exit too

  return exiting;
  }

	//Note: gui_mut must be locked before calling this function!
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
    nextphase = PHASE_EQUIP;
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
    if(game->CurrentRound() == 1) nextphase = PHASE_DECLARE;
    else {
      SDL_mutexP(off_mut);
      nextphase = PHASE_REPLAY;
      last_time = SDL_GetTicks();
      last_offset = 0;
      playback_speed = 5; //Default is play
      SDL_mutexP(gui_mut);
      rcontrols->Set(playback_speed);
      SDL_mutexV(gui_mut);
      SDL_mutexV(off_mut);
      }
    }
  }

	//Note: off_mut must be locked before calling this function!
void Player_Local::CalcOffset(Uint32 cur_time) { // off_mut must
  switch(playback_speed) {
    case(0): {	// Rewind
      offset = last_offset - (cur_time - last_time) * 4;
      }break;
    case(1): {	// Backward
      offset = last_offset - (cur_time - last_time);
      }break;
    case(2): {	// SlowBack
      offset = last_offset - (cur_time - last_time) / 4;
      }break;
    case(3): {	// Pause
      //Do nothing.
      }break;
    case(4): {	// Slow
      offset = last_offset + (cur_time - last_time) / 4;
      }break;
    case(5): {	// Play
      offset = last_offset + (cur_time - last_time);
      }break;
    case(6): {	// Forward
      offset = last_offset + (cur_time - last_time) * 4;
      }break;
    default: {
      fprintf(stderr, "Warning, unknown control setting %d\n",
	playback_speed);	// Should never happen
      }break;
    }
  if(offset > 2147483647U) {	//Compare to INT_MAX, since is unsigned
    offset = 0;
    playback_speed = 3; //Auto-stop
    SDL_mutexP(gui_mut);
    rcontrols->Set(playback_speed);
    SDL_mutexV(gui_mut);
    }
  else if(offset > 3000) {
    offset = 3000;
    playback_speed = 3; //Auto-stop
    SDL_mutexP(gui_mut);
    rcontrols->Set(playback_speed);
    SDL_mutexV(gui_mut);
    }
  SDL_mutexV(off_mut);
  }