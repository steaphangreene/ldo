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
  POPUP_LOADMAP=-3,
  SCREEN_BACK=-2,
  SCREEN_NONE=-1,
  SCREEN_TITLE,
  SCREEN_CONFIG,
  SCREEN_MULTI,
  SCREEN_SINGLE,
  SCREEN_REPLAY,
  SCREEN_EQUIP,
  SCREEN_PLAY, //Not Handled within Screens class
  SCREEN_RESULTS,
  SCREEN_MAX
  };

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

class Screen_Multi : public Screen {
public:
  Screen_Multi();
  virtual ~Screen_Multi();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  };

class Screen_Single : public Screen {
public:
  Screen_Single();
  virtual ~Screen_Single();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  };

class Screen_Replay : public Screen {
public:
  Screen_Replay();
  virtual ~Screen_Replay();
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
  };

class Screen_Equip : public Screen {
public:
  Screen_Equip();
  virtual ~Screen_Equip();
  virtual void Start(SimpleGUI *gui);
  virtual ScreenNum Handle(SimpleGUI *gui, SDL_Event &event);
protected:
  SG_TextArea *name;
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
  };

class Screens {
public:
  Screens();
  ~Screens();
  int Handle();

private:
  void Set(ScreenNum s);
  ScreenNum screen, last_screen;
  SimpleGUI *gui;
  map<int, SG_Alignment *> swidget;//Map of ScreenNums to Screen Widgets (TEMP)
  map<SG_Widget *, ScreenNum> smap;	//Map of buttons->resulting screens

  map<int, Screen *> sscr;		//Map of ScreenNums to Screens
  };

#endif // SCREENS_H
