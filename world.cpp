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

#define SEL_BASE 0.0625
#define SEL_HEIGHT 4.0

#include <cmath>
using namespace std;

#include "world.h"

World::World(Percept *per, Orders *ord) {
  percept = per;
  orders = ord;
  angle = 45;
  pointx = 0.0;
  pointy = 0.0;
  models.push_back(SM_LoadModel("models/players/trooper"));
  models.push_back(SM_LoadModel("models/players/trooper", "blue"));
  models.push_back(SM_LoadModel("models/players/trooper", "red"));
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

  glPushMatrix();
  glBegin(GL_QUADS);	//Temporary test world

  glBindTexture(GL_TEXTURE_2D, 0);
  glNormal3d(0.0, 0.0, 1.0);
  for(float x = 1.0; x <= 127.0; x += 2.0) {
    for(float y = 1.0; y <= 127.0; y += 2.0) {
      glColor3f(x/126.0, 0.5, y/126.0);
      glVertex3f(x-1.0, y-1.0, 0.0);
      glVertex3f(x+1.0, y-1.0, 0.0);
      glVertex3f(x+1.0, y+1.0, 0.0);
      glVertex3f(x-1.0, y+1.0, 0.0);
      }
    }
  glEnd();

  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINES);
  for(float part = 0.0; part <= 128.0; part += 2.0) {
    glVertex3f(part,  0.0,   0.03125);
    glVertex3f(part,  128.0, 0.03125);
    glVertex3f(0.0,   part,  0.03125);
    glVertex3f(128.0, part,  0.03125);
    }
  glEnd();
  glPopMatrix();
  }

void World::DrawModels(Uint32 offset) {
  vector<int> anims;
  vector<Uint32> times;

  anims.push_back(LEGS_IDLE);
  anims.push_back(TORSO_STAND);
  times.push_back(0);
  times.push_back(0);

  vector<UnitAct>::iterator act = percept->my_acts.begin();
  for(; act != percept->my_acts.end(); ++act) {
    if(act->time <= offset) {
      times[0] = act->time;
      times[1] = act->time;
      float x = act->x * 2 + 1;
      float y = act->y * 2 + 1;
      glPushMatrix();
      glTranslatef(x, y, 0.0);
      models[1]->Render(offset, anims, times);
      glPopMatrix();
      }
    }
  }

void World::Render() {			// Render for declaration
  DrawMap();
  DrawModels(3000);
  }

void World::Render(Uint32 offset) {	// Render for playback
  DrawMap();
  DrawModels(offset);
  }

void World::DrawSelBox(int sel_x, int sel_y, float r, float g, float b) {
        //FIXME: Use REAL map x and y size for limits
  if(sel_x < 0 || sel_y < 0 || sel_x >= 64 || sel_y >= 64) return;

//  fprintf(stderr, "Selbox drawing at %d,%d\n", sel_x, sel_y);

  glDisable(GL_LIGHTING);
  glPushMatrix();

  glTranslatef(sel_x*2.0+1.0, sel_y*2.0+1.0, 0.0);

  glBindTexture(GL_TEXTURE_2D, 0);

  glColor3f(r, g, b);

  glBegin(GL_LINES);

  glVertex3f(-1.0, -1.0, SEL_BASE);
  glVertex3f(-1.0,  1.0, SEL_BASE);

  glVertex3f(-1.0,  1.0, SEL_BASE);
  glVertex3f( 1.0,  1.0, SEL_BASE);

  glVertex3f( 1.0,  1.0, SEL_BASE);
  glVertex3f( 1.0, -1.0, SEL_BASE);

  glVertex3f( 1.0, -1.0, SEL_BASE);
  glVertex3f(-1.0, -1.0, SEL_BASE);

  glVertex3f(-1.0, -1.0, SEL_HEIGHT);
  glVertex3f(-1.0,  1.0, SEL_HEIGHT);

  glVertex3f(-1.0,  1.0, SEL_HEIGHT);
  glVertex3f( 1.0,  1.0, SEL_HEIGHT);

  glVertex3f( 1.0,  1.0, SEL_HEIGHT);
  glVertex3f( 1.0, -1.0, SEL_HEIGHT);

  glVertex3f( 1.0, -1.0, SEL_HEIGHT);
  glVertex3f(-1.0, -1.0, SEL_HEIGHT);

  glVertex3f(-1.0, -1.0, SEL_BASE);
  glVertex3f(-1.0, -1.0, SEL_HEIGHT);

  glVertex3f(-1.0,  1.0, SEL_BASE);
  glVertex3f(-1.0,  1.0, SEL_HEIGHT);

  glVertex3f( 1.0,  1.0, SEL_BASE);
  glVertex3f( 1.0,  1.0, SEL_HEIGHT);

  glVertex3f( 1.0, -1.0, SEL_BASE);
  glVertex3f( 1.0, -1.0, SEL_HEIGHT);

  glEnd();

  glPopMatrix();
  }
