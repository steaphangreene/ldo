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

#include <cstdlib>
using namespace std;

#include "SDL_image.h"

double cur_zoom = 16.0, xspd = 0.0, yspd = 0.0, cur_ang = 45.0, cur_down = 22.5;
int cur_zpos = 0;
int sel_x = -1, sel_y = -1, sel_z = -1, sel_id = -1;
int mouse_x = -1, mouse_y = -1;

#include "player_local.h"

#include "game.h"

#define MOVE_SPEED 10.0	//Cells-per-second
#define MOVE_DELAY 250
#define ZOOM_DELAY 250
#define ROT_DELAY 500

extern Game *cur_game;
extern int click;

Player_Local::Player_Local(Game *gm, PlayerType tp, int num, int c)
	: Player(gm, tp, num, c) {
  gui = SimpleGUI::Current(); //Yes, this is ok, it's static!
  if(!gui) {
    //FIXME: Initialize GUI myself if it's not already done for me!
    }
  video = SimpleVideo::Current(); //Yes, this is ok, it's static!
  if(!video) {
    //FIXME: Initialize video myself if it's not already done for me!
    }
  scene = SimpleScene::Current(); //Yes, this is ok, it's static!
  if(!scene) {
    scene = new SimpleScene();
    }
  audio = SimpleAudio::Current(); //Yes, this is ok, it's static!
  if(!audio) {
    //FIXME: Initialize audio myself if it's not already done for me!
    }

  world = new World(&percept, &orders, id);

  music = audio->LoadMusic("music/iconoclasm.ogg");
  cur_music = -1;
  past = 0;

  phase = PHASE_NONE;
  popphase = POPPHASE_NONE;
  nextphase = PHASE_NONE;
  nextpopphase = POPPHASE_NONE;

  drkred = gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);

  gun_icon = IMG_Load("graphics/m41.png");
  gren_icon = IMG_Load("graphics/mark2.png");
  equip_bg = IMG_Load("graphics/equip_bg.png");

  //Define base GUI for Equip phase
  wind[PHASE_EQUIP] = new SG_Table(16, 24, 0.0, 0.0);
  ecancelb = new SG_Button("Cancel");
  wind[PHASE_EQUIP]->AddWidget(ecancelb, 12, 0, 2, 1);
  edoneb = new SG_Button("Done");
  wind[PHASE_EQUIP]->AddWidget(edoneb, 14, 0, 2, 1);
  estats = new SG_TextArea("", drkred);
  wind[PHASE_EQUIP]->AddWidget(estats, 12, 1, 4, 1);
  ednd = NULL;

  //Define base GUI for Replay phase
  wind[PHASE_REPLAY] = new SG_Table(48, 14, 0.0625, 0.125);
  wind[PHASE_REPLAY]->SetBackground(
	new SG_PassThrough(SG_PT_CLICK, SG_PT_CLICK, SG_PT_CLICK));
  roptb = new SG_StickyButton("Options");
  wind[PHASE_REPLAY]->AddWidget(roptb, 0, 13, 8, 1);
  rpastb = new SG_StickyButton("+");
  rpastb->SetAlignment(SG_ALIGN_CENTER);
  wind[PHASE_REPLAY]->AddWidget(rpastb, 14, 13, 2, 1);
  rdoneb = new SG_Button("Ok");
  wind[PHASE_REPLAY]->AddWidget(rdoneb, 40, 13, 8, 1);
  rtext = new SG_TransLabel("Playback Turn", drkred);
  rtext->SetFontSize(50);
  rtext->SetAlignment(SG_ALIGN_CENTER);
  wind[PHASE_REPLAY]->AddWidget(rtext, 8, 12, 32, 1);
  rstamp = new SG_TransLabel("<Time Offset>", drkred);
  rstamp->SetFontSize(50);
  rstamp->SetAlignment(SG_ALIGN_CENTER);
  wind[PHASE_REPLAY]->AddWidget(rstamp, 8, 11, 32, 1);
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
  rcontrols->SetAlignment(SG_ALIGN_CENTER);
  wind[PHASE_REPLAY]->AddWidget(rcontrols, 16, 13, 16, 1);

  //Define base GUI for Declare phase
  wind[PHASE_DECLARE] = new SG_Table(6, 14, 0.0625, 0.125);
  dpass = new SG_PassThrough(SG_PT_CLICK, SG_PT_MENU, SG_PT_MENU);
  wind[PHASE_DECLARE]->SetBackground(dpass);
  dpass->SetSendMotion();

  maction = 0;
  mactions[0].push_back("View");
  mactions[0].push_back("Stats");
  mactions[1].push_back("View");
  mactions[1].push_back("Stats");
  mactions[2].push_back("View");
  mactions[2].push_back("Stats");
  raction = 0;
  ractions[0].push_back("<No Unit Selected>");
  ractions[1].push_back("Go Here");
  ractions[1].push_back("Run Here");
  ractions[1].push_back("Throw Here");
  ractions[1].push_back("Shoot Here");
  ractions[2].push_back("Shoot At");
  ractions[2].push_back("Throw At");
  dpass->SetMenu(2, mactions[maction]);
  dpass->SetMenu(3, ractions[raction]);
  targ_id = -1;

  doptb = new SG_StickyButton("Options");
  wind[PHASE_DECLARE]->AddWidget(doptb, 0, 13);
  ddoneb = new SG_StickyButton("Ready");
  wind[PHASE_DECLARE]->AddWidget(ddoneb, 5, 13);
  dtext = new SG_TransLabel("Declare Turn (#1)", drkred);
  dtext->SetFontSize(50);
  dtext->SetAlignment(SG_ALIGN_CENTER);
  wind[PHASE_DECLARE]->AddWidget(dtext, 1, 12, 4, 1);

//  //Temporary!
//  SG_Widget *tmp = wind[PHASE_DECLARE];
//  wind[PHASE_DECLARE] = new SG_Table(20, 20, 0.0, 0.0);
//  wind[PHASE_DECLARE]->AddWidget(tmp, 2, 0, 12, 12);
//  wind[PHASE_DECLARE]->AddWidget(new SG_Panel(), 0, 0, 2, 20);
//  wind[PHASE_DECLARE]->AddWidget(new SG_Panel(), 0, 12, 20, 8);
//  wind[PHASE_DECLARE]->AddWidget(new SG_Panel(), 14, 0, 6, 20);

  //Define configuration GUI
  config_gui = new SimpleConfig;

  vid_mut = SDL_CreateMutex();
  off_mut = SDL_CreateMutex();
  }

Player_Local::~Player_Local() {
  SDL_DestroyMutex(vid_mut);
  SDL_DestroyMutex(off_mut);
  delete world;
  }

int Player_Local::event_thread_func(void *arg) {
  return ((Player_Local *)(arg))->EventHandler();
  }

int Player_Local::EventHandler() {
  SDL_Event event;
  while(exiting == 0) {
    if(!gui->WaitEvent(&event, true)) {
      fprintf(stderr, "ERROR: Event System Failure!\n");
      exit(1);
      }

    if(event.type == SDL_QUIT) {
      exiting = 1;
      }
    else if(event.type == SDL_KEYDOWN) {
      if(event.key.keysym.sym == SDLK_ESCAPE) {
	exiting = 1;
	}
      else if(event.key.keysym.sym == SDLK_RIGHT) {
	xspd += MOVE_SPEED;
	if(xspd > MOVE_SPEED) xspd = MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_LEFT) {
	xspd -= MOVE_SPEED;
	if(xspd < -MOVE_SPEED) xspd = -MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_UP) {
	yspd += MOVE_SPEED;
	if(yspd > MOVE_SPEED) yspd = MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_DOWN) {
	yspd -= MOVE_SPEED;
	if(yspd < -MOVE_SPEED) yspd = -MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_PAGEUP) {
	cur_ang += 90.0;
	SDL_mutexP(vid_mut);
	video->SetAngle(cur_ang, ROT_DELAY);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_PAGEDOWN) {
	cur_ang -= 90.0;
	SDL_mutexP(vid_mut);
	video->SetAngle(cur_ang, ROT_DELAY);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_KP_MINUS) {
	++cur_zpos;
	if(cur_zpos >= percept.mapzs) --cur_zpos;
	else {
	  SDL_mutexP(vid_mut);
	  video->SetZPosition(CELL_HEIGHT*cur_zpos, MOVE_DELAY);
	  SDL_mutexV(vid_mut);
	  }
	}
      else if(event.key.keysym.sym == SDLK_KP_PLUS) {
	--cur_zpos;
	if(cur_zpos < 0) ++cur_zpos;
	else {
	  SDL_mutexP(vid_mut);
	  video->SetZPosition(CELL_HEIGHT*cur_zpos, MOVE_DELAY);
	  SDL_mutexV(vid_mut);
	  }
	}
      }
    else if(event.type == SDL_KEYUP) {
      if(event.key.keysym.sym == SDLK_RIGHT) {
	xspd -= MOVE_SPEED;
	if(xspd < -MOVE_SPEED) xspd = -MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_LEFT) {
	xspd += MOVE_SPEED;
	if(xspd > MOVE_SPEED) xspd = MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_UP) {
 	yspd -= MOVE_SPEED;
	if(yspd < -MOVE_SPEED) yspd = -MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      else if(event.key.keysym.sym == SDLK_DOWN) {
	yspd += MOVE_SPEED;
	if(yspd > MOVE_SPEED) yspd = MOVE_SPEED;
	SDL_mutexP(vid_mut);
	video->SetMove(xspd, yspd);
	SDL_mutexV(vid_mut);
	}
      }
    else if(event.type == SDL_SG_EVENT) {
      if(event.user.code == SG_EVENT_BUTTONCLICK) {
	audio->Play(click);
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
	  last_offset = (pround - 2) * 3000;
	  playback_speed = 5; //Default is play
	  gui->Lock();
	  rcontrols->Set(playback_speed);
	  gui->Unlock();
	  SDL_mutexV(off_mut);
	  }
	else if(event.user.data1 == (void*)rdoneb) {
	  nextphase = PHASE_DECLARE; //Move on to declaring orders for next turn
	  }
	}

      else if(event.user.code == SG_EVENT_MENU + 3) {
	if(*((int*)event.user.data2) == 0 && raction == 1) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_MOVE, mouse_x, mouse_y, cur_zpos));
	  }
	else if(*((int*)event.user.data2) == 1 && raction == 1) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_RUN, mouse_x, mouse_y, cur_zpos));
	  }
	else if(*((int*)event.user.data2) == 2 && raction == 1) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_THROW, mouse_x, mouse_y, cur_zpos));
	  }
	else if(*((int*)event.user.data2) == 3 && raction == 1) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_SHOOT, mouse_x, mouse_y, cur_zpos));
	  }
	else if(*((int*)event.user.data2) == 0 && raction == 2) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_SHOOT, -1, targ_id));
	  }
	else if(*((int*)event.user.data2) == 1 && raction == 2) {
	  orders.orders.push_back(
		UnitOrder(sel_id, 0, ORDER_THROW, -1, targ_id));
	  }
//	fprintf(stderr, "Got right-menu event %d\n", *((int*)event.user.data2));
	}

      else if(event.user.code == SG_EVENT_MENU + 2) {
//	fprintf(stderr, "Got mid-menu event %d\n", *((int*)event.user.data2));
	}

      else if(event.user.code == SG_EVENT_STICKYON) {
	audio->Play(click);
	if(event.user.data1 == (void*)ddoneb) {
	  if(!game->SetReady(id, true)) {
	    gui->Lock();
	    ddoneb->TurnOff(); // Reject this attempt
	    gui->Unlock();
	    }
	  }
	else if(event.user.data1 == (void*)rpastb) {
	  past = 1;
	  }
	else if(event.user.data1 == (void*)roptb) {
	  gui->SetPopupWidget(config_gui);
	  }
	else if(event.user.data1 == (void*)doptb) {
	  gui->SetPopupWidget(config_gui);
	  }
	else {
	  exiting = 1;  //Return
	  }
	}
      else if(event.user.code == SG_EVENT_STICKYOFF) {
	audio->Play(click);
	if(event.user.data1 == (void*)ddoneb) {
	  if(game->SetReady(id, false)) {
	    gui->Lock();
	    ddoneb->TurnOn(); // Reject this attempt
	    gui->Unlock();
	    }
	  }
	else if(event.user.data1 == (void*)rpastb) {
	  past = 0;
	  }
	else if(event.user.data1 == (void*)roptb) {
	  gui->UnsetPopupWidget();
	  }
	else if(event.user.data1 == (void*)doptb) {
	  gui->UnsetPopupWidget();
	  }
	else {
	  exiting = 1;  //Return
	  }
	}
      else if(event.user.code == SG_EVENT_SELECT) {
	if(event.user.data1 == (void*)(ednd)) {
	  const Unit *u = game->UnitRef(eqid[*((int*)(event.user.data2))]);
	  if(u != NULL) {
	    gui->Lock();
	    estats->SetText(u->name);
	    gui->Unlock();
	    }
	  }
	else if(event.user.data1 == (void*)(rcontrols)) {
	  SDL_mutexP(off_mut);
	  Uint32 cur_time = SDL_GetTicks();
	  if((!past) && offset == (pround - 1) * 3000
		&& playback_speed == 3 && *((int*)(event.user.data2)) > 3) {
	    offset = (pround - 2) * 3000;		//Re-Play
	    }
	  else if((!past) && offset == (pround - 2) * 3000
		&& playback_speed == 3 && *((int*)(event.user.data2)) < 3) {
	    offset = (pround - 1) * 3000;		//Re-Reverse
	    }
	  else if(past && offset == (pround - 1) * 3000
		&& playback_speed == 3 && *((int*)(event.user.data2)) > 3) {
	    offset = 0;						//Re-Play All
	    }
	  else if(past && offset == 0
		&& playback_speed == 3 && *((int*)(event.user.data2)) < 3) {
	    offset = (pround - 1) * 3000;		//Re-Rev All
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
	cur_zoom -= 0.5;
	if(cur_zoom < 8.0) cur_zoom = 8.0;
	SDL_mutexP(vid_mut);
	video->SetZoom(cur_zoom, ZOOM_DELAY);
	SDL_mutexV(vid_mut);
	}
      else if(event.user.code == SG_EVENT_SCROLLDOWN) {
	cur_zoom += 0.5;
	if(cur_zoom > 32.0) cur_zoom = 32.0;
	SDL_mutexP(vid_mut);
	video->SetZoom(cur_zoom, ZOOM_DELAY);
	SDL_mutexV(vid_mut);
	}
      else if(event.user.code == SG_EVENT_LEFTCLICK) {
	double x = ((float*)(event.user.data2))[0];
	double y = ((float*)(event.user.data2))[1];
	SDL_mutexP(vid_mut);
	video->ScreenToMap(x, y, cur_zpos*CELL_HEIGHT);
	SDL_mutexV(vid_mut);
	int s_x = ((int)(x)) / 2;
	int s_y = ((int)(y)) / 2;
	if(percept.UnitPresent(s_x, s_y, cur_zpos, sel_id) > 0) {
	  sel_x = s_x;
	  sel_y = s_y;
	  sel_z = cur_zpos;
	  maction = 1;
	  raction = 1;
	  dpass->SetMenu(2, mactions[maction]);
	  dpass->SetMenu(3, ractions[raction]);
	  }
	else {
	  sel_id = -1;
	  sel_x = -1;
	  sel_y = -1;
	  sel_z = -1;
	  maction = 0;
	  raction = 0;
	  dpass->SetMenu(2, mactions[maction]);
	  dpass->SetMenu(3, ractions[raction]);
	  }
	}
      else if(event.user.code == SG_EVENT_MOTION) {
	double x = ((float*)(event.user.data2))[0];
	double y = ((float*)(event.user.data2))[1];
	SDL_mutexP(vid_mut);
	video->ScreenToMap(x, y, cur_zpos*CELL_HEIGHT);
	SDL_mutexV(vid_mut);
	mouse_x = ((int)(x)) / 2;
	mouse_y = ((int)(y)) / 2;
	if(sel_id > 0 && percept.UnitPresent(mouse_x, mouse_y, cur_zpos, targ_id) < 0) {
	  raction = 2;
	  dpass->SetMenu(3, ractions[raction]);
	  }
	else if(sel_id > 0) {
	  raction = 1;
	  targ_id = -1;
	  dpass->SetMenu(3, ractions[raction]);
	  }
	}
      else if(event.user.code == SG_EVENT_DND) {
	int *vars = (int*)(event.user.data2);
//	fprintf(stderr, "DnD %d from (%d,%d)[%d] to (%d,%d)[%d]\n",
//		vars[0], vars[3], vars[4], vars[6], vars[1], vars[2], vars[5]);
	if(vars[6] == 0) {	// Moved FROM ground
	  vector<SG_DNDBoxes*>::iterator dnd = dnds.begin();
	  gui->Lock();
	  for(; dnd != dnds.end(); ++dnd) {
	    if((*dnd) != (SG_DNDBoxes *)(event.user.data1)) {
	      (*dnd)->RemoveItem(vars[3], vars[4]);
	      }
	    }
	  gui->Unlock();
	  }
	if(vars[5] == 0) {	// Moved TO ground
	  vector<SG_DNDBoxes*>::iterator dnd = dnds.begin();
	  gui->Lock();
	  for(; dnd != dnds.end(); ++dnd) {
	    if((*dnd) != (SG_DNDBoxes *)(event.user.data1)) {
	      if(vars[0] == 1)
	        (*dnd)->AddItem(gun_icon, vars[1], vars[2], 4, 6, 1, 1);
	      else if(vars[0] == 2)
	        (*dnd)->AddItem(gren_icon, vars[1], vars[2], 2, 2, 2, 0);
	      }
	    }
	  gui->Unlock();
	  }
	}
//      else if(event.user.code == SG_EVENT_DNDDENIED) {
//	fprintf(stderr, "DEBUG: Denied that DnD request!\n");
//	}
      }
    }
  return exiting;
  }

static char buf[256];

bool Player_Local::Run() {
  Player::Run();	// Start with the basics

  if(cur_music < 0) cur_music = audio->Loop(music);

  UpdateEquipIDs();	// See if we need to do the Equip thing

  if(phase != PHASE_NONE) gui->MasterWidget()->AddWidget(wind[phase]);

//  //Temporary!
//  if(phase == PHASE_DECLARE) video->SetSubscreen(-0.8, -0.2, 0.4, 1.0);
//  else video->ResetSubscreen();

  video->SetOrtho();
//  video->SetPerspective(45.0);	//Just for testing
  video->SetSubscreen(-1.0, -1.0, 1.0, 1.0);
  video->SetZExtents(0.0, 12.0);	//FIXME: Get real extents
  video->SetPosition(64.0, 64.0, 0);	//FIXME: Really find start pos
  video->SetAngle(cur_ang, 0);
  video->SetZoom(cur_zoom, 0);
  video->SetDown(cur_down, 0);

  exiting = 0;
  SDL_Thread *th = SDL_CreateThread(event_thread_func, (void*)(this));

  while(exiting == 0) {
    if(pround != game->CurrentRound()) {
      pround = game->CurrentRound();
      game->UpdatePercept(id, pround);

      gui->Lock();

      ddoneb->TurnOff(); // Make sure "Ready" isn't checked next time

      UpdateEquipIDs();	 // See if we need to do the Equip thing again

      sprintf(buf, "Declare Turn (#%d)%c", pround, 0);
      dtext->SetText(buf);
      sprintf(buf, "Playback Turn (#%d)%c", pround-1, 0);
      rtext->SetText(buf);
      disround = pround-1;

      gui->Unlock();
      }

    if(phase != nextphase) {
      gui->Lock();
      if(nextphase == PHASE_REPLAY) UpdateEquipIDs();
      gui->MasterWidget()->RemoveWidget(wind[phase]);
      phase = nextphase;
      gui->MasterWidget()->AddWidget(wind[phase]);
      gui->Unlock();

//      //Temporary!
//      if(phase == PHASE_DECLARE) video->SetSubscreen(-0.8, -0.2, 0.4, 1.0);
//      else video->ResetSubscreen();
      }
    Uint32 cur_time = SDL_GetTicks();

    if(phase == PHASE_REPLAY) {
      SDL_mutexP(off_mut);
      CalcOffset(cur_time);
      sprintf(buf, "%d.%.3d seconds%c", offset / 1000, offset % 1000, 0);
      SDL_mutexV(off_mut);

      unsigned int showround = offset / 3000 + 1;
      if(showround > pround - 1)
	showround = pround - 1;
      if(disround != showround) {
	disround = showround;
	sprintf(buf, "Playback Turn (#%d)%c", disround, 0);
	rtext->SetText(buf);
	}
      rstamp->SetText(buf);
      }

    SDL_mutexP(vid_mut);
    video->StartScene();
    SDL_mutexV(vid_mut);

    gui->RenderStart(cur_time, true);

    if(phase == PHASE_DECLARE) {
      world->Render((pround-1)*3000);

      int unit;
      int unitthere = percept.UnitPresent(mouse_x, mouse_y, cur_zpos, unit);
      if(unitthere > 0) {
	world->DrawSelBox(mouse_x, mouse_y, cur_zpos, 0.0, 1.0, 0.0);
	}
      else if(unitthere < 0) {
	world->DrawSelBox(mouse_x, mouse_y, cur_zpos, 1.0, 0.0, 0.0);
	}
      else {
	world->DrawSelBox(mouse_x, mouse_y, cur_zpos, 1.0, 1.0, 0.0);
	}

      world->DrawSelBox(sel_x, sel_y, sel_z);

      scene->Render((pround-1)*3000);
      }
    else if(phase == PHASE_REPLAY) {
      world->Render(offset);
      scene->Render(offset);
      }

    gui->RenderFinish(cur_time, true);

    SDL_mutexP(vid_mut);
    video->FinishScene();
    SDL_mutexV(vid_mut);

    if(game->ShouldTerm()) {
      SDL_Event event;
      event.type = SDL_KEYDOWN;
      event.key.keysym.sym = SDLK_ESCAPE;
      SDL_PushEvent(&event);
      }
    }

  SDL_WaitThread(th, NULL);

  gui->MasterWidget()->RemoveWidget(wind[phase]);

  if(cur_music >= 0) audio->Stop(cur_music);
  cur_music = -1;

  game->TermThreads();	// Tell everyone else to exit too

  return exiting;
  }

	//Note: gui_mut must be locked before calling this function!
void Player_Local::UpdateEquipIDs() {
  Uint32 start_time = (pround-1)*3000;
  set<int> eqtmp;	//Temporary set of ids for eq
  eqid.clear();

  map<int, vector<UnitAct> >::iterator unit;
  unit = percept.my_units.begin();
  for(; unit != percept.my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->act == ACT_EQUIP && act->time >= start_time
		&& act->time < start_time+3000) {
	//eqtmp.insert(act->id);	//Temporarily Disable Equip Screen
	}
      }
    }

  vector<UnitOrder>::iterator order = orders.orders.begin();
  for(; order != orders.orders.end(); ++order) {
    if(order->order == ORDER_EQUIP) eqtmp.erase(order->id);
    }

  int targ = -1;	//Each troop/group equips SEPARATELY!
  unit = percept.my_units.begin();
  for(; unit != percept.my_units.end(); ++unit) {
    vector<UnitAct>::iterator act = unit->second.begin();
    for(; act != unit->second.end(); ++act) {
      if(act->act == ACT_EQUIP && eqtmp.count(act->id)
		&& (targ == -1 || targ == act->targ1)
		&& act->time >= start_time && act->time < start_time+3000) {
	//targ = act->targ1;		//Temporarily Disable Equip Screen
	//eqid.push_back(act->id);
	}
      }
    }

  vector<string> troops;
  vector<SG_Alignment *> adnds;
  vector<int>::iterator id = eqid.begin();
  for(; id != eqid.end(); ++id) {
    troops.push_back(game->UnitRef(*id)->name);
    if(ednd != NULL) {
      wind[PHASE_EQUIP]->RemoveWidget(ednd);
      delete ednd;
      ednd = NULL;
      dnds.clear();
      }

    SG_DNDBoxes *dnd = new SG_DNDBoxes(36, 28);
    dnd->Include( 0,  5,  4,  6, 4, 6, 1, 1);	//Left Hand
    dnd->Include(16,  5,  4,  6, 4, 6, 2, 0);	//Right Hand
    dnd->Include( 2,  1,  4,  2, 2, 2, 3, 0);	//Left Shoulder
    dnd->Include(14,  1,  4,  2, 2, 2, 4, 0);	//Right Shoulder
    dnd->Include( 0, 13,  4,  2, 2, 2, 5, 0);	//Left Leg
    dnd->Include(16, 13,  4,  2, 2, 2, 6, 0);	//Right Leg
    dnd->Include(25,  2,  6,  6, 2, 2, 7, 0);	//Backpack
    dnd->Include(24, 10,  8,  2, 2, 2, 8, 0);	//Main Belt
    dnd->Include(24, 12,  2,  2, 2, 2, 8, 0);	//Left Belt
    dnd->Include(30, 12,  2,  2, 2, 2, 8, 0);	//Right Belt
    dnd->Include( 0, 16, 36,  12, 2, 2, 0, 0);	//Ground

      // Hardcoded loadout for now - Temporary!
    if(troops.size() != 2) dnd->AddItem(gun_icon, 16, 5, 4, 6, 1, 1);
    if(troops.size() != 1) dnd->AddItem(gren_icon, 24, 12, 2, 2, 2, 0);

    dnd->SetBackground(new SG_Panel(equip_bg));

    adnds.push_back(dnd);
    dnds.push_back(dnd);
    }

  if(troops.size() > 0) {
    nextphase = PHASE_EQUIP;
    if(ednd == NULL) {
      ednd = new SG_MultiTab(troops, adnds, 24);
      wind[PHASE_EQUIP]->AddWidget(ednd, 0, 0, 12, 24);
      estats->SetText(troops[0]);
      }
    }
  else {
    if(ednd != NULL) {
      wind[PHASE_EQUIP]->RemoveWidget(ednd);
      delete ednd;
      ednd = NULL;
      dnds.clear();
      }
    if(pround == 1) nextphase = PHASE_DECLARE;
    else {
      SDL_mutexP(off_mut);
      nextphase = PHASE_REPLAY;
      last_time = SDL_GetTicks();
      last_offset = (pround - 2) * 3000;
      playback_speed = 5; //Default is play
      gui->Lock();
      rcontrols->Set(playback_speed);
      gui->Unlock();
      SDL_mutexV(off_mut);
      }
    }
  }

	//Note: off_mut must be locked before calling this function!
void Player_Local::CalcOffset(Uint32 cur_time) {
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
    if(past) {
      offset = 0;
      }
    else {
      offset = (pround - 2) * 3000;
      }
    playback_speed = 3; //Auto-stop
    gui->Lock();
    rcontrols->Set(playback_speed);
    gui->Unlock();
    }
  else if((!past) && offset < (pround - 2) * 3000) {
    offset = (pround - 2) * 3000;
    playback_speed = 3; //Auto-stop
    gui->Lock();
    rcontrols->Set(playback_speed);
    gui->Unlock();
    }
  else if(offset > (pround - 1) * 3000) {
    offset = (pround - 1) * 3000;
    playback_speed = 3; //Auto-stop
    gui->Lock();
    rcontrols->Set(playback_speed);
    gui->Unlock();
    }
  }
