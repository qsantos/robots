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

void State_Update(State* s, u32 id, Commands* c)
{
  assert(id < s->n_robots);

  for (u32 i = 0; i < c->length; i++)
  {
    switch (c->command[i].type)
    {
    case FORWARD:
      s->robot[id].x += c->command[i].amount * cos(s->robot[id].angle);
      s->robot[id].y += c->command[i].amount * sin(s->robot[id].angle);
      break;

    case BACKWARD:
      s->robot[id].x -= c->command[i].amount * cos(s->robot[id].angle);
      s->robot[id].y -= c->command[i].amount * sin(s->robot[id].angle);
      break;

    case ROTATE:
      s->robot[id].angle += Angle_ToRad(c->command[i].amount);
      break;

    case FIRE:
      s->n_bullets++;
      s->bullet = REALLOC(s->bullet, Bullet, s->n_bullets);

      s->bullet[s->n_bullets - 1].x  = s->robot[id].x;
      s->bullet[s->n_bullets - 1].y  = s->robot[id].y;
      s->bullet[s->n_bullets - 1].angle = Angle_ToRad(c->command[i].amount);
      s->bullet[s->n_bullets - 1].energy = c->command[i].amount;

      break;
    }
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

void Commands_Send(FILE* f, u32 len, ...)
{
  Commands* c = ALLOC(Commands, 1);
  c->length  = len;
  c->command = ALLOC(Command, len);

  va_list vl;
  va_start(vl, len);
  for (u32 i = 0; i < len; i++)
  {
    c->command[i].type   = va_arg(vl, Command_Type);
    c->command[i].amount = va_arg(vl, double);
  }
  va_end(vl);

  fwrite(&len,       sizeof(u32),     1,   f);
  fwrite(c->command, sizeof(Command), len, f);

  free(c);
}

Commands* Commands_Get(FILE* f)
{
  Commands* c = ALLOC(Commands, 1);
  fread(&c->length, sizeof(u32), 1, f);
  c->command = ALLOC(Command, c->length);
  fread(c->command, sizeof(Command), c->length, f);
  return c;
}

void Commands_Delete(Commands* c)
{
  assert(c);
  free(c->command);
  free(c);
}
