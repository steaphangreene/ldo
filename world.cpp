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

  models.push_back(SM_LoadModel("models/players/trooper"));
  models.back()->AttachSubmodel("tag_weapon", weap);

  SimpleModel::AddSourceFile("models");
  models.push_back(SM_LoadModel("grey/tris.md2"));

  models.push_back(SM_LoadModel("models/players/trooper", "blue"));
  models.back()->AttachSubmodel("tag_weapon", weap);
  models.push_back(SM_LoadModel("models/players/trooper", "red"));
  models.back()->AttachSubmodel("tag_weapon", weap);

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

  effectsto = 0;
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
  static Uint32 mround = 0;

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
    multimap<Uint32, Uint32>::const_iterator seen;
    seen = obj->second.seen.at(plnum).begin();
    for(; seen != obj->second.seen.at(plnum).end(); ++seen) {
      if(seen->first <= offset && seen->second > offset) break;
      }
    if(seen != obj->second.seen.at(plnum).end()) {

      if(obj->second.type == EFFECT_FIRE) {
	if(obj->second.which >= (int)(effectsto)
		&& obj->second.which <= (int)(offset)) {
	  for(Uint32 start = obj->second.which;
		(int)(start) < obj->second.which + 20000; start += 10) {
	    SS_Particle part = scene->AddParticle(fire);
	    scene->SetParticlePosition(part,
		obj->first.x*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.y*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.z*2.0);
	    scene->SetParticleTime(part, start);
	    }
	  }
	}
      else if(obj->second.type == EFFECT_FIRE || obj->second.type == EFFECT_SMOKE) {
	if(obj->second.which >= (int)(effectsto)
		&& obj->second.which <= (int)(offset)) {
	  for(Uint32 start = obj->second.which;
		(int)(start) < obj->second.which + 20000; start += 250) {
	    SS_Particle part = scene->AddParticle(smoke);
	    scene->SetParticlePosition(part,
		obj->first.x*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.y*2.0 + 1.0 + float(rand()) / RAND_MAX / 2.0,
		obj->first.z*2.0);
	    scene->SetParticleTime(part, start);
	    }
	  }
	}
      }
    if(obj->second.type != EFFECT_FIRE && obj->second.type != EFFECT_SMOKE) {
      SS_Model mod;
      if(modmap.count(obj->second.which) > 0) {
	mod = modmap[obj->second.which];
	}
      else {
	mod = modmap[-(obj->second.type)];
	}

      if(mod == modmap[0x0000]) {		//Items that aren't drawn
	continue;	//Don't draw it
	}

      SS_Object sobj;
      if(objmap.count(obj->second.id) < 1) {
	sobj = scene->AddObject(mod);
	objmap[obj->second.id] = sobj;
	scene->SetObjectPosition(sobj, obj->first.x*2.0+1.0,
	  obj->first.y*2.0+1.0, obj->first.z*CELL_HEIGHT);

	if(texmap.count(obj->second.which) > 0) {
	  scene->SetObjectSkin(sobj, texmap[obj->second.which]);
	  }
	else {
	  scene->SetObjectColor(sobj,
		obj->first.x/float(50), 0.5, obj->first.y/float(50));
	  scene->SetObjectSkin(sobj, obj->second.which & 0xFF);
	  }
	}
      else {
	sobj = objmap[obj->second.id];
	}
      if(mround != percept->round) {
	multimap<Uint32, Uint32>::const_iterator act;
	act = obj->second.seen.at(plnum).begin();
	for(; act != obj->second.seen.at(plnum).end(); ++act) {
//	  if(act->second > (percept->round - 2)*3000) {
	  Uint32 base = act->first;
	  Uint32 end = act->second;
	  scene->ObjectAct(sobj, SS_ACT_VISIBLE, end, end-base);
	  }
	}
      }
    }

  mround = percept->round;
  effectsto = offset + 1;
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
    vector<UnitAct>::const_reverse_iterator act = unitacts->second.rbegin();
    while(act != unitacts->second.rend()
	&& act->finish > (offset + act->duration)) {
      ++act;
      }
    unitact.insert(pair<int,UnitAct>(unitacts->first, *act));
    }
  unitacts = percept->other_units.begin();
  for(; unitacts != percept->other_units.end(); ++unitacts) {
    unitact.erase(unitacts->first);
    vector<UnitAct>::const_reverse_iterator act = unitacts->second.rbegin();
    while(act != unitacts->second.rend()
	&& act->finish > (offset + act->duration)) {
      ++act;
      }
    unitact.insert(pair<int,UnitAct>(unitacts->first, *act));
    }

  map<int, UnitAct>::const_iterator mapact = unitact.begin();
  for(; mapact != unitact.end(); ++mapact) {
    const UnitAct *act = &(mapact->second);
    int mod = cur_game->PlayerForUnit(act->id)->Color();
    if(act->finish <= (offset + act->duration)) {
//      fprintf(stderr, "Doing %d on %d, since %d <= (%d + %d)\n",
//	act->act, act->id, act->finish, offset, act->duration);
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
      times[0] = act->finish - act->duration;
      times[1] = act->finish - act->duration;
      float x = act->x * 2 + 1;
      float y = act->y * 2 + 1;
      float z = azh;
      float a = act->angle;
      if(act->act == ACT_STAND) {
	if(act->finish <= offset + act->duration) {
	  x = act->targ1 * 2 + 1;
	  y = act->targ2 * 2 + 1;
	  z = tzh;
	  }
	}
      else if(act->act == ACT_FALL) {
	if(act->finish + 1000 <= offset + act->duration) {
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
      else if(act->act == ACT_MOVE || act->act == ACT_RUN) {
	int dx = act->x - act->targ1;
	int dy = act->y - act->targ2;
	float dur = act->duration;
	if(act->finish <= offset) {
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	else {
	  Uint32 off = offset + act->duration - act->finish;
	  int anim;
	  if(act->act == ACT_MOVE) {
	    anim = models[mod]->LookUpAnimation("LEGS_WALK");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("WALK");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("RUN");
	    }
	  else {
	    anim = models[mod]->LookUpAnimation("LEGS_RUN");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("RUN");
	    if(anim < 0) anim = models[mod]->LookUpAnimation("WALK");
	    }
	  anims[0] = anim;
	  x = (act->targ1 * 2 + 1) * (dur - off) + (act->x * 2 + 1) * off;
	  y = (act->targ2 * 2 + 1) * (dur - off) + (act->y * 2 + 1) * off;
	  z = tzh * (dur - off) + azh * off;
	  x /= dur; y /= dur; z /= dur;
	  a = 180.0 * atan2f(dy, dx) / M_PI;
	  }
	}
      else if(act->act == ACT_SHOOT) {
	int dx = act->targ1 - act->x;
	int dy = act->targ2 - act->y;
	//int dz = act->targ3 - act->z;
	a = 180.0 * atan2f(dy, dx) / M_PI;
	if(act->finish + 1500 <= offset + act->duration) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
	  times[1] += 1500;
	  }
	else {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_ATTACK");
	  }
	}
      else if(act->act == ACT_EQUIP && act->finish > 0) { // First EQUIP Free
	if(act->finish + 1500 <= offset + act->duration) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_STAND");
	  times[1] += 1500;
	  }
	else if(act->finish + 1000 <= offset + act->duration) {
	  anims[1] = models[mod]->LookUpAnimation("TORSO_RAISE");
	  times[1] += 1000;
	  }
	else if(act->finish + 500 <= offset + act->duration) {
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
//      fprintf(stderr, "Not Doing %d on %d, since %d > (%d + %d)\n",
//	act->act, act->id, act->finish, offset, act->duration);
      }
    }
  }

void World::Render(Uint32 offset) {	// Render for playback
  DrawMap(offset);
  DrawModels(offset);
  if(offset == (percept->round - 1) * 3000) DrawOrders(offset);
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
