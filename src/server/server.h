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

#ifndef SERVER_H
#define SERVER_C

#include "../game.h"
#include "../allocateArray.h"

typedef struct
{
	int     listener;
	int     display;
	int*    clients;

	Game    game;

	// because items are added or removed simultaneously on 'robots' and 'robotOrders', they should use the same array cells
	DEF(Robot,      robots)
	DEF(RobotOrder, robotOrders)
	DEF(Bullet,     bullets)
} Server;

Server* Server_New          (int, u32);
void    Server_Delete       (Server*);
void    Server_Debug        (Server*);
bool    Server_HandleOrder  (Server*, u32);
void    Server_Tick         (Server*, float);
void    Server_Dump         (Server*);
void    Server_Loop         (Server*);

#endif
