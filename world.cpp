// *************************************************************************
//  This file is part of Life, Death, and the Objective ("LDO")
//  a simple squad-tactics strategy game by Steaphan Greene
//
//  Copyright 2005-2008 Steaphan Greene <stea@cs.binghamton.edu>
//
//  LDO is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  LDO is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with LDO (see the file named "COPYING");
//  If not, see <http://www.gnu.org/licenses/>.
//  
// *************************************************************************

#define OFF_BASE 0.0625

#include <cmath>
using namespace std;

#include "world.h"
#include "game.h"

extern Game *cur_game;
extern int cur_zpos;

World::World(Percept *per, Orders *ord, int pl) {
  percept = per;
  orders = ord;
  plnum = pl;
  angle = 45;
  pointx = 0.0;
  pointy = 0.0;

  scene = SimpleScene::Current(); //Yes, this is ok, it's static!
  scene->Clear();

  SimpleModel *weap = SM_LoadModel("models/weapons2/machinegun/machinegun.md3");

  SimpleModel *mod = SM_LoadModel("models/players/trooper");
  mod->AttachSubmodel("tag_weapon", weap);
  int ssmod = scene->AddModel(mod);
  scene->SetModelAnim(ssmod, ACT_SHOOT, "TORSO_ATTACK", 1);
  scene->SetModelAnim(ssmod, ACT_EQUIP, "TORSO_DROP", 1);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 0);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 1);

  SimpleModel::AddSourceFile("models");
  ssmod = scene->AddModel(SM_LoadModel("grey/tris.md2"));
  scene->SetModelAnim(ssmod, ACT_SHOOT, "ATTACK");
  scene->SetModelAnim(ssmod, ACT_FALL, "DEATH3");

  mod = SM_LoadModel("models/players/trooper", "blue");
  mod->AttachSubmodel("tag_weapon", weap);
  ssmod = scene->AddModel(mod);
  scene->SetModelAnim(ssmod, ACT_SHOOT, "TORSO_ATTACK", 1);
  scene->SetModelAnim(ssmod, ACT_EQUIP, "TORSO_DROP", 0);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 0);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 1);

  mod = SM_LoadModel("models/players/trooper", "red");
  mod->AttachSubmodel("tag_weapon", weap);
  ssmod = scene->AddModel(mod);
  scene->SetModelAnim(ssmod, ACT_SHOOT, "TORSO_ATTACK", 1);
  scene->SetModelAnim(ssmod, ACT_EQUIP, "TORSO_DROP", 1);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 0);
  scene->SetModelAnim(ssmod, ACT_FALL, "BOTH_DEATH1", 1);

  //For Items that aren't drawn
  modmap[0x0000] = (SS_Model)(-1);

  modmap[-GROUND_FLOOR] = scene->AddModel(SM_LoadModel("models/floor.obj"));

  modmap[-OBJECT_MISC] = scene->AddModel(SM_LoadModel("models/object.obj"));

  modmap[-WALL_NORTH] =
	scene->AddModel(SM_LoadModel("models/nwall_thin_high.obj"));

  modmap[-WALL_WEST] =
	scene->AddModel(SM_LoadModel("models/wwall_thin_high.obj"));

  modmap[0x11D] = scene->AddModel(SM_LoadModel("models/nfence1.obj"));
  modmap[0x119] = scene->AddModel(SM_LoadModel("models/wfence1.obj"));

  modmap[0x11E] = scene->AddModel(SM_LoadModel("models/nfence2.obj"));
  modmap[0x11A] = scene->AddModel(SM_LoadModel("models/wfence2.obj"));

  modmap[0x111] = scene->AddModel(SM_LoadModel("models/nwall_thick_low.obj"));
  modmap[0x115] = modmap[0x111];
  modmap[0x110] = scene->AddModel(SM_LoadModel("models/wwall_thick_low.obj"));
  modmap[0x114] = modmap[0x110];

  modmap[0x113] = scene->AddModel(SM_LoadModel("models/nwall_thick_high.obj"));
  modmap[0x112] = scene->AddModel(SM_LoadModel("models/wwall_thick_high.obj"));

  modmap[0x182] = scene->AddModel(SM_LoadModel("models/nramp_high.obj"));
  modmap[0x183] = scene->AddModel(SM_LoadModel("models/nramp_med.obj"));
  modmap[0x184] = scene->AddModel(SM_LoadModel("models/nramp_low.obj"));

  //Items that aren't drawn
  modmap[0x120] = modmap[0x0000];	//Broken Fence

  for(int n=0; n < 0x0100; ++n) {
    char name[64];
    sprintf(name, "graphics/util/test%.2X.png%c", n, 0);
    texmap[n] = scene->AddSkin(new SimpleTexture(name));
    }

  texmap[0x146] = scene->AddSkin(new SimpleTexture("graphics/wall.png"));
  texmap[0x182] = texmap[0x146];
  texmap[0x183] = texmap[0x146];
  texmap[0x184] = texmap[0x146];

  texmap[0x119] = scene->AddSkin(new SimpleTexture("models/wood.png"));
  texmap[0x11A] = texmap[0x119];
  texmap[0x11D] = texmap[0x119];
  texmap[0x11E] = texmap[0x119];

  texmap[0x110] = scene->AddSkin(new SimpleTexture("models/stone.png"));
  texmap[0x111] = texmap[0x110];

  texmap[0x112] = scene->AddSkin(new SimpleTexture("models/hedge.png"));
  texmap[0x113] = texmap[0x112];
  texmap[0x114] = texmap[0x112];
  texmap[0x115] = texmap[0x112];

  texmap[0x102] = scene->AddSkin(new SimpleTexture("models/grass.png"));

  texmap[0x104] = scene->AddSkin(new SimpleTexture("models/dirt.png"));

  texmap[0x105] = scene->AddSkin(new SimpleTexture("models/reaped.png"));

  texmap[0x106] = scene->AddSkin(new SimpleTexture("models/wheat.png"));

  texmap[0x109] = scene->AddSkin(new SimpleTexture("models/cabbage.png"));

  texmap[0x12C] = scene->AddSkin(new SimpleTexture("models/boards.png"));
  texmap[0x12D] = texmap[0x12C];

  //FIXME: Window LOS!
  texmap[0x12E] = scene->AddSkin(new SimpleTexture("models/boards_window.png"));
  texmap[0x12F] = texmap[0x12E];

  //FIXME: Door Open/Close!
  texmap[0x138] = scene->AddSkin(new SimpleTexture("models/door.png"));
  texmap[0x139] = texmap[0x138];

  texmap[0x131] = scene->AddSkin(new SimpleTexture("models/brick.png"));
  texmap[0x132] = texmap[0x131];

  texmap[0x13A] = scene->AddSkin(new SimpleTexture("models/pavers.png"));

  texmap[0x129] = scene->AddSkin(new SimpleTexture("models/metalroof.png"));

  //Special Effects Resources
  SimpleTexture *smoke_tex = new SimpleTexture("graphics/smoke.png");
  scene->AddSkin(smoke_tex);

  smoke = scene->AddPType();
  scene->SetPTypeTexture(smoke, smoke_tex);
  scene->SetPTypeColor0(smoke, 0.8, 0.8, 0.8, 1.0);
  scene->SetPTypeColor1(smoke, 0.2, 0.2, 0.2, 0.0);
  scene->SetPTypeVelocity(smoke, 0.0, 0.0, 1.0);
  scene->SetPTypeDuration(smoke, 10000);
  scene->SetPTypeSize0(smoke, 0.5);
  scene->SetPTypeSize1(smoke, 3.0);

  fire = scene->AddPType();
  scene->SetPTypeTexture(fire, smoke_tex);
  scene->SetPTypeColor0(fire, 1.0, 0.7, 0.2, 1.0);
  scene->SetPTypeColor1(fire, 1.0, 0.2, 0.0, 1.0);
  scene->SetPTypeVelocity(fire, 0.0, 0.0, 10.0);
  scene->SetPTypeDuration(fire, 250);
  scene->SetPTypeSize0(fire, 1.0);
  scene->SetPTypeSize1(fire, 0.5);

  mround = 0;
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

void World::DrawMap(Uint32 offset) {
  map<MapCoord, MapObject>::const_iterator obj = percept->objects.begin();
  for(; obj != percept->objects.end(); ++obj) {
    multimap<Uint32, Uint32>::const_iterator seen;
    seen = obj->second.seen.at(plnum).begin();
    for(; seen != obj->second.seen.at(plnum).end(); ++seen) {
      if(seen->first <= offset && seen->second > offset) break;
      }
    SS_Model mod;
    if(modmap.count(obj->second.which) > 0) {
      mod = modmap[obj->second.which];
      }
    else {
      mod = modmap[-(obj->second.type)];
      }

    if(mod == modmap[0x0000]) {		//Items that aren't drawn
      continue;		//Don't draw it
      }

    SS_Object sobj;
    if(objmap.count(obj->second.id) < 1) {
      sobj = scene->AddObject(mod);
      objmap[obj->second.id] = sobj;
      scene->MoveObject(sobj, obj->first.x*2.0+1.0,
	obj->first.y*2.0+1.0, obj->first.z*CELL_HEIGHT);
      scene->ColorObject(sobj, 0.5, 0.5, 0.5, 0);	//Initially Unseen

      if(texmap.count(obj->second.which) > 0) {
	scene->SkinObject(sobj, texmap[obj->second.which]);
	}
      else {
	scene->SkinObject(sobj, obj->second.which & 0xFF);
	}
      }
    else {
      sobj = objmap[obj->second.id];
      }
    if(mround != percept->round) {
      multimap<Uint32, Uint32>::const_iterator act, oact;
      act = obj->second.seen.at(plnum).begin();
      oact = act;
      for(; act != obj->second.seen.at(plnum).end(); ++act) {
	if(act->second > (percept->round - 2)*3000) {
	  if(oact != act) {
	    scene->ColorObject(sobj, 0.5, 0.5, 0.5, oact->second);
	    }
	  else {
	    scene->ShowObject(sobj, act->first);
	    }
	  scene->ColorObject(sobj, 1.0, 1.0, 1.0, act->first);
	  }
	oact = act;
	}
      if(oact != act && oact->second < (percept->round - 1) * 3000 + 1) {
	scene->ColorObject(sobj, 0.5, 0.5, 0.5, oact->second);
	}
      Uint8 burn = 0;
      if(!(obj->second.burn.empty())) {	// Check for Smoke and/or Fire
	map<Uint32, Uint8>::const_reverse_iterator bit
		= obj->second.burn.rbegin();
	for(; bit != obj->second.burn.rend(); ++bit) {
	  if(bit->first <= percept->round) {
	    burn = bit->second;
	    break;
	    }
	  }
      if(burn > 0 && obj->second.type == GROUND_FLOOR) { // Smoke and/or Fire
	for(Uint32 start = (percept->round - 2)*3000;
		start < (percept->round - 1)*3000; start += 250) {
	  SS_Particle part = scene->AddParticle(smoke);
	  scene->SetParticlePosition(part,
		obj->first.x*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.y*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.z*2.0);
	  scene->SetParticleTime(part, start);
	  }
	}
      if(burn >= 64)		// Fire
	for(Uint32 start = (percept->round - 2)*3000;
		start < (percept->round - 1)*3000; start += 10) {
	  SS_Particle part = scene->AddParticle(fire);
	  float xo = 0.0, yo = 0.0, zo = 0.0;
	  if(obj->second.type == WALL_NORTH) {
	    yo = -1.0; zo = 1.5;
	    }
	  else if(obj->second.type == WALL_WEST) {
	    xo = -1.0; zo = 1.5;
	    }
	  else if(obj->second.type == OBJECT_MISC) {
	    zo = 0.5;
	    }
	  scene->SetParticlePosition(part,
		xo + obj->first.x*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		yo + obj->first.y*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		zo + obj->first.z*2.0);
	  scene->SetParticleTime(part, start);
	  }
	}
      }
    }
  }

void World::DrawModels(Uint32 offset) {
  if(mround != percept->round) {
    map<int, vector<UnitAct> >::const_iterator unitacts;
    unitacts = percept->my_units.begin();
    for(; unitacts != percept->other_units.end(); ++unitacts) {
      if(unitacts == percept->my_units.end()) {
	unitacts = percept->other_units.begin();
	if(unitacts == percept->other_units.end()) break;
	}
      if(unitmap.count(unitacts->first) < 1) {
	//FIXME: Real Unit Type
	int mod = cur_game->PlayerForUnit(unitacts->first)->Color();
	unitmap[unitacts->first] = scene->AddObject(mod);
	}
      vector<UnitAct>::const_iterator act = unitacts->second.begin();
      for(; act != unitacts->second.end(); ++act) {
	if(act->act == ACT_MOVE || act->act == ACT_RUN || act->act == ACT_START) {
	  scene->MoveObject(unitmap[unitacts->first],
		act->x * 2 + 1, act->y * 2 + 1, act->z * CELL_HEIGHT,
		act->finish, act->duration
		);
	  scene->TurnObject(unitmap[unitacts->first], act->angle,
		act->finish - act->duration / 2, act->duration / 2
		);
	  }
	else if(act->act == ACT_SHOOT) {
	  scene->TurnObject(unitmap[unitacts->first], act->angle,
		act->finish - act->duration, 250
		);
	  scene->TargetObject(unitmap[unitacts->first],
		act->targ1*2 + 1, act->targ2*2 + 1, act->targ3 * CELL_HEIGHT,
		act->finish - act->duration, 250
		);
	  scene->ActObject(unitmap[unitacts->first], ACT_SHOOT,
		act->finish, act->duration
		);
	  scene->UnTargetObject(unitmap[unitacts->first],
		act->finish + 250, 250
		);
	  }
	else if(act->act == ACT_EQUIP) {
	  if(act->finish > 0) { // Initial EQUIP is NOT Shown
	    scene->ActObject(unitmap[unitacts->first], act->act,
		act->finish, act->duration
		);
	    }
	  }
	else {
	  scene->ActObject(unitmap[unitacts->first], act->act,
		act->finish, act->duration
		);
	  }
	}
      }
    }
  }

void World::Render(Uint32 offset) {	// Render for playback
  DrawMap(offset);
  DrawModels(offset);
  if(offset == (percept->round - 1) * 3000) DrawOrders(offset);
  mround = percept->round;
  }

void World::DrawOrders(Uint32 offset) {
  int xo, yo, zo, xt, yt, zt;
  float zof, ztf;
  map<UnitOrder, bool>::const_iterator ordit = orders->orders.begin();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glBindTexture(GL_TEXTURE_2D, 0);
//FIXME: Better Order Graphics!
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//  glEnable(GL_BLEND);
  for(; ordit != orders->orders.end(); ++ordit) {
    const UnitOrder *ord = &(ordit->first);
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
