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

#define OFF_BASE 0.0625

#include <cmath>
using namespace std;

#include "world.h"
#include "game.h"

extern Game *cur_game;
extern int cur_zpos;

World::World(Percept *per, Orders *ord) {
  percept = per;
  orders = ord;
  angle = 45;
  pointx = 0.0;
  pointy = 0.0;
  SimpleModel *weap = SM_LoadModel("models/weapons2/machinegun/machinegun.md3");

  models.push_back(SM_LoadModel("models/players/trooper"));
  models.back()->AttachSubmodel("tag_weapon", weap);

  SimpleModel::AddSourceFile("models");
  models.push_back(SM_LoadModel("grey/tris.md2"));

  models.push_back(SM_LoadModel("models/players/trooper", "blue"));
  models.back()->AttachSubmodel("tag_weapon", weap);
  models.push_back(SM_LoadModel("models/players/trooper", "red"));
  models.back()->AttachSubmodel("tag_weapon", weap);

  modmap[25] = models.size();
  modmap[29] = models.size();
  models.push_back(SM_LoadModel("models/fence1.obj"));

  modmap[26] = models.size();
  modmap[30] = models.size();
  models.push_back(SM_LoadModel("models/fence2.obj"));

  modmap[16] = models.size();
  modmap[17] = models.size();
  modmap[20] = models.size();
  modmap[21] = models.size();
  models.push_back(SM_LoadModel("models/wall_low.obj"));
  models.push_back(SM_LoadModel("models/wall_med.obj"));

  modmap[18] = models.size();
  modmap[19] = models.size();
  models.push_back(SM_LoadModel("models/wall_high.obj"));

  modmap[130] = models.size();
  models.push_back(SM_LoadModel("models/ramp_high.obj"));
  modmap[131] = models.size();
  models.push_back(SM_LoadModel("models/ramp_med.obj"));
  modmap[132] = models.size();
  models.push_back(SM_LoadModel("models/ramp_low.obj"));

  for(int n=0; n < 1000; ++n) {
    char name[64];
    sprintf(name, "graphics/util/test%.3d.png%c", n, 0);
    textures.push_back(new SimpleTexture(name));
    }

  texmap[70] = textures.size();
  texmap[130] = textures.size();
  texmap[131] = textures.size();
  texmap[132] = textures.size();
  textures.push_back(new SimpleTexture("graphics/wall.png"));

  texmap[25] = textures.size();
  texmap[26] = textures.size();
  texmap[29] = textures.size();
  texmap[30] = textures.size();
  textures.push_back(new SimpleTexture("models/wood.png"));

  texmap[16] = textures.size();
  texmap[17] = textures.size();
  textures.push_back(new SimpleTexture("models/stone.png"));

  texmap[18] = textures.size();
  texmap[19] = textures.size();
  texmap[20] = textures.size();
  texmap[21] = textures.size();
  textures.push_back(new SimpleTexture("models/hedge.png"));

  texmap[2] = textures.size();
  textures.push_back(new SimpleTexture("models/grass.png"));

  texmap[4] = textures.size();
  textures.push_back(new SimpleTexture("models/dirt.png"));

  texmap[5] = textures.size();
  textures.push_back(new SimpleTexture("models/reaped.png"));

  texmap[6] = textures.size();
  textures.push_back(new SimpleTexture("models/wheat.png"));

  texmap[9] = textures.size();
  textures.push_back(new SimpleTexture("models/cabbage.png"));

  texmap[44] = textures.size();
  texmap[45] = textures.size();
  textures.push_back(new SimpleTexture("models/boards.png"));

  texmap[46] = textures.size();	//FIXME: Window LOS!
  texmap[47] = textures.size();	//FIXME: Window LOS!
  textures.push_back(new SimpleTexture("models/boards_window.png"));

  texmap[56] = textures.size();	//FIXME: Door Open/Close!
  texmap[57] = textures.size();	//FIXME: Door Open/Close!
  textures.push_back(new SimpleTexture("models/door.png"));

  texmap[49] = textures.size();
  texmap[50] = textures.size();
  textures.push_back(new SimpleTexture("models/brick.png"));

  texmap[58] = textures.size();
  textures.push_back(new SimpleTexture("models/pavers.png"));

  texmap[41] = textures.size();
  textures.push_back(new SimpleTexture("models/metalroof.png"));

  //Items that aren't drawn
  modmap[32] = -2;	//Broken Fence
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
  map<MapCoord, MapObject>::const_iterator obj = percept->objects.begin();
  for(; obj != percept->objects.end(); ++obj) {
    int tex = -1, mod = -1;
    if(modmap.count(obj->second.which) > 0) mod = modmap[obj->second.which];
    if(texmap.count(obj->second.which) > 0) tex = texmap[obj->second.which];

    if(obj->first.z > cur_zpos) {	//Items above the view Z plane
      continue;	//Don't draw it
      }

    if(mod < -1) {			//Items that aren't drawn
      continue;	//Don't draw it
      }

    if(tex >= 0) {
      glColor3f(1.0, 1.0, 1.0);
      glBindTexture(GL_TEXTURE_2D, textures[tex]->GLTexture());
      }
    else {
      glColor3f(obj->first.x/float(50), 0.5, obj->first.y/float(50));
      glBindTexture(GL_TEXTURE_2D, textures[obj->second.which]->GLTexture());
      }

    if(obj->second.type == GROUND_FLOOR) {
      glNormal3d(0.0, 0.0, 1.0);
      glBegin(GL_QUADS);
      glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
      glVertex3f(obj->first.x*2.0+0.0, obj->first.y*2.0+0.0, obj->first.z*CELL_HEIGHT);
      glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
      glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0+0.0, obj->first.z*CELL_HEIGHT);
      glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
      glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT);
      glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
      glVertex3f(obj->first.x*2.0+0.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT);
      glEnd();
      }
    else if(obj->second.type == WALL_EASTWEST) {
      if(mod >= 0) {
	glPushMatrix();
	glTranslatef(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	models[mod]->Render(0);
	glPopMatrix();
	}
      else {
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);

	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glEnd();
	}
      }
    else if(obj->second.type == WALL_NORTHSOUTH) {
      if(mod >= 0) {
	glPushMatrix();
	glTranslatef(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	models[mod]->Render(0);
	glPopMatrix();
	}
      else {
	glNormal3d(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT);

	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0, obj->first.y*2.0+2.0, obj->first.z*CELL_HEIGHT + CELL_HEIGHT);
	glEnd();
	}
      }
    else if(obj->second.type == OBJECT_MISC) {
      if(mod >= 0) {
	glPushMatrix();
	glTranslatef(obj->first.x*2.0+1.0, obj->first.y*2.0+1.0, obj->first.z*CELL_HEIGHT);
	models[mod]->Render(0);
	glPopMatrix();
	}
      else {
	glNormal3d(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);

	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);

	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);

	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+0.2, obj->first.z*CELL_HEIGHT+1.0);

	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);
	glTexCoord2f(textures[obj->second.which]->ScaleX(0.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+1.8, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(1.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT);
	glTexCoord2f(textures[obj->second.which]->ScaleX(1.0), textures[obj->second.which]->ScaleY(0.0));
	glVertex3f(obj->first.x*2.0+0.2, obj->first.y*2.0+1.8, obj->first.z*CELL_HEIGHT+1.0);
	glEnd();
	}
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
      float azh, tzh;
      {	MapCoord apos = { act->x, act->y, act->z };
	azh = act->z * CELL_HEIGHT + percept->HeightAt(apos);
	}
      {	MapCoord tpos = { act->targ1, act->targ2, act->targ3 };
	tzh = act->targ3 * CELL_HEIGHT + percept->HeightAt(tpos);
	}
      anims[0] = models[mod]->LookUpAnimation("LEGS_IDLE");
      anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
      if(anims[0] < 0) {
	anims[0] = models[mod]->LookUpAnimation("STAND");
	anims[1] = models[mod]->LookUpAnimation("STAND");
	}
      times[0] = act->time;
      times[1] = act->time;
      float x = act->x * 2 + 1;
      float y = act->y * 2 + 1;
      float z = azh;
      float a = 0.0;
      if(act->act == ACT_STAND) {
	if(act->time + 0 <= offset) {
	  x = act->targ1 * 2 + 1;
	  y = act->targ2 * 2 + 1;
	  z = tzh;
	  }
	}
      else if(act->act == ACT_FALL) {
	if(act->time + 0 <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  z = azh;
	  if(act->time + 1000 <= offset) {
	    int anim = models[mod]->LookUpAnimation("BOTH_DEAD1");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("DEATH3");
	    anims[0] = anim;
	    anims[1] = anim;
	    }
	  else {
	    int anim = models[mod]->LookUpAnimation("BOTH_DEATH1");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("DEATH3");
	    anims[0] = anim;
	    anims[1] = anim;
	    }
	  }
	}
      else if(act->act == ACT_MOVE) {
        int dx = act->x - act->targ1;
        int dy = act->y - act->targ2;
        int dz = act->z - act->targ3;
	float duration = sqrt(dx*dx + dy*dy + dz*dz) * 333.33333333;
	if((unsigned int)(act->time + duration) <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  z = azh;
	  }
	else {
	  Uint32 off = offset - act->time;
	  int anim = models[mod]->LookUpAnimation("LEGS_WALK");
	  if(anim < 0) anim = models[mod]->LookUpAnimation("WALK");
	  if(anim < 0) anim = models[mod]->LookUpAnimation("RUN");
	  anims[0] = anim;
	  x = (act->targ1 * 2 + 1) * (duration - off) + (act->x * 2 + 1) * off;
	  y = (act->targ2 * 2 + 1) * (duration - off) + (act->y * 2 + 1) * off;
	  z = tzh * (duration - off) + azh * off;
	  x /= duration; y /= duration; z /= duration;
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	}
      else if(act->act == ACT_RUN) {
        int dx = act->x - act->targ1;
        int dy = act->y - act->targ2;
        int dz = act->z - act->targ3;
	float duration = sqrt(dx*dx + dy*dy + dz*dz) * 166.66666666;
	if((unsigned int)(act->time + duration) <= offset) {
	  x = act->x * 2 + 1;
	  y = act->y * 2 + 1;
	  z = azh;
	  }
	else {
	  Uint32 off = offset - act->time;
	  int anim = models[mod]->LookUpAnimation("LEGS_RUN");
	  if(anim < 0) anim = models[mod]->LookUpAnimation("RUN");
	  if(anim < 0) anim = models[mod]->LookUpAnimation("WALK");
	  anims[0] = anim;
	  x = (act->targ1 * 2 + 1) * (duration - off) + (act->x * 2 + 1) * off;
	  y = (act->targ2 * 2 + 1) * (duration - off) + (act->y * 2 + 1) * off;
	  z = tzh * (duration - off) + azh * off;
	  x /= duration; y /= duration; z /= duration;
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	}
      else if(act->act == ACT_SHOOT) {
        int dx = act->targ1 - act->x;
        int dy = act->targ2 - act->y;
        //int dz = act->targ3 - act->z;
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
      if(z < (cur_zpos+1)*CELL_HEIGHT) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(a, 0.0, 0.0, 1.0);
	models[mod]->Render(offset, anims, times);
	glPopMatrix();
	}
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
  float zof, ztf;
  vector<UnitOrder>::const_iterator ord = orders->orders.begin();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  for(; ord != orders->orders.end(); ++ord) {
    percept->GetPos(ord->id, xo, yo, zo);
    float ang = atan2f(ord->targ2 - yo, ord->targ1 - xo);
    xo = xo * 2 + 1;
    yo = yo * 2 + 1;
    zof = zo * CELL_HEIGHT + OFF_BASE;
    xt = ord->targ1 * 2 + 1;
    yt = ord->targ2 * 2 + 1;
    ztf = ord->targ3 * CELL_HEIGHT + OFF_BASE;
    if(ord->order == ORDER_MOVE) {
      glColor4f(1.0, 1.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, zof);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, ztf);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, ztf);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, zof);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), ztf);
      glVertex3f(xt + cos(ang), yt + sin(ang), ztf);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), ztf);
      glEnd();
      }
    else if(ord->order == ORDER_RUN) {
      glColor4f(0.0, 1.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, zof);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, ztf);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, ztf);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, zof);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), ztf);
      glVertex3f(xt + cos(ang), yt + sin(ang), ztf);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), ztf);
      glEnd();
      }
    else if(ord->order == ORDER_SHOOT) {
      if(ord->targ1 == -1) {
	percept->GetPos(ord->targ2, xt, yt, zt);
	xt = xt * 2 + 1;
	yt = yt * 2 + 1;
	ztf = zt * CELL_HEIGHT + OFF_BASE;
	ang = atan2f(yt - yo, xt - xo);
	}
      glColor4f(1.0, 0.0, 0.0, 0.25);
      glBegin(GL_QUADS);
      glVertex3f(xo + cos(ang + M_PI/2)/2, yo + sin(ang + M_PI/2)/2, zof);
      glVertex3f(xt + cos(ang + M_PI/2)/2, yt + sin(ang + M_PI/2)/2, ztf);
      glVertex3f(xt + cos(ang - M_PI/2)/2, yt + sin(ang - M_PI/2)/2, ztf);
      glVertex3f(xo + cos(ang - M_PI/2)/2, yo + sin(ang - M_PI/2)/2, zof);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex3f(xt + cos(ang + M_PI/2), yt + sin(ang + M_PI/2), ztf);
      glVertex3f(xt + cos(ang), yt + sin(ang), ztf);
      glVertex3f(xt + cos(ang - M_PI/2), yt + sin(ang - M_PI/2), ztf);
      glEnd();
      }
    }
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  }

void World::DrawSelBox(int sel_x, int sel_y, int sel_z, float r, float g, float b) {
        //FIXME: Use REAL map x and y size for limits
  if(sel_x < 0 || sel_y < 0 || sel_x >= 64 || sel_y >= 64) return;

//  fprintf(stderr, "Selbox drawing at %d,%d\n", sel_x, sel_y);

  glDisable(GL_LIGHTING);
  glPushMatrix();

  glTranslatef(sel_x*2.0+1.0, sel_y*2.0+1.0, sel_z*CELL_HEIGHT);

  glBindTexture(GL_TEXTURE_2D, 0);

  glColor3f(r, g, b);

  glBegin(GL_LINES);

  glVertex3f(-1.0, -1.0, OFF_BASE);
  glVertex3f(-1.0,  1.0, OFF_BASE);

  glVertex3f(-1.0,  1.0, OFF_BASE);
  glVertex3f( 1.0,  1.0, OFF_BASE);

  glVertex3f( 1.0,  1.0, OFF_BASE);
  glVertex3f( 1.0, -1.0, OFF_BASE);

  glVertex3f( 1.0, -1.0, OFF_BASE);
  glVertex3f(-1.0, -1.0, OFF_BASE);

  glVertex3f(-1.0, -1.0, CELL_HEIGHT);
  glVertex3f(-1.0,  1.0, CELL_HEIGHT);

  glVertex3f(-1.0,  1.0, CELL_HEIGHT);
  glVertex3f( 1.0,  1.0, CELL_HEIGHT);

  glVertex3f( 1.0,  1.0, CELL_HEIGHT);
  glVertex3f( 1.0, -1.0, CELL_HEIGHT);

  glVertex3f( 1.0, -1.0, CELL_HEIGHT);
  glVertex3f(-1.0, -1.0, CELL_HEIGHT);

  glVertex3f(-1.0, -1.0, OFF_BASE);
  glVertex3f(-1.0, -1.0, CELL_HEIGHT);

  glVertex3f(-1.0,  1.0, OFF_BASE);
  glVertex3f(-1.0,  1.0, CELL_HEIGHT);

  glVertex3f( 1.0,  1.0, OFF_BASE);
  glVertex3f( 1.0,  1.0, CELL_HEIGHT);

  glVertex3f( 1.0, -1.0, OFF_BASE);
  glVertex3f( 1.0, -1.0, CELL_HEIGHT);

  glEnd();

  glPopMatrix();

  if(sel_z > 0) {	//Recursive!
    DrawSelBox(sel_x, sel_y, sel_z-1, 0.0, 0.0, 0.0);
    }
  }
