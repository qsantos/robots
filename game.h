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

static const u8 MAGIC_WORD     = 0x42;
static const u8 VERSION_NUMBER = 0x01;
static const u8 START_MESSAGE  = 0x42;

typedef enum
{
	O_ADVANCE = 0x01,
	O_TURN    = 0x02,
	O_TURNGUN = 0x03,
	O_FIRE    = 0x04,
} OrderCode;

typedef enum
{
	E_TICK     = 0x00,
	E_DUMP     = 0x01,
	E_SPOT     = 0x02,
	E_BULLET   = 0x03,
	E_HIT      = 0x04,
	E_HITBY    = 0x05,
	E_HITROBOT = 0x06,
	E_HITWALL  = 0x07,
} EventCode;

typedef struct PACKED
{
	OrderCode code;
	float     param;
} Order;

typedef struct PACKED
{
	float height;
	float width;
	u32   n_slots;
	u32   n_clients;
} Game;

typedef struct PACKED
{
	u32   id;
	float x;
	float y;
	float width;
	float height;
	float angle;
	float gunAngle;
	float energy;
	float velocity;
	float turnSpeed;
	float turnGunSpeed;
} Robot;

typedef struct PACKED
{
	u32   from;
	float x;
	float y;
	float angle;
	float energy;
} Bullet;

bool GameContainsPoint(Game*, float, float);
bool GameContainsRobot(Game*, Robot*);
bool RobotCollidePoint(Robot*, float, float);
bool RobotCollideRobot(Robot*, Robot*);

#endif
