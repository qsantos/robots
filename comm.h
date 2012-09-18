/*
 *   robots
 *   Copyright (C) 2012  Thomas GREGOIRE, Quentin SANTOS
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Delete Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMM_H
#define COMM_H

#include <stdio.h>

#include "utils.h"

typedef enum
{
  FORWARD,
  BACKWARD,
  ROTATE,
  FIRE,
} Command_Type;

typedef struct PACKED
{
  Command_Type type;
  double       amount;
} Command;

typedef struct PACKED
{
  u32      length;
  Command* command;
} Commands;

void      Commands_Delete(Commands*);
void      Commands_Send  (FILE*, u32, ...);
Commands* Commands_Get   (FILE*);

typedef struct PACKED
{
  double x;
  double y;
  double angle;
  double energy;
} Robot;

typedef struct PACKED
{
  double x;
  double y;
  double vx;
  double vy;
} Bullet;

typedef struct PACKED
{
  int*   fd;
  FILE** fh;
  
  u32     n_robots;
  Robot*  robot;
  
  u32     n_bullets;
  Bullet* bullet;
} State;

State* State_New   (u32);
void   State_Delete(State*);
void   State_Send  (FILE*, State*);
State* State_Get   (FILE*);
void   State_Update(State*, u32, Commands*);
void   State_Debug (State*);

#endif
