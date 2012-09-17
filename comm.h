/*
 *   robots
 *   Copyright (C) 2012  Thomas GREGOIRE, Quentin SANTOS
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
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

typedef struct __attribute__((packed))
{
  double x;
  double y;
  double angle;
  double energy;
} Robot;

void   File_SendRobot(FILE*, Robot*);
Robot* File_GetRobot (FILE*);

typedef struct __attribute__((packed))
{
  u32    n_robots;
  Robot* robot;
} State;

void   File_SendState(FILE*, State*);
State* File_GetState (FILE*);

typedef enum
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    FIRE,
} Command_Type;

typedef struct __attribute__((packed))
{
  Command_Type type;
  double       amount;
} Command;

void     File_SendCommand(FILE*, Command*);
Command* File_GetCommand (FILE*);

#endif
