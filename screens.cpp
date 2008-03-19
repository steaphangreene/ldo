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

#include "SDL_net.h"
#include "SDL_image.h"

#include "screens.h"
#include "click.h"

#include "game.h"
#include "player.h"
#include "player_local.h"
#include "player_ai.h"

#include "simplemodel.h"
#include "simpleconfig.h"

Game *cur_game = NULL;			//Temporary, just for testing

static int drkred = 0;	//Global colordef
static vector<int> cols; //Team colors

int click = 0;			//Button Click Sound
int music = 0;			//Background Music (Temporary)
PlayingSound cur_music = -1;	//Currently Playing Music (Temporary)

class Screen {
public:
  Screen() { main = NULL; };
  virtual ~Screen() { if(main) delete main; };
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual void Render(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Table *main;
  };

class Screen_Title : public Screen {
public:
  Screen_Title();
  virtual ~Screen_Title();
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
  virtual void Render(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
protected:
  SG_Button *optb, *multb, *singb, *replb, *quitb;
  SimpleModel *guy, *weap;
  vector<int> anims;
  vector<Uint32> times;
  };

class Screen_Config : public Screen {
public:
  Screen_Config();
  virtual ~Screen_Config();
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
protected:
  SG_Button *backb;
  };

class Screen_Single : public Screen {
public:
  Screen_Single();
  virtual ~Screen_Single();
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob, *contb;
  SG_ScrollingArea *connscr;
  SimpleConnect *connector;
  };

class Screen_Multi : public Screen {
public:
  Screen_Multi();
  virtual ~Screen_Multi();
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
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
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
protected:
  SG_Button *cancelb, *optb, *loadb, *gob;
  };

class Screen_Play : public Screen {
public:
  Screen_Play();
  virtual ~Screen_Play();
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
  virtual void Finish(SimpleGUI *gui);
protected:
  SG_Button *optb, *doneb;
  };

class Screen_Results : public Screen {
public:
  Screen_Results();
  virtual ~Screen_Results();
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
protected:
  SG_Button *replb, *saveb, *doneb, *quitb;
  };


class Popup : public Screen {
public:
  Popup() { main = NULL; };
  virtual ~Popup() {};
  virtual ScreenNum Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio);
  virtual void Finish(SimpleGUI *gui);
  };

class Popup_LoadMap : public Popup {
public:
  Popup_LoadMap();
  virtual ~Popup_LoadMap();
  virtual ScreenNum Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event);
  };



Screens::Screens() {
  popup = SCREEN_NONE;
  screen = SCREEN_NONE;
  last_screen = SCREEN_NONE;

  video = new SimpleVideo(1024, 640, 16.0/10.0);
  audio = new SimpleAudio(2048);
  click = audio->BuildSound((Uint8*)click_data, sizeof(click_data));
  music = audio->LoadMusic("music/cantus.ogg");
  cur_music = audio->Loop(music);

  gui = new SimpleGUI(ASPECT_FIXED_Y|ASPECT_FIXED_X, 16.0/10.0);
  gui->LoadFont("fonts/Denmark Regular.ttf", 100);

  gui->SetColor(SG_COL_BG,	0.250, 0.0, 0.0);
  gui->SetColor(SG_COL_FG,	0.750, 0.0, 0.0);
  gui->SetColor(SG_COL_RAISED,	0.500, 0.0, 0.0);
  gui->SetColor(SG_COL_LOW,	0.375, 0.0, 0.0);
  gui->SetColor(SG_COL_HIGH,	0.625, 0.0, 0.0);

  cols.push_back(gui->NewColor(0.0, 1.0, 0.0));
  cols.push_back(gui->NewColor(0.0, 0.0, 1.0));
  cols.push_back(gui->NewColor(1.0, 0.0, 0.0));	//Temporary Model Has No Yellow
//  cols.push_back(gui->NewColor(1.0, 1.0, 0.0));

  SDL_Surface *mouse_cursor = IMG_Load("graphics/cursor.png");
  gui->SetMouseCursor(mouse_cursor, 0.125, 0.125);

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

  audio->Stop(cur_music);
  cur_music = -1;

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
      sscr[popup]->Start(gui, video, audio);
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
    ScreenNum res = sscr[screen]->Start(gui, video, audio);
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
	  case(SG_EVENT_OK):
	  case(SG_EVENT_SELECT):
	  case(SG_EVENT_CANCEL):
	  case(SG_EVENT_BUTTONPRESS): {
	    audio->Play(click);
	    }break;
	  default: {
	    }break;
	  }
	}

      //This calls the individual popup (if there is one)
      if(popup != SCREEN_NONE && sscr.count(popup)) {
	ScreenNum next = sscr[popup]->Handle(gui, video, audio, event);
	if(next != SCREEN_SAME) Set(next);
	}

      //This calls the individual screen (if there is one - there should be!)
      if(screen != SCREEN_NONE && sscr.count(screen)) {
	ScreenNum next = sscr[screen]->Handle(gui, video, audio, event);
	if(next != SCREEN_SAME) Set(next);
	}
      }

    video->StartScene();
    gui->RenderStart(SDL_GetTicks());
    if(screen != SCREEN_NONE) sscr[screen]->Render(gui, video, audio);
    gui->RenderFinish(SDL_GetTicks());
    video->FinishScene();
    }

  return 0;
  }


ScreenNum Screen::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  gui->MasterWidget()->AddWidget(main);
  return SCREEN_SAME;
  }

void Screen::Render(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  }

void Screen::Finish(SimpleGUI *gui) {
  gui->MasterWidget()->RemoveWidget(main);
  }

ScreenNum Screen::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
  return SCREEN_SAME;
  }

ScreenNum Popup::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  gui->SetPopupWidget(main);
  return SCREEN_SAME;
  }

void Popup::Finish(SimpleGUI *gui) {
  gui->UnsetPopupWidget();
  }

Screen_Config::Screen_Config() {
  main = new SG_Table(6, 10, 0.0625, 0.125);

/*
  SG_Alignment *align;	// For temporary storage;

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

  main->AddWidget(new SG_MultiTab(cfg_tab, cfg_scr, 12);
	0, 0, 5, 7);
*/

  main->AddWidget(new SimpleConfig, 0, 0, 5, 10);

  backb = new SG_Button("Back");
  main->AddWidget(backb, 5, 0);
  }

Screen_Config::~Screen_Config() {
  //FIXME: Fill!
  }

ScreenNum Screen_Config::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  Screen::Start(gui, video, audio);
  return SCREEN_SAME;
  }

ScreenNum Screen_Config::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)backb) return SCREEN_BACK;
      }
    }
  return SCREEN_SAME;
  }

Screen_Title::Screen_Title() {
  main = new SG_Table(4, 13, 0.0625, 0.125);
  optb = new SG_Button("Options");
  main->AddWidget(optb, 3, 1);
  multb = new SG_Button("Multiplayer");
  main->AddWidget(multb, 3, 3);
  singb = new SG_Button("Single Player");
  main->AddWidget(singb, 3, 4);
  replb = new SG_Button("View Replay");
  main->AddWidget(replb, 3, 6);
  quitb = new SG_Button("Quit Game");
  main->AddWidget(quitb, 3, 8);

  SG_TextArea *title = new SG_TextArea("LDO", drkred);
  title->SetMargins(0.125, 0.0);
  title->SetVisibleSize(SG_AUTOSIZE, SG_AUTOSIZE);
  title->SetFontSize(400);
  main->AddWidget(title, 0, 0, 3, 4);

  SG_AutoScroll *scr =
	new SG_AutoScroll(1.0, 15.0, 0.0, -15.0, 0.0, -15.0, 0.0, 60.0);
  main->AddWidget(scr, 0, 4, 3, 9);

  FILE *credfl = fopen("CREDITS", "r");
  if(credfl) {
    fseek(credfl, 0, SEEK_END);
    int size = ftell(credfl);
    fseek(credfl, 0, SEEK_SET);
    char *data = new char[size+1];
    fread(data, 1, size, credfl); 
    data[size] = 0;
//    SG_TextArea *credits = new SG_TextArea(data, drkred);
    SG_TransLabel *credits = new SG_TransLabel(data, drkred);
    credits->SetMargins(0.125, 0.03125);
    credits->SetFontSize(20);
    credits->SetVisibleLines(15);
    scr->AddWidget(credits);
    delete [] data;
    fclose(credfl);

    double vislines = credits->NumLines()+30.0;
    scr->SetYScroll(-15.0, vislines-15.0, vislines*2.0);
    }

  SimpleModel::AddSourceFile("models/md3-trooper");
  SimpleModel::AddSourceFile("models/wep2-m41a");
  guy = SM_LoadModel("models/players/trooper");
  weap = SM_LoadModel("models/weapons2/machinegun/machinegun.md3");
  guy->AttachSubmodel("tag_weapon", weap);
  Uint32 t = SDL_GetTicks()/2;
//  anims.push_back(guy->LookUpAnimation("LEGS_IDLE")); //Trooper is to jittery!
  anims.push_back(guy->LookUpAnimation("LEGS_WALK"));
  anims.push_back(guy->LookUpAnimation("TORSO_STAND"));
  times.push_back(t);
  times.push_back(t);
  }

ScreenNum Screen_Title::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  Screen::Start(gui, video, audio);
  video->SetPerspective(45.0);
  video->SetSubscreen(-1.0, -1.0, 0.5, (1.0/3.0));
  video->SetAngle(90.0, 0);
  video->SetDown(0.5, 0);
  video->SetPosition(0.0, 0.0, 0);
  video->SetZoom(2.0, 0);
  video->SetMove(0.0, 0.0);
  video->SetZPosition(0.0, 0);
  video->SetZExtents(-3.0, 1.5);
  return SCREEN_SAME;
  }

void Screen_Title::Render(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  glTranslatef(0.0, 0.0, -3.0);
  Uint32 t = SDL_GetTicks()/2;
  glScalef(1.5, 1.5, 1.5);
  guy->Render(t, anims, times);
  }

Screen_Title::~Screen_Title() {
  //FIXME: Fill!
  if(guy) delete guy;
  guy = NULL;
  if(weap) delete weap;
  weap = NULL;
  }

ScreenNum Screen_Title::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
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
  main = new SG_Table(6, 12, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Define Teams", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel");
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options");
  main->AddWidget(optb, 5, 1);
  loadb = new SG_Button("Load");
  main->AddWidget(loadb, 5, 2);
  gob = new SG_Button("Go");
  main->AddWidget(gob, 5, 6);
  gob->Disable();
  contb = new SG_Button("Continue");
  main->AddWidget(contb, 5, 7);
  contb->Disable();
  connscr = new SG_ScrollingArea(SG_AUTOSIZE, 12.0);
  main->AddWidget(connscr, 0, 2, 5, 10);
  connector = new SimpleConnect();
  connector->SetColors(cols);
  connscr->AddWidget(connector);
  }

Screen_Single::~Screen_Single() {
  //FIXME: Fill!
  }

ScreenNum Screen_Single::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
  bool play_game = false, create_game = false;
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)loadb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)contb) play_game = true;
      else if(event.user.data1 == (void*)gob) {
	play_game = true;
	create_game = true;
	}
      }
    else if(event.user.code == SG_EVENT_CONNECTDONE) {
      play_game = true;
      create_game = true;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      contb->Disable();
      if(cur_game) gob->Enable();
      vector<SC_SlotType> slots;
      vector<int> slot_cols;
      for(int side = 0; side < cur_game->MapNumSides(); ++side) {
	for(int pl = 0; pl < cur_game->MapNumSidePlayers(side); ++pl) {
	  slots.push_back(SC_SLOT_PLAYER);
	  slot_cols.push_back(side+1);
	  }
	}
      connector->SetSlots(slots);
      connector->SetSlotColors(slot_cols);
      connector->SetSlotTeams(slot_cols);
      connector->Config();
      }
    }
  if(!play_game) return SCREEN_SAME;

  if(create_game) {
    SimpleConnections conn = connector->ClaimConnections();
    vector<SlotData>::iterator slot = conn.slots.begin();
    for(int pn=0; slot != conn.slots.end(); ++slot, ++pn) {
      if(slot->ptype == SC_PLAYER_LOCAL) {
	cur_game->AttachPlayer(new Player_Local(cur_game, PLAYER_LOCAL, pn,
		slot->color - 1));
	}
      else { //if(slot->ptype == SC_PLAYER_AI) {
	cur_game->AttachPlayer(new Player_AI(cur_game, PLAYER_AI, pn,
		slot->color - 1));
	}
      }
    gob->Disable();
    contb->Enable();
    }

  return SCREEN_PLAY;
  }

Screen_Multi::Screen_Multi() {
  net_init = false;
  main = new SG_Table(6, 12, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Gather Players", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel");
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options");
  main->AddWidget(optb, 5, 1);
  hostb = new SG_Button("Host");
  main->AddWidget(hostb, 5, 2);
  scanb = new SG_Button("Search");
  main->AddWidget(scanb, 5, 3);
  readyb = new SG_StickyButton("Ready");
  main->AddWidget(readyb, 5, 5);
  gob = new SG_Button("Go");
//  gob->SetAlignment(SG_ALIGN_CENTER);	//Temporary!
  main->AddWidget(gob, 5, 6);
  gob->Disable();
  connscr = new SG_ScrollingArea(SG_AUTOSIZE, 12.0);
  main->AddWidget(connscr, 0, 2, 5, 10);
  connector = new SimpleConnect();
  connector->SetColors(cols);
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

ScreenNum Screen_Multi::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  Screen::Start(gui, video, audio);
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

ScreenNum Screen_Multi::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)hostb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) return SCREEN_PLAY;
      else if(event.user.data1 == (void*)scanb) {
	connector->Search();
	}
      }
    else if(event.user.code == SG_EVENT_STICKYON) {
      //if(cur_game) gob->Enable();	//FIXME: Multiplayer Disabled
      }
    else if(event.user.code == SG_EVENT_STICKYOFF) {
      gob->Disable();
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      if(cur_game && readyb->IsOn()) {
	//gob->Enable();	//FIXME: Multiplayer Disabled
	}
      connector->SetMapName(cur_game->MapName());
      vector<SC_SlotType> slots;
      vector<int> slot_cols;
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(1);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(3);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(3);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(3);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(3);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      slots.push_back(SC_SLOT_PLAYER);
      slot_cols.push_back(2);
      connector->SetSlots(slots);
      connector->SetSlotColors(slot_cols);
      connector->SetSlotTeams(slot_cols);
      connector->Host();
      }
    else if(event.user.code == SG_EVENT_OK) {
      }
    else if(event.user.code == SG_EVENT_CONNECTDONE) {
      SimpleConnections conn = connector->ClaimConnections();
      char buf[16] = {0};
      if(conn.sock) { // Client
	SDLNet_TCP_Recv(conn.sock, buf, 16);
	fprintf(stderr, "Received '%s' from server\n", buf);
	SDLNet_TCP_Close(conn.sock);
	conn.sock = NULL;

	fprintf(stderr, "ERROR: Networking not yet implemented!\n");
	exit(1);
	}
      else {		// Server
	sprintf(buf, "Hello%c", 0);
	vector<SlotData>::iterator slot = conn.slots.begin();
	for(; slot != conn.slots.end(); ++slot) {
	  if(slot->sock) {
	    SDLNet_TCP_Send(slot->sock, buf, 16);
	    SDLNet_TCP_Close(slot->sock);	// Close it, no networking yet
	    slot->sock = NULL;
	    }
	  }
	fprintf(stderr, "Sent data to clients\n");
	}
      gob->Disable();
      return SCREEN_PLAY;
      }
    }
  return SCREEN_SAME;
  }

Screen_Replay::Screen_Replay() {
  main = new SG_Table(6, 12, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Load Replay", drkred), 0, 0, 5, 2);
  cancelb = new SG_Button("Cancel");
  main->AddWidget(cancelb, 5, 0);
  optb = new SG_Button("Options");
  main->AddWidget(optb, 5, 1);
  loadb = new SG_Button("Load");
  main->AddWidget(loadb, 5, 2);
  gob = new SG_Button("Go");
//  gob->SetAlignment(SG_ALIGN_RIGHT);	//Temporary!
  main->AddWidget(gob, 5, 6);
  gob->Disable();
  }

Screen_Replay::~Screen_Replay() {
  //FIXME: Fill!
  }

ScreenNum Screen_Replay::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
  if(event.type == SDL_SG_EVENT) {
    if(event.user.code == SG_EVENT_BUTTONCLICK) {
      if(event.user.data1 == (void*)cancelb) return SCREEN_TITLE;
      else if(event.user.data1 == (void*)optb) return SCREEN_CONFIG;
      else if(event.user.data1 == (void*)loadb) return POPUP_LOADMAP;
      else if(event.user.data1 == (void*)gob) return SCREEN_PLAY;
      }
    else if(event.user.code == SG_EVENT_FILEOPEN) {
      //if(cur_game) gob->Enable();	//FIXME: Replay Disabled
      }
    }
  return SCREEN_SAME;
  }


Screen_Results::Screen_Results() {
  main = new SG_Table(6, 10, 0.0625, 0.125);
  main->AddWidget(new SG_TextArea("Game Results", drkred), 0, 0, 5, 2);
  replb = new SG_Button("Replay");
  main->AddWidget(replb, 5, 0);
  saveb = new SG_Button("Save");
  main->AddWidget(saveb, 5, 1);
  doneb = new SG_Button("Done");
  main->AddWidget(doneb, 5, 2);
  quitb = new SG_Button("Quit");
  main->AddWidget(quitb, 5, 6);
  }

Screen_Results::~Screen_Results() {
  //FIXME: Fill!
  }

ScreenNum Screen_Results::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  Screen::Start(gui, video, audio);
  return SCREEN_SAME;
  }

ScreenNum Screen_Results::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
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
//  optb = new SG_Button("Options");
//  main->AddWidget(optb, 0, 6);
//  doneb = new SG_Button("Done");
//  main->AddWidget(doneb, 5, 6);
  }

Screen_Play::~Screen_Play() {
  }

ScreenNum Screen_Play::Start(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio) {
  audio->Stop(cur_music);
  cur_music = -1;

  PlayResult res = cur_game->Play();

  cur_music = audio->Loop(music, 8, 0);

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

ScreenNum Screen_Play::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
//  Do Nothing!
  return SCREEN_SAME;
  }

Popup_LoadMap::Popup_LoadMap() {
  main = new SG_FileBrowser("*.map");
  }

Popup_LoadMap::~Popup_LoadMap() {
  //FIXME: Fill!
  }

ScreenNum Popup_LoadMap::Handle(SimpleGUI *gui, SimpleVideo *video, SimpleAudio *audio, SDL_Event &event) {
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

	//cur_game->AttachPlayer( // temporary!
	//	new Player_Local(cur_game, PLAYER_LOCAL, 0)
	//	);

	return POPUP_CLEAR;
	}
      }
    else if(event.user.code == SG_EVENT_CANCEL) {
      return POPUP_CLEAR;
      }
    }
  return SCREEN_SAME;
  }
