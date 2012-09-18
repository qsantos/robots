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

#include "comm.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>

State* State_New(u32 n_clients)
{
  State* ret = ALLOC(State, 1);
  
  ret->fd        = ALLOC(int,   n_clients);
  ret->fh        = ALLOC(FILE*, n_clients);
  ret->n_robots  = n_clients;
  ret->robot     = ALLOC(Robot, n_clients);
  ret->n_bullets = 0;
  ret->bullet    = NULL;
  
  return ret;
}

void State_Delete(State* s)
{
  assert(s);
  free(s->bullet);
  free(s->robot);
  free(s->fh);
  free(s->fd);
  free(s);
}

void State_Send(FILE* f, State* s)
{
  assert(s);
  fwrite(s,         sizeof(u32),    2,           f);
  fwrite(s->robot,  sizeof(Robot),  s->n_robots, f);
  fwrite(s->bullet, sizeof(Bullet), s->n_bullets, f);
}

State* State_Get(FILE* f)
{
  State* s = ALLOC(State, 1);
  fread(s, sizeof(u32), 2, f);
  s->robot = ALLOC(Robot, s->n_robots);
  fread(s->robot, sizeof(Robot), s->n_robots, f);
  s->bullet = ALLOC(Bullet, s->n_bullets);
  fread(s->bullet, sizeof(Bullet), s->n_bullets, f);
  return s;
}

void State_Update(State* s, u32 id, Command c)
{
  assert(id < s->n_robots);

  switch (c.type)
  {
  case FORWARD:
    s->robot[id].x += c.amount * cos(s->robot[id].angle);
    s->robot[id].y += c.amount * sin(s->robot[id].angle);
    break;

  case ROTATE:
    s->robot[id].angle += Angle_ToRad(c.amount);
    break;

  case FIRE:
    s->n_bullets++;
    s->bullet = REALLOC(s->bullet, Bullet, s->n_bullets);
    
    s->bullet[s->n_bullets - 1].x  = s->robot[id].x;
    s->bullet[s->n_bullets - 1].y  = s->robot[id].y;
    s->bullet[s->n_bullets - 1].angle = Angle_ToRad(c.amount);
    s->bullet[s->n_bullets - 1].energy = c.amount;
    
    break;
  }
}

void State_Debug(State* s)
{
  assert(s);
  printf("State information\n-----------------\n\n");
  printf("# robots : %lu\n\n", s->n_robots);
  printf("# bullets : %lu\n\n", s->n_bullets);

  for (u32 i = 0; i < s->n_robots; i++)
  {
    printf(" > Robot #%lu\n", i);
    printf("    x      %f\n", s->robot[i].x);
    printf("    y      %f\n", s->robot[i].y);
    printf("    angle  %f\n", s->robot[i].angle);
    printf("    energy %f\n", s->robot[i].energy);
    putchar('\n');
  }

  for (u32 i = 0; i < s->n_bullets; i++)
  {
    printf(" > Bullet #%lu\n", i);
    printf("    x      %f\n", s->bullet[i].x);
    printf("    y      %f\n", s->bullet[i].y);
    printf("    angle  %f\n", s->bullet[i].angle);
    printf("    energy %f\n", s->bullet[i].energy);
    putchar('\n');
  }
}

void Command_Send(FILE* f, Command c)
{
  fwrite(&c, sizeof(Command), 1, f);
}

Command Command_Get(FILE* f)
{
  Command c;
  fread(&c, sizeof(Command), 1, f);
  return c;
}
