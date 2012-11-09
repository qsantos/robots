/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Quentin SANTOS
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

#ifndef MATH_H
#define MATH_H

#include <stdlib.h>

#include "game.h"

/* RANDOM */

// return a float between a and b
inline float randf(float a, float b)
{
	return a + ((float) random()) * (b-a) / RAND_MAX;
}

/* TRIGONOMETRY */

#define PI (3.14159265358979323846)

// positive float mod
inline float pfmod(float x, float m)
{
	return m ? x - m*floor(x/m) : x;
}
// centered float mod
inline float cfmod(float x, float m)
{
	return pfmod(x+m/2, m)-m/2;
}

// maps angle to [-PI, PI[
inline float normRad(float a)
{
	return cfmod(a, 2*PI);
}
// maps angle to [-180, 180[
inline float normDeg(float a)
{
	return cfmod(a, 360);
}

// angle conversion
inline float deg2rad(float a)
{
	return normRad(a * PI / 180.0);
}
inline float rad2deg(float a)
{
	return normDeg(a * 180.0 / PI);
}

/* PHYSICS */

bool GameContainsPoint(Game*, float, float);
bool GameContainsRobot(Game*, Robot*);
bool RobotCollidePoint(Robot*, float, float);
bool RobotCollideRobot(Robot*, Robot*);

float distance(float, float, float, float);
float angle   (float, float, float, float);

inline float distanceRobots(Robot* a, Robot* b)
{
	return distance(a->x, a->y, b->x, b->y);
}
inline float angleRobots(Robot* a, Robot* b)
{
	return angle(a->x, a->y, b->x, b->y);
}
inline float distanceBullet(Robot* r, Bullet* b)
{
	return distance(r->x, r->y, b->x, b->y);
}
inline float angleBullet(Robot* r, Bullet* b)
{
	return angle(r->x, r->y, b->x, b->y);
}

#endif
