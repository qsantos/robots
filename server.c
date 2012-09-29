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

#include <time.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

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
  
  ret->client        = ALLOC(s32,   n_clients);
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
    close(s->client[i]);
  free(s->client);
  close(s->display);
  close(s->listener);
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
    s->client[i] = TCP_Accept(s->listener);
    s->game.n_clients++;
    
    write(s->client[i], &MAGIC_WORD,     sizeof(u8));
    write(s->client[i], &VERSION_NUMBER, sizeof(u8));
    write(s->client[i], &s->game,        sizeof(Game));
    for (u32 j = 0; j < i; j++)
      write(s->client[j], &s->game.n_clients, sizeof(u32));
  }
}

bool Server_HandleOrder(Server* s, u32 id)
{
  u8 code;
  float param;
  if (read(s->client[id], &code,  sizeof(u8))    <= 0) return false;
  if (read(s->client[id], &param, sizeof(float)) <= 0) return false;

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

void Server_Dump(Server* s, s32 f)
{
  assert(s);
  write(f, &s->game,      sizeof(Game));
  write(f, &s->n_robots,  sizeof(u32));
  write(f, s->robot,      sizeof(Robot) * s->game.n_clients);
//  write(f, &s->n_bullets, sizeof(u32));
//  write(f, &s->bullet,    sizeof(Bullet) * s->game.n_bullets);
}

void Server_Loop(Server* s)
{
  assert(s);
  
  int fd_max = 0;
  for (u32 i = 0; i < s->game.n_clients; i++)
    if (s->client[i] > fd_max)
      fd_max = s->client[i];
  fd_max++;
  
  srandom(time(NULL));
  for (u32 i = 0; i < s->game.n_clients; i++)
  {
    Robot r = { random() % (u32)s->game.width, random() % (u32)s->game.height, deg2rad(random() % 360), 0, 100.0, 0, 0, 0 };
    s->robot[i] = r;
    write(s->client[i], &r, sizeof(Robot));
  }
  for (u32 i = 0; i < s->game.n_clients; i++)
    write(s->client[i], &START_MESSAGE, sizeof(u8));


  int epollfd = epoll_create(s->game.n_clients);
  struct epoll_event  ev;
  for (u32 i = 0; i < s->game.n_clients; i++)
  {
    int flags = fcntl(s->client[i], F_GETFL, 0);
    fcntl(s->client[i], F_SETFL, flags | O_NONBLOCK);
    ev.events   = EPOLLIN;
    ev.data.u32 = i;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, s->client[i], &ev);
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
    Server_Debug(s);
  }
}
