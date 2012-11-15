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

#include "../game.h"

// This file intends to provide an obvious interface for the robot communication
// You may create a very simple robot by just calling 'autoClient' in your 'main'
// To get a slightly more advanced robot, register your own callbacks
// To do so, define a function with the given header (e.g. 'void start(Robot*)')
// and change the callback in the 'main' (e.g. 'cb_Start = start;')

// Note: prefer putting starting code in the 'cb_Start' callback rather than in
// 'main'

// information about the game and the AI's robot
extern Game       game;
extern Robot      robot;
extern RobotOrder robotOrder;

// Use them to send orders to your robot
void Order_Advance     (float);
void Order_Turn        (float);
void Order_TurnGun     (float);
void Order_Fire        (float);
void Order_SetVelocity (float);
void Order_SetTurnSpeed(float);
void Order_SetGunSpeed (float);

// these waits for the order to be fully executed before returning
// NOTE: they should be used in threaded functions
void Order_BlockAdvance(float);
void Order_BlockTurn   (float);
void Order_BlockTurnGun(float);

// Set these to your callback functions
//     return value
//     |      callback name
//     |      |            parameters
//     |      |            |
extern void (*cb_Start   )(            );
extern void (*cb_Tick    )(            );
extern void (*cb_Dump    )(            );
extern void (*cb_Robot   )(Robot*      );
extern void (*cb_Bullet  )(Bullet*     );
extern void (*cb_Hit     )(Bullet*, u32);
extern void (*cb_HitBy   )(Bullet*     );
extern void (*cb_HitRobot)(u32         );
extern void (*cb_HitWall )(            );
extern void (*cb_Kaboum  )(Robot*      );

// Call this when you have set your callbacks
int autoClient(int argc, char** argv);

#endif
