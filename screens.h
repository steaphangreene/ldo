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

#include "../simplegui/simplegui.h"

enum ScreenNum {
  SCREEN_NONE=-1,
  SCREEN_TITLE,
  SCREEN_MULTI,
  SCREEN_SINGLE,
  SCREEN_REPLAY,
  SCREEN_EQUIP,
  SCREEN_PLAY, //Not Handled within Screens class
  SCREEN_RESULTS,
  SCREEN_MAX
  };

class Screens {
public:
  Screens();
  ~Screens();
  int Handle();

private:
  void Set(ScreenNum s);
  ScreenNum screen;
  SimpleGUI *gui;
  int click;
  vector<SG_Widget *> swidget;
  map<SG_Widget *, ScreenNum> smap; //Map of buttons->resulting screens
  };

#endif // SCREENS_H

