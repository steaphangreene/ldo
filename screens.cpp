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

#include "SDL_net.h"

#include "screens.h"
#include "audio.h"
#include "click.h"

#include "game.h"
#include "player.h"
#include "player_local.h"

SDL_Surface *but_normal, *but_pressed, *but_disabled, *but_activated;

#include "cursor.h"
SDL_Surface *mouse_cursor;
#define TGA_COLFIELDS SG_COL_U32B3, SG_COL_U32B2, SG_COL_U32B1, SG_COL_U32B4

Game *cur_game = NULL;			//Temporary, just for testing

static int drkred = 0;	//Global colordef

int music = 0;			//Background Music (Temporary)
Sound *cur_music = NULL;	//Currently Playing Music (Temporary)

class Screen {
public:
  Screen() { main = NULL; };
  virtual ~Screen() { if(main) delete main; };
  virtual ScreenNum Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Table *main;
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
  virtual ScreenNum Start(SimpleGUI *gui);
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
  SG_ScrollingArea *connscr;
  SimpleConnect *connector;
  };

class Screen_Multi : public Screen {
public:
  Screen_Multi();
  virtual ~Screen_Multi();
  virtual ScreenNum Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Button *cancelb, *optb, *hostb, *gob, *scanb;
  SG_StickyButton *readyb;
  SG_ScrollingArea *connscr;
  SimpleConnect *connector;
  bool net_init;
  };

class Screen_Replay : public Screen {
public:
  Screen_Replay();
  virtual ~Screen_Replay();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob;
  };

class Screen_Play : public Screen {
public:
  Screen_Play();
  virtual ~Screen_Play();
  virtual ScreenNum Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Button *optb, *doneb;
  };

class Screen_Results : public Screen {
public:
  Screen_Results();
  virtual ~Screen_Results();
  virtual ScreenNum Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_Button *replb, *saveb, *doneb, *quitb;
  };


class Popup : public Screen {
public:
  Popup() { main = NULL; };
  virtual ~Popup() {};
  virtual ScreenNum Start(SimpleGUI *gui);
  virtual void Finish(SimpleGUI *gui);
  };

class Popup_LoadMap : public Popup {
public:
  Popup_LoadMap();
  virtual ~Popup_LoadMap();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  };



Screens::Screens() {
  popup = SCREEN_NONE;
  screen = SCREEN_NONE;
  last_screen = SCREEN_NONE;

  renderer = new SimpleVideo(640, 360, 16.0/9.0);
  audio_init(4096);
  click = audio_buildsound(click_data, sizeof(click_data));
  music = audio_loadmusic("music/cantus.wav");
  cur_music = audio_loop(music, 16, 0);

  gui = new SimpleGUI(ASPECT_FIXED_Y|ASPECT_FIXED_X, 16.0/9.0);
  gui->LoadFont("fonts/Denmark Regular.ttf", 24);

  mouse_cursor = SDL_CreateRGBSurfaceFrom(cursor, 256, 256, 32, 256*4, TGA_COLFIELDS);
  gui->SetMouseCursor(mouse_cursor, 0.125, 0.125);

  but_normal = SDL_LoadBMP("buttontex_normal.bmp");
  but_pressed = SDL_LoadBMP("buttontex_pressed.bmp");
  but_disabled = SDL_LoadBMP("buttontex_disabled.bmp");
  but_activated = SDL_LoadBMP("buttontex_activated.bmp");

  drkred = gui->NewColor(0.0, 0.0, 0.0, 0.5, 0.0, 0.0);

  sscr[SCREEN_TITLE] = new Screen_Title;
  sscr[SCREEN_CONFIG] = new Screen_Config;
  sscr[SCREEN_SINGLE] = new Screen_Single;
  sscr[SCREEN_MULTI] = new Screen_Multi;
  sscr[SCREEN_REPLAY] = new Screen_Replay;
  sscr[SCREEN_PLAY] = new Screen_Play;
  sscr[SCREEN_RESULTS] = new Screen_Results;

  sscr[POPUP_LOADMAP] = new Popup_LoadMap;
  }

Screens::~Screens() {
  if(cur_game) delete cur_game;
  cur_game = NULL;

  audio_stop(cur_music);
  cur_music = NULL;

  map<ScreenNum, Screen *>::iterator itrs = sscr.begin();
  for(; itrs != sscr.end(); ++itrs) {
    delete itrs->second;
    }
  sscr.clear();

  delete gui;
  }

void Screens::Set(ScreenNum s) {
  gui->UnsetPopupWidget();	//Step 1: Remove any popups
  if(sscr.count(popup)) {
    sscr[popup]->Finish(gui);
    }
  popup = SCREEN_NONE;

  if(s == POPUP_CLEAR) return;	//Just needed to clear popup - done

  if(s < POPUP_MAX) {		//It's a Popup, not a Screen
    popup = s;
    if(sscr.count(popup)) {
      sscr[popup]->Start(gui);
      }
    return;			//Don't want to swap screens too.
    }

  if(sscr.count(screen)) {
    sscr[screen]->Finish(gui);
    }

  if(s == SCREEN_BACK) {
    screen = last_screen;
    }
  else {
    last_screen = screen;
    screen = s;
    }

  if(sscr.count(screen)) {
    ScreenNum res = sscr[screen]->Start(gui);
    if(res != SCREEN_SAME) Set(res);
    }
  }

int Screens::Handle() {
  Set(SCREEN_TITLE);

  SDL_Event event;

  while(screen != SCREEN_NONE) {
    while(gui->PollEvent(&event)) {
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
      if(event.type == SDL_SG_EVENT) {	//Handle all GUI sound effects here.
	switch(event.user.code) {
	  case(SG_EVENT_FILEOPEN):
	  case(SG_EVENT_STICKYOFF):
	  case(SG_EVENT_STICKYON):
	  case(SG_EVENT_SELECT):
	  case(SG_EVENT_BUTTONPRESS): {
	    audio_play(click, 8, 8);
	    }break;
	  default: {
	    }break;
	  }
	}

      //This calls the individual popup (if there is one)
      if(popup != SCREEN_NONE && sscr.count(popup)) {
	ScreenNum next = sscr[popup]->Handle(gui, event);
	if(next != SCREEN_SAME) Set(next);
	}

      //This calls the individual screen (if there is one - there should be!)
      if(screen != SCREEN_NONE && sscr.count(screen)) {
	ScreenNum next = sscr[screen]->Handle(gui, event);
	if(next != SCREEN_SAME) Set(next);
	}
      }

    renderer->StartScene();
    gui->RenderStart(SDL_GetTicks());
    gui->RenderFinish(SDL_GetTicks());
    renderer->FinishScene();
    }

  return 0;
  }


ScreenNum Screen::Start(SimpleGUI *gui) {
  gui->MasterWidget()->AddWidget(main);
  return SCREEN_SAME;
  }

void Screen::Finish(SimpleGUI *gui) {
  gui->MasterWidget()->RemoveWidget(main);
  }

ScreenNum Screen::Handle(SimpleGUI *gui, SDL_Event &event) {
  return SCREEN_SAME;
  }

ScreenNum Popup::Start(SimpleGUI *gui) {
  gui->SetPopupWidget(main);
  return SCREEN_SAME;
  }

void Popup::Finish(SimpleGUI *gui) {
  gui->UnsetPopupWidget();
  }

Screen_Config::Screen_Config() {
  //Setup SCREEN_CONFIG
  SG_Alignment *align;	// For temporary storage;

  main = new SG_Table(6, 7, 0.0625, 0.125);

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
  }

Screen_Config::~Screen_Config() {
  //FIXME: Fill!
  }

ScreenNum Screen_Config::Start(SimpleGUI *gui) {
  Screen::Start(gui);
  return SCREEN_SAME;
  }

ScreenNum Screen_Config::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)backb) return SCREEN_BACK;
      }
    }
  return SCREEN_SAME;
  }

Screen_Title::Screen_Title() {
  main = new SG_Table(4, 9, 0.0625, 0.125);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 3, 1);
  multb = new SG_Button("Multiplayer", but_normal, but_disabled, but_pressed);
  main->AddWidget(multb, 3, 3);
  singb = new SG_Button("Single Player", but_normal, but_disabled, but_pressed);
  main->AddWidget(singb, 3, 4);
  replb = new SG_Button("View Replay", but_normal, but_disabled, but_pressed);
  main->AddWidget(replb, 3, 6);
  quitb = new SG_Button("Quit Game", but_normal, but_disabled, but_pressed);
  main->AddWidget(quitb, 3, 8);

  SG_TextArea *title = new SG_TextArea("LDO", drkred);
  title->SetMargins(0.125, 0.0);
  title->SetFontSize(100);
  main->AddWidget(title, 0, 0, 3, 3);

  SG_AutoScroll *scr =
	new SG_AutoScroll(1.0, 15.0, 0.0, -15.0, 0.0, -15.0, 0.0, 60.0);
  main->AddWidget(scr, 0, 3, 3, 6);

  FILE *credfl = fopen("CREDITS", "r");
  if(credfl) {
    fseek(credfl, 0, SEEK_END);
    int size = ftell(credfl);
    fseek(credfl, 0, SEEK_SET);
    char *data = new char[size+1];
    fread(data, 1, size, credfl); 
    data[size] = 0;
    SG_TextArea *credits = new SG_TextArea(data, drkred);
    credits->SetMargins(0.125, 0.03125);
    credits->SetFontSize(20);
    credits->SetVisibleSize(30.0, 15.0);
    scr->AddWidget(credits);
    delete data;
    fclose(credfl);

    double vislines = credits->NumLines()+30.0;
    scr->SetYScroll(-15.0, vislines-15.0, vislines*2.0);
    }
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
  main = new SG_Table(6, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Define Teams", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 5, 1);
  loadb = new SG_Button("Load Scenario", but_normal, but_disabled, but_pressed);
  main->AddWidget(loadb, 5, 2);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  gob->SetAlignment(SG_ALIGN_LEFT);	//Temporary!
  main->AddWidget(gob, 5, 6);
  gob->Disable();
  connscr = new SG_ScrollingArea(8.0, 8.0);
  main->AddWidget(connscr, 0, 2, 5, 5);
  connector = new SimpleConnect();
  connscr->AddWidget(connector);
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
      else if(event.user.data1 == (void*)gob) return SCREEN_PLAY;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_game) gob->Enable();
      vector<SC_SlotType> slots;
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      connector->SetSlots(slots);
      connector->Config();
      }
    else if(event.user.code == SG_EVENT_OK) {
      }
    }
  return SCREEN_SAME;
  }

Screen_Multi::Screen_Multi() {
  net_init = false;
  main = new SG_Table(6, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Gather Players", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 5, 1);
  hostb = new SG_Button("Host", but_normal, but_disabled, but_pressed);
  main->AddWidget(hostb, 5, 2);
  scanb = new SG_Button("Search", but_normal, but_disabled, but_pressed);
  main->AddWidget(scanb, 5, 3);
  readyb = new SG_StickyButton("Ready", but_normal, but_disabled, but_pressed, but_activated);
  main->AddWidget(readyb, 5, 5);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  gob->SetAlignment(SG_ALIGN_CENTER);	//Temporary!
  main->AddWidget(gob, 5, 6);
  gob->Disable();
  connscr = new SG_ScrollingArea(8.0, 8.0);
  main->AddWidget(connscr, 0, 2, 5, 5);
  connector = new SimpleConnect();
  connscr->AddWidget(connector);
  }

Screen_Multi::~Screen_Multi() {
  if(connscr) {
    main->RemoveWidget(connscr);
    delete connscr;	// To delete connector before shutting down SDL_net!
    connector = NULL;
    }
  if(net_init) {
    SDLNet_Quit();
    net_init = false;
    }
  //FIXME: Cleanup Widgets!
  }

ScreenNum Screen_Multi::Start(SimpleGUI *gui) {
  Screen::Start(gui);
  if(!net_init) {
    if(SDLNet_Init() < 0) {
      fprintf(stderr, "ERROR: SDLNet_Init Failed: %s\n", SDL_GetError());
      exit(1);
      }
    net_init = true;
    }

  connector->SetTag("LDO");
  connector->Search();

  return SCREEN_SAME;
  }

void Screen_Multi::Finish(SimpleGUI *gui) {
  Screen::Finish(gui);
  connector->Reset();
  }

ScreenNum Screen_Multi::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)hostb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) {
	SimpleConnections conn = connector->ClaimConnections();
	char buf[16] = {0};
	if(conn.sock) { // Client
	  SDLNet_TCP_Recv(conn.sock, buf, 16);
	  fprintf(stderr, "Recived '%s' from server\n", buf);
	  }
	else {		// Server
	  sprintf(buf, "Hello%c", 0);
	  vector<SlotData>::iterator slot = conn.slots.begin();
	  for(; slot != conn.slots.end(); ++slot) {
	    if(slot->sock) SDLNet_TCP_Send(slot->sock, buf, 16);
	    }
	  fprintf(stderr, "Sent data to clients\n");
	  }
	return SCREEN_PLAY;
	}
      else if(event.user.data1 == (void*)scanb) {
	connector->Search();
	}
      }
    else if(event.user.code == SG_EVENT_STICKYON) {
      if(cur_game) gob->Enable();
      }
    else if(event.user.code == SG_EVENT_STICKYOFF) {
      gob->Disable();
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_game && readyb->IsOn()) {
	gob->Enable();
	}
      connector->SetMapName(cur_game->MapName());
      vector<SC_SlotType> slots;
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      slots.push_back(SC_SLOT_PLAYER);
      connector->SetSlots(slots);
      connector->Host();
      }
    else if(event.user.code == SG_EVENT_OK) {
      }
    }
  return SCREEN_SAME;
  }

Screen_Replay::Screen_Replay() {
  main = new SG_Table(6, 7, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Load Replay", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel", but_normal, but_disabled, but_pressed);
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
  main->AddWidget(optb, 5, 1);
  loadb = new SG_Button("Load Replay", but_normal, but_disabled, but_pressed);
  main->AddWidget(loadb, 5, 2);
  gob = new SG_Button("Go", but_normal, but_disabled, but_pressed);
  gob->SetAlignment(SG_ALIGN_RIGHT);	//Temporary!
  main->AddWidget(gob, 5, 6);
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
      else if(event.user.data1 == (void*)gob) return SCREEN_PLAY;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_game) gob->Enable();
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

ScreenNum Screen_Results::Start(SimpleGUI *gui) {
  Screen::Start(gui);
  return SCREEN_SAME;
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

Screen_Play::Screen_Play() {
//  //Setup SCREEN_PLAY (Temporary - will be handled by Game, not Screens)
//  main = new SG_Table(6, 7, 0.0625, 0.125);
//  main->AddWidget(new SG_TextArea("Playing LDO", drkred),
//	0, 0, 4, 2);
//  optb = new SG_Button("Options", but_normal, but_disabled, but_pressed);
//  main->AddWidget(optb, 0, 6);
//  doneb = new SG_Button("Done", but_normal, but_disabled, but_pressed);
//  main->AddWidget(doneb, 5, 6);
  }

Screen_Play::~Screen_Play() {
  }

ScreenNum Screen_Play::Start(SimpleGUI *gui) {
  audio_stop(cur_music);
  cur_music = NULL;

  PlayResult res = cur_game->Play();

  cur_music = audio_loop(music, 8, 0);

  if(res == PLAY_FINISHED) return SCREEN_RESULTS;
  else if(res == PLAY_CONFIG) return SCREEN_CONFIG;
  else if(res == PLAY_SAVE) return SCREEN_RESULTS; //For now
  else {
    fprintf(stderr, "ERROR: Game::Play() failure!\n");
    exit(1);
    }
  return SCREEN_SAME;
  }

void Screen_Play::Finish(SimpleGUI *gui) {
//  Do Nothing!
  }

ScreenNum Screen_Play::Handle(SimpleGUI *gui, SDL_Event &event) {
//  Do Nothing!
  return SCREEN_SAME;
  }

Popup_LoadMap::Popup_LoadMap() {
  main = new SG_FileBrowser("*.map");
  }

Popup_LoadMap::~Popup_LoadMap() {
  //FIXME: Fill!
  }

ScreenNum Popup_LoadMap::Handle(SimpleGUI *gui, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_FILEOPEN) {
      string fn = ((SG_FileBrowser *)(main))->FileName();
      if(!cur_game) cur_game = new Game;
      if(!cur_game->Load(fn)) {
	delete cur_game;
	cur_game = NULL;
	fprintf(stderr, "WARNING: Could not load map file '%s'\n", fn.c_str());
	}
      else {
	//cur_game->Save(fn); // Uncomment for auto-upgrade of mapfile

	cur_game->AttachPlayer( // temporary!
		new Player_Local(cur_game, PLAYER_LOCAL, 0)
		);

	return POPUP_CLEAR;
	}
      }
    }
  return SCREEN_SAME;
  }
