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

#include "screens.h"
#include "renderer.h"
#include "audio.h"
#include "click.h"

#include "game.h"

SDL_Surface *but_normal, *but_pressed, *but_disabled, *but_activated;
SDL_Surface *equip_bg;

#include "cursor.h"

SDL_Surface *mouse_cursor;

#include "m41.h"
#include "mark2.h"

SDL_Surface *gun_icon, *gren_icon;

map<ScreenNum, SG_Widget *> gomap;	//Map of go buttons per screen
					//Temporary, just for testing

map<ScreenNum, SG_Widget *> readymap;	//Map of ready buttons per screen
					//Temporary, just for testing

map<ScreenNum, SG_Widget *> saymap;	//Map of go buttons per screen
					//Temporary, just for testing

int music;				//Background music
					//Temporary, just for testing

Game *cur_map = NULL;			//Temporary, just for testing

#define TGA_COLFIELDS SG_COL_U32B3, SG_COL_U32B2, SG_COL_U32B1, SG_COL_U32B4

static int drkred = 0;	//Global colordef


Screens::Screens() {
  screen = SCREEN_NONE;
  last_screen = SCREEN_NONE;

  init_renderer(640, 360);
  audio_init(4096);
  click = audio_buildsound(click_data, sizeof(click_data));
  music = audio_loadmusic("music/cantus.wav");
  audio_loop(music, 8, 0);

  gui = new SimpleGUI(ASPECT_FIXED_Y|ASPECT_FIXED_X, 16.0/9.0);
  gui->LoadFont("fonts/Denmark Regular.ttf", 100);

  mouse_cursor = SDL_CreateRGBSurfaceFrom(cursor, 256, 256, 32, 256*4, TGA_COLFIELDS);
  gui->SetMouseCursor(mouse_cursor, 0.125, 0.125);

  but_normal = SDL_LoadBMP("buttontex_normal.bmp");
  but_pressed = SDL_LoadBMP("buttontex_pressed.bmp");
  but_disabled = SDL_LoadBMP("buttontex_disabled.bmp");
  but_activated = SDL_LoadBMP("buttontex_activated.bmp");
  equip_bg = SDL_LoadBMP("equip_bg.bmp");
  gun_icon = SDL_CreateRGBSurfaceFrom(m41, 170, 256, 32, 170*4, TGA_COLFIELDS);
  gren_icon = SDL_CreateRGBSurfaceFrom(mark2, 256, 256, 32, 256*4, TGA_COLFIELDS);

  SG_Table *tab;	// For temporary storage;
  SG_Widget *wid;	// For temporary storage;
  SG_Alignment *align;	// For temporary storage;

  drkred = gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);

  //Setup POPUP_LOADMAP
  tab = new SG_FileBrowser("*.map");
  swidget[POPUP_LOADMAP] = tab;

  //Setup SCREEN_TITLE
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_TITLE] = tab;
  tab->AddWidget(new SG_TextArea("LDO", drkred), 0, 0, 2, 4);
  wid = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 1);
  smap[wid] = SCREEN_CONFIG;
  wid = new SG_Button("Multiplayer", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 3);
  smap[wid] = SCREEN_MULTI;
  wid = new SG_Button("Single Player", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 4);
  smap[wid] = SCREEN_SINGLE;
  wid = new SG_Button("View Replay", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 5);
  smap[wid] = SCREEN_REPLAY;
  wid = new SG_Button("Quit Game", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_NONE;


  //Setup SCREEN_CONFIG
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_CONFIG] = tab;

  vector<string> cfg_tab;
  vector<SG_Alignment *> cfg_scr;

  cfg_tab.push_back("Video");
  align = new SG_Table(3, 7, 0.0625, 0.125);
  cfg_scr.push_back(align);

  cfg_tab.push_back("Audio");
  align = new SG_Table(3, 7, 0.0625, 0.125);
  cfg_scr.push_back(align);

  cfg_tab.push_back("Mouse");
  align = new SG_Table(3, 7, 0.0625, 0.125);
  cfg_scr.push_back(align);

  cfg_tab.push_back("Keyboard");
  align = new SG_Table(3, 7, 0.0625, 0.125);
  cfg_scr.push_back(align);

  cfg_tab.push_back("Game");
  align = new SG_Table(3, 7, 0.0625, 0.125);
  cfg_scr.push_back(align);

  tab->AddWidget(new SG_MultiTab(cfg_tab, cfg_scr, 12,
	but_normal, but_disabled, but_pressed, but_activated),
	0, 0, 5, 7);

  wid = new SG_Button("Back", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 0);
  smap[wid] = SCREEN_BACK;


  //Setup SCREEN_SINGLE
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_SINGLE] = tab;
  tab->AddWidget(new SG_TextArea("Define Teams", drkred), 0, 0, 2, 2);
  wid = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 1);
  smap[wid] = SCREEN_CONFIG;
  wid = new SG_Button("Load Scenario", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 2);
  smap[wid] = POPUP_LOADMAP;
  wid = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_EQUIP;
  gomap[SCREEN_SINGLE] = wid;
  wid->Disable();


  //Setup SCREEN_MULTI
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_MULTI] = tab;
  tab->AddWidget(new SG_TextArea("Gather Players", drkred), 0, 0, 2, 2);
  wid = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 1);
  smap[wid] = SCREEN_CONFIG;
  wid = new SG_Button("Load Scenario", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 2);
  smap[wid] = POPUP_LOADMAP;
  wid = new SG_Button("Connect to Game", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 3);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_StickyButton("Ready to Play", but_normal, but_disabled, but_pressed, but_activated);
  tab->AddWidget(wid, 2, 5);
//  smap[wid] = SCREEN_TITLE;
  readymap[SCREEN_MULTI] = wid;
  wid = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_EQUIP;
  gomap[SCREEN_MULTI] = wid;
  wid->Disable();


  //Setup SCREEN_REPLAY
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_REPLAY] = tab;
  tab->AddWidget(new SG_TextArea("Load Replay", drkred), 0, 0, 2, 2);
  wid = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 1);
  smap[wid] = SCREEN_CONFIG;
  wid = new SG_Button("Load Replay", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 2);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_PLAY;
  gomap[SCREEN_REPLAY] = wid;
  wid->Disable();


  //Setup of SCREEN_EQUIP is done after map load!
  swidget[SCREEN_EQUIP] = NULL;

  //Setup SCREEN_PLAY
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_PLAY] = tab;
  tab->AddWidget(new SG_TextArea("Playing/Replaying LDO....", drkred),
	0, 0, 4, 2);
  wid = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 0, 6);
  smap[wid] = SCREEN_CONFIG;
  wid = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 6);
  smap[wid] = SCREEN_RESULTS;


  //Setup SCREEN_RESULTS
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_RESULTS] = tab;
  tab->AddWidget(new SG_TextArea("Game Results", drkred), 0, 0, 2, 2);
  wid = new SG_Button("Replay", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 0);
  smap[wid] = SCREEN_PLAY;
  wid = new SG_Button("Save", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 1);
//  smap[wid] = SCREEN_RESULTS;
  wid = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 2);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Quit", but_normal, but_disabled, but_pressed);
  tab->AddWidget(wid, 5, 6);
  smap[wid] = SCREEN_NONE;
  }

Screens::~Screens() {
  delete gui;
  }

void Screens::Set(ScreenNum s) {
  if(screen != SCREEN_NONE) gui->MasterWidget()->RemoveWidget(swidget[screen]);
  if(gomap.count(s)) {
    if(cur_map && (readymap.count(s) == 0 || readymap[s]->IsOn())) {
      gomap[s]->Enable();
      }
    else gomap[s]->Disable();
    }
  if(screen != s && readymap.count(screen)) readymap[screen]->TurnOff();
  gui->UnsetPopupWidget();
  if(s == SCREEN_BACK) {
    screen = last_screen;
    }
  else if(s < SCREEN_BACK) {	//Popup!
    gui->SetPopupWidget(swidget[s]);
    }
  else {
    last_screen = screen;
    screen = s;
    }

  if(screen == SCREEN_EQUIP && swidget[screen] == NULL) {
    if(!cur_map) {
      fprintf(stderr, "ERROR: SCREEN_EQUIP requires loaded map!\n");
      exit(1);
      }

    vector<string> troops;  //Just for example
    for(int n = 0; cur_map->PlayerUnit(0, 0, n) != NULL; ++n) {
      troops.push_back(cur_map->PlayerUnit(0, 0, n)->name);
      }

    if(troops.size() < 1) {
      fprintf(stderr, "ERROR: SCREEN_EQUIP requires loaded non-empty map!\n");
      exit(1);
      }

    SG_Table *tab;
    SG_Widget *wid;
//    tab = new SG_Table(16, 9, 0.0625, 0.125);
    tab = new SG_Table(16, 9, 0.0, 0.0);
    swidget[SCREEN_EQUIP] = tab;
//    wid = new SG_TextArea("Equip Your Team", drkred);
//    tab->AddWidget(wid, 12, 3, 4, 1);
    wid = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
    tab->AddWidget(wid, 12, 0, 2, 1);
    smap[wid] = SCREEN_TITLE;
    wid = new SG_Button("Done", but_normal, but_disabled, but_pressed);
    tab->AddWidget(wid, 14, 0, 2, 1);
    smap[wid] = SCREEN_PLAY;

    vector<SG_Alignment *> dnds;
    for(int troop = 0; troop < (int)(troops.size()); ++troop) {
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

	// Hardcoded loadout for now
      if(troop != 2) dnd->AddItem(gun_icon, 8, 3, 2, 3);
      if(troop != 1) dnd->AddItem(gren_icon, 11, 7);

      wid = new SG_Panel(equip_bg);
      dnd->SetBackground(wid);

      dnds.push_back(dnd);
      }

    wid = new SG_MultiTab(troops, dnds, 9,
	but_normal, but_disabled, but_pressed, but_activated);
    tab->AddWidget(wid, 0, 0, 12, 9);

    wid = new SG_TextArea(troops[0], drkred);
    tab->AddWidget(wid, 12, 1, 4, 1);
    saymap[SCREEN_EQUIP] = wid;
    }

  if(screen != SCREEN_NONE) gui->MasterWidget()->AddWidget(swidget[screen]);
  }

int Screens::Handle() {
  Set(SCREEN_TITLE);

  SDL_Event event;

  while(screen != SCREEN_NONE && SDL_WaitEvent(&event)) {
    do { // while(screen != SCREEN_NONE && SDL_PollEvent(&event));
      if(!gui->ProcessEvent(&event)) continue;

      if(event.type == SDL_SG_EVENT) {
        if(event.user.code == SG_EVENT_BUTTONPRESS) {
          audio_play(click, 8, 8);
	  }
        else if(event.user.code == SG_EVENT_BUTTONCLICK) {
	  if(smap.count((SG_Widget*)event.user.data1)) {
	    Set(smap[(SG_Widget*)event.user.data1]);
	    }
          }
        else if(event.user.code == SG_EVENT_STICKYON) {
          audio_play(click, 8, 8);
	  if(((SG_TextArea *)(event.user.data1))->Text() == "Ready to Play"
		&& cur_map != NULL) {
	    gomap[screen]->Enable();
	    }
          }
        else if(event.user.code == SG_EVENT_STICKYOFF) {
          audio_play(click, 8, 8);
	  if(((SG_TextArea *)(event.user.data1))->Text() == "Ready to Play") {
	    gomap[screen]->Disable();
	    }
          }
        else if(event.user.code == SG_EVENT_SELECT) {
          audio_play(click, 8, 8);
	  if(screen == SCREEN_EQUIP) {
	    const Unit *u = cur_map->PlayerUnit(0, 0, *((int*)(event.user.data2)));
	    if(u) ((SG_TextArea*)(saymap[screen]))->SetText(u->name);
	    }
          }
        else if(event.user.code == SG_EVENT_FILEOPEN) {
          audio_play(click, 8, 8);
	  string filename = ((SG_FileBrowser*)(event.user.data1))->FileName();
	  if(!cur_map) cur_map = new Game;
	  if(!cur_map->Load(filename)) {
	    delete cur_map;
	    cur_map = NULL;
	    fprintf(stderr, "WARNING: Could not load map file '%s'\n",
		filename.c_str());
	    }
	  else {
	    cur_map->Save(filename); // For auto-upgrade of mapfile
	    gui->UnsetPopupWidget();
	    if(readymap.count(screen) == 0 || readymap[screen]->IsOn()) {
	      gomap[screen]->Enable();
	      }
	    }
          }
        }
      else if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == SDLK_ESCAPE) {
          if(screen != SCREEN_TITLE) Set(SCREEN_TITLE);
	  else Set(SCREEN_NONE);
          }
        }
      else if(event.type == SDL_QUIT) {
	Set(SCREEN_NONE);
        }
      } while(screen != SCREEN_NONE && SDL_PollEvent(&event));
    start_scene();
    gui->RenderStart(SDL_GetTicks());
    gui->RenderFinish(SDL_GetTicks());
    finish_scene();
    }

  return 0;
  }
