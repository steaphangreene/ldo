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

Screens::Screens() {
  screen = SCREEN_NONE;
  swidget.resize(SCREEN_MAX, NULL);

  init_renderer(640, 360);
  audio_init(256);
  click = audio_buildsound(click_data, sizeof(click_data));

  gui = new SimpleGUI(ASPECT_FIXED_Y|ASPECT_FIXED_X, 16.0/9.0);
  gui->LoadFont("fonts/Denmark Regular.ttf", 100);

  SG_Table *tab;	// For temporary storage;
  SG_Widget *wid;	// For temporary storage;


  //Setup SCREEN_TITLE
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_TITLE] = tab;
  tab->AddWidget(new SG_TextArea("LDO",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 2, 4);
  wid = new SG_Button("Multiplayer");
  tab->AddWidget(wid, 2, 3);
  smap[wid] = SCREEN_MULTI;
  wid = new SG_Button("Single Player");
  tab->AddWidget(wid, 2, 4);
  smap[wid] = SCREEN_SINGLE;
  wid = new SG_Button("View Replay");
  tab->AddWidget(wid, 2, 5);
  smap[wid] = SCREEN_REPLAY;
  wid = new SG_Button("Quit Game");
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_NONE;


  //Setup SCREEN_SINGLE
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_SINGLE] = tab;
  tab->AddWidget(new SG_TextArea("Define Teams",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 2, 2);
  wid = new SG_Button("Cancel");
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Load Scenario");
  tab->AddWidget(wid, 2, 2);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Go");
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_EQUIP;


  //Setup SCREEN_MULTI
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_MULTI] = tab;
  tab->AddWidget(new SG_TextArea("Gather Players",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 2, 2);
  wid = new SG_Button("Cancel");
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Load Scenario");
  tab->AddWidget(wid, 2, 2);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Connect to Game");
  tab->AddWidget(wid, 2, 3);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_StickyButton("Ready to Play");
  tab->AddWidget(wid, 2, 5);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Go");
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_EQUIP;


  //Setup SCREEN_REPLAY
  tab = new SG_Table(3, 7, 0.0625, 0.125);
  swidget[SCREEN_REPLAY] = tab;
  tab->AddWidget(new SG_TextArea("Load Replay",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 2, 2);
  wid = new SG_Button("Cancel");
  tab->AddWidget(wid, 2, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Load Replay");
  tab->AddWidget(wid, 2, 2);
//  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Go");
  tab->AddWidget(wid, 2, 6);
  smap[wid] = SCREEN_PLAY;


  //Setup SCREEN_EQUIP
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_EQUIP] = tab;
  tab->AddWidget(new SG_TextArea("Equip Your Team",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 4, 2);
  wid = new SG_Button("Cancel");
  tab->AddWidget(wid, 4, 0);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Done");
  tab->AddWidget(wid, 5, 0);
  smap[wid] = SCREEN_PLAY;


  //Setup SCREEN_PLAY
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_PLAY] = tab;
  tab->AddWidget(new SG_TextArea("Playing/Replaying LDO....",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 4, 2);
  wid = new SG_Button("Done");
  tab->AddWidget(wid, 5, 6);
  smap[wid] = SCREEN_RESULTS;


  //Setup SCREEN_RESULTS
  tab = new SG_Table(6, 7, 0.0625, 0.125);
  swidget[SCREEN_RESULTS] = tab;
  tab->AddWidget(new SG_TextArea("Game Results",
	gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0)),
	0, 0, 2, 2);
  wid = new SG_Button("Replay");
  tab->AddWidget(wid, 5, 0);
  smap[wid] = SCREEN_PLAY;
  wid = new SG_Button("Save");
  tab->AddWidget(wid, 5, 1);
//  smap[wid] = SCREEN_RESULTS;
  wid = new SG_Button("Done");
  tab->AddWidget(wid, 5, 2);
  smap[wid] = SCREEN_TITLE;
  wid = new SG_Button("Quit");
  tab->AddWidget(wid, 5, 6);
  smap[wid] = SCREEN_NONE;
  }

Screens::~Screens() {
  delete gui;
  }

void Screens::Set(ScreenNum s) {
  if(screen != SCREEN_NONE) gui->MasterWidget()->RemoveWidget(swidget[screen]);
  screen = s;
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
          }
        else if(event.user.code == SG_EVENT_STICKYOFF) {
          audio_play(click, 8, 8);
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
