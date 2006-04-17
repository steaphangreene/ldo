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

#ifndef WORLD_H
#define WORLD_H

#include "SDL.h"
#include "SDL_opengl.h"

#include "simplemodel.h"

#include "percept.h"
#include "orders.h"

class World {
public:
  World(Percept *per, Orders *ord);
  ~World();

  void Render(Uint32 offset);		// Render for playback

  void SetViewPoint(float xv, float yv);
  void SetViewAngle(int ang);

  void DrawSelBox(int sel_x, int sel_y, float r=1.0, float g=1.0, float b=1.0);

protected:
  void DrawMap();
  void DrawModels(Uint32 offset);

  Percept *percept;
  Orders *orders;

  int angle;
  float pointx, pointy;

  vector<SimpleModel *> models;
  };

#endif // WORLD_H

