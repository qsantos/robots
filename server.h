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

#ifndef SERVER_H
#define SERVER_C

#include <stdio.h>

#include "game.h"

typedef struct
{
  s32     listener;
  FILE*   display;
  FILE**  fh;
  int*    fd;
  
  Game game;
  
  u32     n_robots;
  Robot*  robot;
  
//  u32     n_bullets;
//  Bullet* bullet;
} Server;

Server* Server_New          (string, u16, u32);
void    Server_Delete       (Server*);
void    Server_Debug        (Server*);
void    Server_AcceptDisplay(Server*);
void    Server_AcceptClients(Server*);
void    Server_HandleOrder  (Server*, u32);
void    Server_Tick         (Server*);
void    Server_Dump         (Server*, FILE*);
void    Server_Loop         (Server*);

#endif
