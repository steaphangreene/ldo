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

int music;				//Background music
					//Temporary, just for testing

Game *cur_map = NULL;			//Temporary, just for testing

#define TGA_COLFIELDS SG_COL_U32B3, SG_COL_U32B2, SG_COL_U32B1, SG_COL_U32B4

static int drkred = 0;	//Global colordef


class Screen {
public:
  Screen() { main = NULL; };
  virtual ~Screen() {};
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Table *main;
  };

class Screen_LoadMap : public Screen {
public:
  Screen_LoadMap();
  virtual ~Screen_LoadMap();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
  };

class Screen_Title : public Screen {
public:
  Screen_Title();
  virtual ~Screen_Title();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *optb, *multb, *singb, *replb, *quitb;
  };

class Screen_Config : public Screen {
public:
  Screen_Config();
  virtual ~Screen_Config();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *backb;
  };

class Screen_Single : public Screen {
public:
  Screen_Single();
  virtual ~Screen_Single();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob;
  };

class Screen_Multi : public Screen {
public:
  Screen_Multi();
  virtual ~Screen_Multi();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob, *connb;
  SG_StickyButton *readyb;
  };

class Screen_Replay : public Screen {
public:
  Screen_Replay();
  virtual ~Screen_Replay();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob;
  };

class Screen_Equip : public Screen {
public:
  Screen_Equip();
  virtual ~Screen_Equip();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_TextArea *name;
  SG_Button *cancelb, *doneb;
  };

class Screen_Play : public Screen {
public:
  Screen_Play();
  virtual ~Screen_Play();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
  };

class Screen_Results : public Screen {
public:
  Screen_Results();
  virtual ~Screen_Results();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *replb, *saveb, *doneb, *quitb;
  };


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

  drkred = gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);

  //Setup POPUP_LOADMAP
  tab = new SG_FileBrowser("*.map");
  swidget[POPUP_LOADMAP] = tab;

  sscr[SCREEN_TITLE] = new Screen_Title;

  sscr[SCREEN_CONFIG] = new Screen_Config;

  sscr[SCREEN_SINGLE] = new Screen_Single;

  sscr[SCREEN_MULTI] = new Screen_Multi;

  sscr[SCREEN_REPLAY] = new Screen_Replay;

  sscr[SCREEN_EQUIP] = new Screen_Equip;

  //Setup SCREEN_PLAY (Temporary - will be handled by Game, not Screens)
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

  sscr[SCREEN_RESULTS] = new Screen_Results;
  }

Screens::~Screens() {
  delete gui;
  }

void Screens::Set(ScreenNum s) {
  if(sscr.count(screen)) {
    sscr[screen]->Finish(gui);
    }
  else if(screen != SCREEN_NONE) {
    gui->MasterWidget()->RemoveWidget(swidget[screen]);
    }
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

  if(sscr.count(screen)) {
    sscr[screen]->Start(gui);
    }
  else if(screen != SCREEN_NONE) {
    gui->MasterWidget()->AddWidget(swidget[screen]);
    }
  }

int Screens::Handle() {
  Set(SCREEN_TITLE);

  SDL_Event event;

  while(screen != SCREEN_NONE && SDL_WaitEvent(&event)) {
    do { // while(screen != SCREEN_NONE && SDL_PollEvent(&event));
      if(!gui->ProcessEvent(&event)) continue;

      //These events are for ALL screens!
      if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == SDLK_ESCAPE) {
          if(screen != SCREEN_TITLE) Set(SCREEN_TITLE);
	  else Set(SCREEN_NONE);
          }
	continue;
        }
      else if(event.type == SDL_QUIT) {
	Set(SCREEN_NONE);
	continue;
        }
      if(event.type == SDL_SG_EVENT) {	//Handle all sound effect here.
	if(event.user.code == SG_EVENT_BUTTONPRESS) {
	  audio_play(click, 8, 8);
	  }
	else if(event.user.code == SG_EVENT_SELECT) {
	  audio_play(click, 8, 8);
	  }
	else if(event.user.code == SG_EVENT_STICKYON) {
	  audio_play(click, 8, 8);
	  }
	else if(event.user.code == SG_EVENT_STICKYOFF) {
	  audio_play(click, 8, 8);
	  }
        else if(event.user.code == SG_EVENT_FILEOPEN) {
          audio_play(click, 8, 8);
	  }
					//Handle all popups here (Temporary)
        if(event.user.code == SG_EVENT_FILEOPEN) {
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
	      if(gomap.count(screen)) gomap[screen]->Enable();
	      }
	    }
          }
	}

      //This calls the individual screen(s)
      if(sscr.count(screen)) {
	ScreenNum next = sscr[screen]->Handle(gui, event);
	if(next != SCREEN_SAME) Set(next);
	continue;
	}

      //This is all obsolete - being replaced now
      if(event.type == SDL_SG_EVENT) {
        if(event.user.code == SG_EVENT_BUTTONCLICK) {
	  if(smap.count((SG_Widget*)event.user.data1)) {
	    Set(smap[(SG_Widget*)event.user.data1]);
	    }
          }
        else if(event.user.code == SG_EVENT_STICKYON) {
	  if(((SG_TextArea *)(event.user.data1))->Text() == "Ready to Play"
		&& cur_map != NULL) {
	    gomap[screen]->Enable();
	    }
          }
        else if(event.user.code == SG_EVENT_STICKYOFF) {
	  if(((SG_TextArea *)(event.user.data1))->Text() == "Ready to Play") {
	    gomap[screen]->Disable();
	    }
          }
        }
      } while(screen != SCREEN_NONE && SDL_PollEvent(&event));
    start_scene();
    gui->RenderStart(SDL_GetTicks());
    gui->RenderFinish(SDL_GetTicks());
    finish_scene();
    }

  return 0;
  }


void Screen::Start(SimpleGUI *gui) {
  gui->MasterWidget()->AddWidget(main);
  }

void Screen::Finish(SimpleGUI *gui) {
  gui->MasterWidget()->RemoveWidget(main);
  }

ScreenNum Screen::Handle(SimpleGUI *gui, SDL_Event &event) {
  return SCREEN_SAME;
  }

Screen_Config::Screen_Config() {
  //Setup SCREEN_CONFIG
  SG_Alignment *align;	// For temporary storage;

  main = new SG_Table(6, 7, 0.0625, 0.125);
//  swidget[SCREEN_CONFIG] = main;

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

  main->AddWidget(new SG_MultiTab(cfg_tab, cfg_scr, 12,
	but_normal, but_disabled, but_pressed, but_activated),
	0, 0, 5, 7);

  backb = new SG_Button("Back", but_normal, but_disabled, but_pressed);
  main->AddWidget(backb, 5, 0);
//  smap[wid] = SCREEN_BACK;
  }

Screen_Config::~Screen_Config() {
  //FIXME: Fill!
  }

void Screen_Config::Start(SimpleGUI *gui) {
  Screen::Start(gui);
  }

ScreenNum Screen_Config::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)backb) return SCREEN_BACK;
      }
    }
  return SCREEN_SAME;
  }

Screen_Equip::Screen_Equip() {
  }

Screen_Equip::~Screen_Equip() {
  //FIXME: Fill!
  }

void Screen_Equip::Start(SimpleGUI *gui) {
  if(main == NULL) {
    if(!cur_map) {
      fprintf(stderr, "ERROR: SCREEN_EQUIP requires loaded map!\n");
      exit(1);
      }

    vector<string> troops;
    for(int n = 0; cur_map->PlayerUnit(0, 0, n) != NULL; ++n) {
      troops.push_back(cur_map->PlayerUnit(0, 0, n)->name);
      }

    if(troops.size() < 1) {
      fprintf(stderr, "ERROR: SCREEN_EQUIP requires loaded non-empty map!\n");
      exit(1);
      }

    SG_Widget *wid;
    main = new SG_Table(16, 9, 0.0, 0.0);
//    wid = new SG_TextArea("Equip Your Team", drkred);
//    main->AddWidget(wid, 12, 3, 4, 1);
    cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
    main->AddWidget(cancelb, 12, 0, 2, 1);
//    smap[wid] = SCREEN_TITLE;
    doneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
    main->AddWidget(doneb, 14, 0, 2, 1);
//    smap[wid] = SCREEN_PLAY;

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
    main->AddWidget(wid, 0, 0, 12, 9);

    name = new SG_TextArea(troops[0], drkred);
    main->AddWidget(name, 12, 1, 4, 1);
    }

  Screen::Start(gui);
  }

ScreenNum Screen_Equip::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_SELECT) {	//Sound already played!
      const Unit *u = cur_map->PlayerUnit(0, 0, *((int*)(event.user.data2)));
      if(u) name->SetText(u->name);
      }
    else if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)doneb) return SCREEN_PLAY;
      }
    }
  return SCREEN_SAME;
  }


Screen_Title::Screen_Title() {
  main = new SG_Table(3, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("LDO", drkred), 0, 0, 2, 4);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 2, 1);
//  smap[optb] = SCREEN_CONFIG;
  multb = new SG_Button("Multiplayer", but_normal, but_disabled, but_pressed);
  main->AddWidget(multb, 2, 3);
//  smap[multb] = SCREEN_MULTI;
  singb = new SG_Button("Single Player", but_normal, but_disabled, but_pressed);
  main->AddWidget(singb, 2, 4);
//  smap[singb] = SCREEN_SINGLE;
  replb = new SG_Button("View Replay", but_normal, but_disabled, but_pressed);
  main->AddWidget(replb, 2, 5);
//  smap[replb] = SCREEN_REPLAY;
  quitb = new SG_Button("Quit Game", but_normal, but_disabled, but_pressed);
  main->AddWidget(quitb, 2, 6);
  }

Screen_Title::~Screen_Title() {
  //FIXME: Fill!
  }

ScreenNum Screen_Title::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)multb) return SCREEN_MULTI;
      else if(event.user.data1 == (void*)singb) return SCREEN_SINGLE;
      else if(event.user.data1 == (void*)replb) return SCREEN_REPLAY;
      else if(event.user.data1 == (void*)quitb) return SCREEN_NONE;
      }
    }
  return SCREEN_SAME;
  }


Screen_Single::Screen_Single() {
  main = new SG_Table(3, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Define Teams", drkred), 0, 0, 2, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 2, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 2, 1);
  loadb = new SG_Button("Load Scenario", but_normal, but_disabled, but_pressed);
  main->AddWidget(loadb, 2, 2);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  main->AddWidget(gob, 2, 6);
  gob->Disable();
  }

Screen_Single::~Screen_Single() {
  //FIXME: Fill!
  }

ScreenNum Screen_Single::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)loadb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) return SCREEN_EQUIP;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_map) gob->Enable();
      }
    }
  return SCREEN_SAME;
  }

Screen_Multi::Screen_Multi() {
  main = new SG_Table(3, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Gather Players", drkred), 0, 0, 2, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 2, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 2, 1);
  loadb = new SG_Button("Load Scenario", but_normal, but_disabled, but_pressed);
  main->AddWidget(loadb, 2, 2);
  connb = new SG_Button("Connect to Game", but_normal, but_disabled, but_pressed);
  main->AddWidget(connb, 2, 3);
  readyb = new SG_StickyButton("Ready to Play", but_normal, but_disabled, but_pressed, but_activated);
  main->AddWidget(readyb, 2, 5);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  main->AddWidget(gob, 2, 6);
  gob->Disable();
  }

Screen_Multi::~Screen_Multi() {
  //FIXME: Fill!
  }

ScreenNum Screen_Multi::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)loadb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) return SCREEN_EQUIP;
      }
    else if(event.user.code == SG_EVENT_STICKYON) {
      if(cur_map) gob->Enable();
      }
    else if(event.user.code == SG_EVENT_STICKYOFF) {
      gob->Disable();
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_map && readyb->IsOn()) gob->Enable();
      }
    }
  return SCREEN_SAME;
  }

Screen_Replay::Screen_Replay() {
  main = new SG_Table(3, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Load Replay", drkred), 0, 0, 2, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 2, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 2, 1);
  loadb = new SG_Button("Load Replay", but_normal, but_disabled, but_pressed);
  main->AddWidget(loadb, 2, 2);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  main->AddWidget(gob, 2, 6);
  gob->Disable();
  }

Screen_Replay::~Screen_Replay() {
  //FIXME: Fill!
  }

ScreenNum Screen_Replay::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)loadb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) return SCREEN_EQUIP;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_map) gob->Enable();
      }
    }
  return SCREEN_SAME;
  }


Screen_Results::Screen_Results() {
  main = new SG_Table(6, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Game Results", drkred), 0, 0, 2, 2);
  replb = new SG_Button("Replay", but_normal, but_disabled, but_pressed);
  main->AddWidget(replb, 5, 0);
  saveb = new SG_Button("Save", but_normal, but_disabled, but_pressed);
  main->AddWidget(saveb, 5, 1);
  doneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
  main->AddWidget(doneb, 5, 2);
  quitb = new SG_Button("Quit", but_normal, but_disabled, but_pressed);
  main->AddWidget(quitb, 5, 6);
  }

Screen_Results::~Screen_Results() {
  //FIXME: Fill!
  }

void Screen_Results::Start(SimpleGUI *gui) {
  Screen::Start(gui);
  }

ScreenNum Screen_Results::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)replb) return SCREEN_PLAY;
      else if(event.user.data1 == (void*)doneb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)quitb) return SCREEN_NONE;
      else if(event.user.data1 == (void*)saveb) {}; //FIXME: Implement!
      }
    }
  return SCREEN_SAME;
  }
