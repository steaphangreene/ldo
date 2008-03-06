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

#define SEL_BASE 0.0625
#define SEL_HEIGHT 4.0

#include <cmath>
using namespace std;

#include "world.h"
#include "game.h"

extern Game *cur_game;

World::World(Percept *per, Orders *ord) {
  percept = per;
  orders = ord;
  angle = 45;
  pointx = 0.0;
  pointy = 0.0;
  SimpleModel *weap = SM_LoadModel("models/weapons2/machinegun/machinegun.md3");
  models.push_back(SM_LoadModel("models/players/trooper"));
  models.back()->AttachSubmodel("tag_weapon", weap);
  models.push_back(SM_LoadModel("models/players/trooper", "blue"));
  models.back()->AttachSubmodel("tag_weapon", weap);
  models.push_back(SM_LoadModel("models/players/trooper", "red"));
  models.back()->AttachSubmodel("tag_weapon", weap);

  modmap[25] = models.size();
  modmap[26] = models.size();
  models.push_back(SM_LoadModel("models/fence1.obj"));

  modmap[29] = models.size();
  modmap[30] = models.size();
  models.push_back(SM_LoadModel("models/fence2.obj"));

  modmap[16] = models.size();
  modmap[17] = models.size();
  modmap[20] = models.size();
  modmap[21] = models.size();
  models.push_back(SM_LoadModel("models/wall_low.obj"));

  modmap[18] = models.size();
  modmap[19] = models.size();
  models.push_back(SM_LoadModel("models/wall_high.obj"));

  textures.push_back(new SimpleTexture("graphics/wall.png"));
  for(int n=1; n < 1000; ++n) {
    char name[64];
    sprintf(name, "graphics/util/test%.3d.png%c", n, 0);
    textures.push_back(new SimpleTexture(name));
    }
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
  GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };

  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

  glPushMatrix();
  glColor3f(1.0, 1.0, 1.0);
  vector<MapObject>::const_iterator obj = percept->objects.begin();
  for(; obj != percept->objects.end(); ++obj) {
    glColor3f(obj->xpos/float(50), 0.5, obj->ypos/float(50));
    if(obj->type == GROUND_FLOOR) {
      glBindTexture(GL_TEXTURE_2D, textures[obj->which]->GLTexture());
      glNormal3d(0.0, 0.0, 1.0);
      glBegin(GL_QUADS);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.0, obj->ypos*2.0+0.0, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0+0.0, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0+2.0, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.0, obj->ypos*2.0+2.0, obj->zpos*4.0);
      glEnd();
      }
    else if(obj->type == WALL_EASTWEST) {
      if(modmap.count(obj->which) > 0) {
	glPushMatrix();
	glTranslatef(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0);
	models[modmap[obj->which]]->Render(0);
	glPopMatrix();
	}
      else {
	glBindTexture(GL_TEXTURE_2D, textures[obj->which]->GLTexture());
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0, obj->zpos + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0, obj->zpos);

	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0, obj->zpos);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0+2.0, obj->ypos*2.0, obj->zpos + SEL_HEIGHT);
	glEnd();
	}
      }
    else if(obj->type == WALL_NORTHSOUTH) {
      if(modmap.count(obj->which) > 0) {
	glPushMatrix();
	glTranslatef(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	models[modmap[obj->which]]->Render(0);
	glPopMatrix();
	}
      else {
	glBindTexture(GL_TEXTURE_2D, textures[obj->which]->GLTexture());
	glNormal3d(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0 + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0+2.0, obj->zpos*4.0 + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0+2.0, obj->zpos*4.0);

	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0 + SEL_HEIGHT);
	glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0, obj->zpos*4.0);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0+2.0, obj->zpos*4.0);
	glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
	glVertex3f(obj->xpos*2.0, obj->ypos*2.0+2.0, obj->zpos*4.0 + SEL_HEIGHT);
	glEnd();
	}
      }
    else if(obj->type == OBJECT_MISC) {
      glBindTexture(GL_TEXTURE_2D, textures[obj->which]->GLTexture());
      glNormal3d(1.0, 0.0, 0.0);
      glBegin(GL_QUADS);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);

      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+0.2, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+1.8, obj->zpos*4.0);

      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+0.2, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+1.8, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);

      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+0.2, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+0.2, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+0.2, obj->zpos*4.0+1.0);

      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+1.8, obj->zpos*4.0);
      glTexCoord2f(textures[obj->which]->ScaleX(0.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+0.2, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(1.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+1.8, obj->zpos*4.0+1.0);
      glTexCoord2f(textures[obj->which]->ScaleX(1.0), textures[obj->which]->ScaleY(0.0));
      glVertex3f(obj->xpos*2.0+1.8, obj->ypos*2.0+1.8, obj->zpos*4.0);
      glEnd();
      }
    }

  glPopMatrix();
  }

void World::DrawModels(Uint32 offset) {
  vector<int> anims;
  vector<Uint32> times;

  anims.push_back(0);
  anims.push_back(0);
  times.push_back(0);
  times.push_back(0);

  map<int, UnitAct> unitact;
  map<int, vector<UnitAct> >::const_iterator unitacts;
  unitacts = percept->my_units.begin();
  for(; unitacts != percept->my_units.end(); ++unitacts) {
    unitact.erase(unitacts->first);
    vector<UnitAct>::const_iterator act = unitacts->second.end();  --act;
    for(; act->time > offset && act != unitacts->second.begin();) { --act; }
    unitact.insert(pair<int,UnitAct>(unitacts->first, *act));
    }
  unitacts = percept->other_units.begin();
  for(; unitacts != percept->other_units.end(); ++unitacts) {
    unitact.erase(unitacts->first);
    vector<UnitAct>::const_iterator act = unitacts->second.end();  --act;
    for(; act->time > offset && act != unitacts->second.begin();) { --act; }
    unitact.insert(pair<int,UnitAct>(unitacts->first, *act));
    }

  map<int, UnitAct>::const_iterator mapact = unitact.begin();
  for(; mapact != unitact.end(); ++mapact) {
    const UnitAct *act = &(mapact->second);
    int mod = cur_game->PlayerForUnit(act->id)->Color();
    if(act->time <= offset) {
      anims[0] = models[mod]->LookUpAnimation("LEGS_IDLE");
      anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
      times[0] = act->time;
      times[1] = act->time;
      float x = act->x * 2 + 1;
      float y = act->y * 2 + 1;
      float a = 0.0;
      if(act->act == ACT_STAND) {
	if(act->time + 0 <= offset) {
	  x = act->targ1 * 2 + 1;
	  y = act->targ2 * 2 + 1;
	  }
	}
      else if(act->act == ACT_FALL) {
	if(act->time + 0 <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  if(act->time + 1000 <= offset) {
	    anims[0] = models[mod]->LookUpAnimation("BOTH_DEAD1");
	    anims[1] = models[mod]->LookUpAnimation("BOTH_DEAD1");
	    }
	  else {
	    anims[0] = models[mod]->LookUpAnimation("BOTH_DEATH1");
	    anims[1] = models[mod]->LookUpAnimation("BOTH_DEATH1");
	    }
	  }
	}
      else if(act->act == ACT_MOVE) {
        int dx = act->x - act->targ1;
        int dy = act->y - act->targ2;
	float duration = sqrt(dx*dx + dy*dy) * 333.33333333;
	if((unsigned int)(act->time + duration) <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  }
	else {
	  Uint32 off = offset - act->time;
	  anims[0] = models[mod]->LookUpAnimation("LEGS_WALK");
	  x = (act->targ1 * 2 + 1) * (duration - off) + (act->x * 2 + 1) * off;
	  y = (act->targ2 * 2 + 1) * (duration - off) + (act->y * 2 + 1) * off;
	  x /= duration; y /= duration;
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	}
      else if(act->act == ACT_RUN) {
        int dx = act->x - act->targ1;
        int dy = act->y - act->targ2;
	float duration = sqrt(dx*dx + dy*dy) * 166.66666666;
	if((unsigned int)(act->time + duration) <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  }
	else {
	  Uint32 off = offset - act->time;
	  anims[0] = models[mod]->LookUpAnimation("LEGS_RUN");
	  x = (act->targ1 * 2 + 1) * (duration - off) + (act->x * 2 + 1) * off;
	  y = (act->targ2 * 2 + 1) * (duration - off) + (act->y * 2 + 1) * off;
	  x /= duration; y /= duration;
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	}
      else if(act->act == ACT_SHOOT) {
        int dx = act->targ1 - act->x;
        int dy = act->targ2 - act->y;
	a = 180.0 * atan2f(dy, dx) / M_PI;
	if(act->time + 1500 <= offset) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
	  times[1] += 1500;
	  }
	else {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_ATTACK");
	  }
	}
      else if(act->act == ACT_EQUIP && act->time > 0) {	// First EQUIP Free
	if(act->time + 1500 <= offset) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
	  times[1] += 1500;
	  }
	else if(act->time + 1000 <= offset) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_RAISE");
	  times[1] += 1000;
	  }
	else if(act->time + 500 <= offset) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_DROP");
	  times[1] += 500;
	  }
	else {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
	  }
        }
//      fprintf(stderr, "Action Time: (%d/%d)\n", act->time, offset);
      glPushMatrix();
      glTranslatef(x, y, 0.0);
      glRotatef(a, 0.0, 0.0, 1.0);
      models[mod]->Render(offset, anims, times);
      glPopMatrix();
      }
    else {
      fprintf(stderr, "Not Doing %d on %d, since %d > %d\n",
	act->act, act->id, act->time, offset);
      }
    }
  }

void World::Render(Uint32 offset) {	// Render for playback
  DrawMap();
  DrawModels(offset);
  DrawOrders(offset);
  }

void World::DrawOrders(Uint32 offset) {
  int xo, yo, zo, xt, yt, zt;
  vector<UnitOrder>::const_iterator ord = orders->orders.begin();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  for(; ord != orders->orders.end(); ++ord) {
    percept->GetPos(ord->id, xo, yo, zo);
    float ang = atan2f(ord->targ2 - yo, ord->targ1 - xo);
    xo = xo * 2 + 1; yo = yo * 2 + 1;
    xt = ord->targ1 * 2 + 1; yt = ord->targ2 * 2 + 1;
    if(ord->order == ORDER_MOVE) {
      glColor4f(1.0, 1.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, 0.0625);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, 0.0625);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), 0.0625);
      glVertex3f(xt + cos(ang), yt + sin(ang), 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), 0.0625);
      glEnd();
      }
    else if(ord->order == ORDER_RUN) {
      glColor4f(0.0, 1.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, 0.0625);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, 0.0625);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), 0.0625);
      glVertex3f(xt + cos(ang), yt + sin(ang), 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), 0.0625);
      glEnd();
      }
    else if(ord->order == ORDER_SHOOT) {
      if(ord->targ1 == -1) {
	percept->GetPos(ord->targ2, xt, yt, zt);
	xt = xt * 2 + 1; yt = yt * 2 + 1;
	ang = atan2f(yt - yo, xt - xo);
	}
      glColor4f(1.0, 0.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, 0.0625);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, 0.0625);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), 0.0625);
      glVertex3f(xt + cos(ang), yt + sin(ang), 0.0625);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), 0.0625);
      glEnd();
      }
    }
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
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
