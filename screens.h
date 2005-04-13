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

#ifndef SCREENS_H
#define	SCREENS_H

#include <map>
#include <vector>
using namespace std;

#include "../simplevideo/simplevideo.h"
#include "../simplegui/simplegui.h"
#include "../simpleconnect/simpleconnect.h"

enum ScreenNum {
  POPUP_MIN,
  POPUP_LOADMAP,
  POPUP_MAX,
  POPUP_CLEAR,
  SCREEN_BACK,
  SCREEN_SAME,
  SCREEN_NONE,
  SCREEN_MIN,
  SCREEN_TITLE,
  SCREEN_CONFIG,
  SCREEN_SINGLE,
  SCREEN_MULTI,
  SCREEN_REPLAY,
  SCREEN_PLAY,    //Actually not handled here but in the player object
//  SCREEN_EQUIP,   //Not really going to be handled within Screens class
//  SCREEN_DECLARE, //Not really going to be handled within Screens class
//  SCREEN_WATCH,   //Not really going to be handled within Screens class
  SCREEN_RESULTS,
  SCREEN_MAX
  };

class Screen;

class Screens {
public:
  Screens();
  ~Screens();
  int Handle();

private:
  void Set(ScreenNum s);
  ScreenNum screen, last_screen, popup;
  SimpleGUI *gui;

  map<ScreenNum, Screen *> sscr;	//Map of ScreenNums to Screens
  int click;				//Button Click Sound

  SimpleVideo *renderer;
  };

#endif // SCREENS_H
