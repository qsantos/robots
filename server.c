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

#include "server.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "socket.h"

const u8 MAGIC_WORD     = 0x42;
const u8 VERSION_NUMBER = 0x01;
const u8 START_MESSAGE  = 0x42;

Server* Server_New(string interface, u16 port, u32 n_clients)
{
  Server* ret = ALLOC(Server, 1);
  
  printf("Listening on %s:%u\n", interface, port);
  printf("Waiting for %lu clients\n", n_clients);
  ret->listener = TCP_ListenTo(interface, port);
  if (ret->listener < 0)
  {
    fprintf(stderr, "The server could not bind the adequate port\n");
    return NULL;
  }
  
  ret->fd        = ALLOC(int,   n_clients);
  ret->fh        = ALLOC(FILE*, n_clients);
  ret->n_robots  = n_clients;
  ret->robot     = ALLOC(Robot, n_clients);
  
  ret->game.width     = 1024;
  ret->game.height    = 768;
  ret->game.n_slots   = n_clients;
  ret->game.n_clients = 0;
//  ret->n_bullets = 0;
//  ret->bullet    = NULL;
  
  return ret;
}

void Server_Delete(Server* s)
{
  assert(s);
//  free(s->bullet);
  free(s->robot);
  for (u32 i = 0; i < s->game.n_clients; i++)
    fclose(s->fh[i]);
  free(s->fh);
  free(s->fd);
  fclose(s->display);
  TCP_Close(s->listener);
  free(s);
}

void Server_Debug(Server* s)
{
  assert(s);
  printf("==================================\n");
  printf("Robots:\n");
  for (u32 i = 0; i < s->n_robots; i++)
  {
    Robot r = s->robot[i];
    printf("#%lu: (%f, %f) %f° %f° %f%%\n", i, r.x, r.y, r.angle, r.gunAngle, r.energy);
  }
  printf("\n");
/*
  printf("Bullets:\n");
  for (u32 i = 0; i < s->n_bullets; i++)
  {
    Bullet b = s->bullet[i];
    printf("#%lu: (%f, %f), %f°, %f%%\n", i, b.x, b.y, b.angle, b.energy);
  }
  printf("\n");
*/
}

void Server_AcceptDisplay(Server* s)
{
  s->display = TCP_Accept(s->listener);
}

void Server_AcceptClients(Server* s)
{
  for (u32 i = 0; i < s->game.n_slots; i++)
  {
    s->fh[i] = TCP_Accept(s->listener);
    s->fd[i] = fileno(s->fh[i]);
    s->game.n_clients++;
    
    fwrite(&MAGIC_WORD,     1,            1, s->fh[i]);
    fwrite(&VERSION_NUMBER, 1,            1, s->fh[i]);
    fwrite(&s->game,        sizeof(Game), 1, s->fh[i]);
    fflush(s->fh[i]);
    for (u32 j = 0; j < i; j++)
    {
      fwrite(&s->game.n_clients, sizeof(u32), 1, s->fh[j]);
      fflush(s->fh[j]);
    }
  }
}

bool Server_HandleOrder(Server* s, u32 id)
{
  
  u8 code;
  if (fread(&code,  sizeof(u8),    1, s->fh[id]) <= 0) return false;
  float param;
  if (fread(&param, sizeof(float), 1, s->fh[id]) <= 0) return false;

  switch (code)
  {
  case ADVANCE:
    s->robot[id].velocity = param;
    break;

  case TURN:
    s->robot[id].turnSpeed = param;
    break;

  case TURNGUN:
    s->robot[id].turnGunSpeed = param;
    break;

  case FIRE:
/*
    s->bullet = REALLOC(s->bullet, Bullet, s->n_bullets+1);
    s->bullet[s->n_bullets].x  = s->robot[id].x;
    s->bullet[s->n_bullets].y  = s->robot[id].y;
    s->bullet[s->n_bullets].angle = s->robot[id].angle + s->robot[id].gunAngle;
    s->bullet[s->n_bullets].energy = param;
    s->n_bullets++;
*/
    break;
  }
  
  Server_Debug(s);
  return true;
}

void Server_Tick(Server* s)
{
    for (u32 i = 0; i < s->n_robots; i++)
    {
      Robot* r = &s->robot[i];
      r->x += r->velocity* sin(r->angle);
      r->y -= r->velocity * cos(r->angle);
      r->angle += deg2rad(r->turnSpeed);
      r->gunAngle += deg2rad(r->turnGunSpeed);
    }
}

void Server_Dump(Server* s, FILE* f)
{
  assert(s);
  fwrite(&s->game,      sizeof(Game),   1,                  f);
  fwrite(&s->n_robots,  sizeof(u32),    1,                  f);
  fwrite(s->robot,      sizeof(Robot),  s->game.n_clients,  f);
//  fwrite(&s->n_bullets, sizeof(u32),    1,                  f);
//  fwrite(s->bullet,     sizeof(Bullet), s->n_bullets,       f);
  fflush(f);
}

void Server_Loop(Server* s)
{
  int fd_max = 0;
  for (u32 i = 0; i < s->game.n_clients; i++)
    if (s->fd[i] > fd_max)
      fd_max = s->fd[i];
  fd_max++;
  
  srandom(time(NULL));
  for (u32 i = 0; i < s->game.n_clients; i++)
  {
    Robot r= { random() % (u32)s->game.width, random() % (u32)s->game.height, deg2rad(random() % 360), 0, 100.0, 0, 0, 0 };
    s->robot[i] = r;
    fwrite(&r, sizeof(Robot), 1, s->fh[i]);
    fflush(s->fh[i]);
  }
  for (u32 i = 0; i < s->game.n_clients; i++)
  {
    fwrite(&START_MESSAGE, 1, 1, s->fh[i]);
    fflush(s->fh[i]);
  }


  int epollfd = epoll_create(s->game.n_clients);
  struct epoll_event  ev;
  for (u32 i = 0; i < s->game.n_clients; i++)
  {
    int flags = fcntl (s->fd[i], F_GETFL, 0);
    fcntl (s->fd[i], F_SETFL, flags | O_NONBLOCK);
    ev.events   = EPOLLIN;
    ev.data.u32 = i;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, s->fd[i], &ev);
  }
  
  struct epoll_event events[10];
  while (42)
  {
    int n_events = epoll_wait(epollfd, events, 10, 1000 / FRAMERATE);
    for (s32 i = 0; i < n_events; i++)
    {
      u32 client = events[i].data.u32;
      while (Server_HandleOrder(s, client));
    }
    Server_Tick(s);
    Server_Dump(s, s->display);
  }
}
