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

#ifndef UNIT_H
#define UNIT_H

enum Attrib {	//List of Attributes
  ATTRIB_B = 0,		// Body
  ATTRIB_Q,		// Quickness
  ATTRIB_S,		// Strength
  ATTRIB_C,		// Charisma
  ATTRIB_I,		// Intelligence
  ATTRIB_W,		// Willpower
  ATTRIB_MAX
  };

enum Skill {	//List of Skills
  SKILL_RIFLE = 0,
  SKILL_PISTOL,
  SKILL_SMG,
  SKILL_ASSAULT,	// Assault Rifles
  SKILL_THROW,
  SKILL_HEAVY,		// Heavy Weapons
  SKILL_LEADERSHIP,
  SKILL_PERCEPTION,
  SKILL_FIRSTAID,
  SKILL_STEALTH,
  SKILL_MAX,
  SKILL_COMBAT_MAX = SKILL_LEADERSHIP
  };

enum Item {	//List of Items
  ITEM_NONE = -1,
  ITEM_LIGHTA,		// Light Armor
  ITEM_MEDIUMA,		// Medium Armor
  ITEM_HEAVYA,		// Heavy Armor
  ITEM_PISTOL,
  ITEM_SMG,
  ITEM_RIFLE,
  ITEM_ASSAULT,		// Assault Rifle
  ITEM_HEAVYW,		// Heavy Weapon
  ITEM_GRENADE,
  ITEM_MEDKIT,
  ITEM_STREASURE,	// Small Treasure
  ITEM_TREASURE,	// Treasure
  ITEM_LTREASURE,	// Large Treasure
  ITEM_MAX,
  ITEM_ARMOR_MAX = ITEM_PISTOL,
  ITEM_WEAPON_MAX = ITEM_GRENADE
  };

enum Locations {// List of Locations on Unit for Equipment
  LOC_NONE = -1,
  LOC_RSHOULD,		// Right Shoulder
  LOC_LSHOULD,		// Left Shoulder
  LOC_RHAND,		// Right Hand
  LOC_LHAND,		// Left Hand
  LOC_RLEG,		// Right Leg
  LOC_LLEG,		// Left Leg
  LOC_BELT,
  LOC_PACK,		// Backpack
  LOC_MAX
  };

#include <cstdio>
#include <string>
#include <vector>
using namespace std;

class Unit {
public:
  Unit();
  int Load(FILE *f, unsigned int ver);
  int Save(FILE *f);

  int id;
  string name;

  char attribs[ATTRIB_MAX];
  char skills[SKILL_MAX];
  vector<Item> items[LOC_MAX];
  };

#endif // UNIT_H
