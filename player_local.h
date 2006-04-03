// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2006 Steaphan Greene <stea@cs.binghamton.edu>
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

#ifndef PLAYER_LOCAL_H
#define PLAYER_LOCAL_H

#include "player.h"
#include "world.h"

#include "SDL.h"
#include "SDL_thread.h"
#include "../simplegui/simplegui.h"
#include "../simplevideo/simplevideo.h"
#include "../simpleaudio/simpleaudio.h"

enum Phase {
  PHASE_NONE = -1,
  PHASE_EQUIP,
  PHASE_REPLAY,
  PHASE_DECLARE,
  PHASE_MAX,
  };
enum PopPhase {
  POPPHASE_NONE = -1,
  POPPHASE_OPTIONS,
  POPPHASE_MAX
  };

class Player_Local : public Player {
public:
  Player_Local(Game *gm, PlayerType tp, int num);
  virtual ~Player_Local();
  virtual bool Run();

protected:
  int UnitPresent(int xc, int yc);	// Enemy = -1, Own = 1, None/Neutral = 0

  void UpdateEquipIDs();

  Phase phase;			//Current phase of main window GUI
  Phase nextphase;		//Current phase of main window GUI
  PopPhase popphase;		//Current phase of popup
  PopPhase nextpopphase;	//Current phase of popup

  SimpleGUI *gui;
  SimpleVideo *video;
  SimpleAudio *audio;
  World *world;

  SG_Table *wind[PHASE_MAX];	//Screens for each phase
  int drkred;			//Colors

  SG_MultiTab *ednd;		//Widgets for Equip phase
  vector<SG_DNDBoxes*> dnds;
  SG_TextArea *estats;
  SG_Button *ecancelb, *edoneb;
  vector<int> eqid;		//Data for Equip phase

				//Textures for Equip phase
  SDL_Surface *but_normal, *but_pressed, *but_disabled, *but_activated;
  SDL_Surface *gun_icon, *gren_icon;
  SDL_Surface *equip_bg;


  SG_Button *roptb, *rdoneb;	//Widgets for Replay phase
  SG_TransLabel *rtext, *rstamp;
  SG_Tabs *rcontrols;

  Uint32 last_time;		//Data for Replay phase
  Uint32 offset, last_offset;
  int playback_speed;
  SDL_mutex *off_mut;		//MutEx to protect offsets

  void CalcOffset(Uint32);

  SG_Button *doptb;		//Widgets for Declare phase
  SG_StickyButton *ddoneb;
  SG_TransLabel *dtext;
  SG_PassThrough *dpass;
  vector<string> mactions[3];	//For noselect, friendly, enemy
  vector<string> ractions[3];	//For noselect, friendly, enemy

  int music;			//Background Music (Temporary)
  PlayingSound cur_music;	//Currently Playing Music (Temporary)

  SDL_mutex *vid_mut;		//MutEx to protect renderer

  static int event_thread_func(void *arg);
  int EventHandler();
  int exiting;			//Are we exiting?
  };

#endif // PLAYER_LOCAL_H
