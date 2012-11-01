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

#ifndef CLIENT_H
#define CLIENT_H

#include "game.h"

// This file intends to provide an obvious interface for the robot communication
// You may create a very simple robot by just calling 'autoClient' in your 'main'
// To get a slightly more advanced robot, register your own callbacks
// To do so, define a function with the given header (e.g. 'void start(Robot*)')
// and change the callback in the 'main' (e.g. 'cb_Start = start;')

// Note: prefer putting starting code in the 'cb_Start' callback rather than in
// 'main'

// information about the game
extern Game game;

// Use them to send orders to your robot
void Order_Advance(float);
void Order_Turn   (float);
void Order_TurnGun(float);
void Order_Fire   (float);

// Set these to your callback functions
//     return value
//     |      callback name
//     |      |            parameters
//     |      |            |
extern void (*cb_Start   )(Robot*              );
extern void (*cb_Tick    )(Robot*              );
extern void (*cb_Dump    )(Robot*              );
extern void (*cb_Robot   )(Robot*, Robot*      );
extern void (*cb_Bullet  )(Robot*, Bullet*     );
extern void (*cb_Hit     )(Robot*, Bullet*, u32);
extern void (*cb_HitBy   )(Robot*, Bullet*     );
extern void (*cb_HitRobot)(Robot*, u32         );
extern void (*cb_HitWall )(Robot*              );
extern void (*cb_Kaboum  )(Robot*, Robot*      );

// Call this when you have set your callbacks
int autoClient(int argc, char** argv);

#endif
