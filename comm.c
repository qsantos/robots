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

#include "comm.h"

#include <assert.h>

void File_SendRobot(FILE* f, Robot* r)
{
  assert(r);
  fwrite(r, sizeof(Robot), 1, f);
}

Robot* File_GetRobot(FILE* f)
{
  Robot* r = ALLOC(Robot, 1);
  fread(r, sizeof(Robot), 1, f);
  return r;
}

void File_SendState(FILE* f, State* s)
{
  assert(s);
  fwrite(&s->n_robots, sizeof(u32),   1,           f);
  fwrite(s->robot,     sizeof(Robot), s->n_robots, f);
}

State* File_GetState(FILE* f)
{
  State* s = ALLOC(State, 1);
  fread(&s->n_robots, sizeof(u32), 1, f);
  s->robot = ALLOC(Robot, s->n_robots);
  fread(s->robot, sizeof(Robot), s->n_robots, f);
  return s;
}

void File_SendCommand(FILE* f, Command* cmd)
{
  assert(cmd);
  fwrite(cmd, sizeof(Command), 1, f);
}

Command* File_GetCommand(FILE* f)
{
  Command* cmd = ALLOC(Command, 1);
  fread(cmd, sizeof(Command), 1, f);
  return cmd;
}
