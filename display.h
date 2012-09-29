/*\
 *  This is an awesome programm simulating awesome batlles of awesome robot tanks
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "game.h"

typedef struct
{
  s32     server;
  bool    opened;

  Game    game;
  u32     n_robots;
  u32     a_robots;
  Robot*  robot;
//  u32     n_bullets;
//  Bullet* bullet:
} Display;

Display* Display_New   (string, u16);
void     Display_Delete(Display*);
void     Display_Update(Display*);
void     Display_Draw  (Display*);
void     Robot_Draw    (Robot*);
void     Texture_Draw  (u32);

#endif
