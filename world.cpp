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

#include <cmath>
using namespace std;

#include "world.h"

World::World(Percept *per, Orders *ord) {
  percept = per;
  orders = ord;
  angle = 45;
  pointx = 0.0;
  pointy = 0.0;
  }

World::~World() {
  }

void World::SetViewPoint(float xv, float yv) {
  pointx = xv;
  pointy = yv;
  }

void World::SetViewAngle(int ang) {
  angle = ang;
  }

void World::DrawMap() {
  GLfloat shininess[] = { 128.0 * 0.75 };
  GLfloat specular[] = { 0.75, 0.75, 0.75, 1.0 };
  GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat diffuse[] = { 0.5 ,0.5 ,0.5, 1.0 };

  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

  glBegin(GL_QUADS);	//Temporary test world

  glNormal3d(0.0, 0.0, 1.0);
  for(float x = 0.0; x < 64.0; x += 1.0) {
    for(float y = 0.0; y < 64.0; y += 1.0) {
      glColor3f(x/63.0, 0.5, y/63.0);
      glVertex3f(x,     y,     0.0);
      glVertex3f(x+1.0, y,     0.0);
      glVertex3f(x+1.0, y+1.0, 0.0);
      glVertex3f(x,     y+1.0, 0.0);
      }
    }
  glEnd();

  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINES);
  for(float part = 0.0; part <= 64.0; part += 1.0) {
    glVertex3f(part, 0.0,  0.0625);
    glVertex3f(part, 64.0, 0.0625);
    glVertex3f(0.0,  part, 0.0625);
    glVertex3f(64.0, part, 0.0625);
    }
  glEnd();
  }

void World::DrawModels(Uint32 offset) {
  //FIXME: Implement This!
  }

void World::Render() {			// Render for declaration
  DrawMap();
  DrawModels(3000);
  }

void World::Render(Uint32 offset) {	// Render for playback
  DrawMap();
  DrawModels(offset);
  }

