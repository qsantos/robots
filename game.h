/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Thomas GREGOIRE, Quentin SANTOS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*/

#ifndef GAME_H
#define GAME_H

#include "common.h"

#define FRAMERATE 60

typedef enum
{
  ADVANCE = 1,
  TURN    = 2,
  TURNGUN = 3,
  FIRE    = 4,
} Command_Type;

typedef struct PACKED
{
  float height;
  float width;
  u32   n_slots;
  u32   n_clients;
} Game;

typedef struct PACKED
{
  float x;
  float y;
  float angle;
  float gunAngle;
  float energy;
  float velocity;
  float turnSpeed;
  float turnGunSpeed;
} Robot;

/*
typedef struct PACKED
{
  float x;
  float y;
  float angle;
  float energy;
} Bullet;
*/

#endif
