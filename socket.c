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

#include "socket.h"

#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>

FILE* TCP_Connect(const string IP, u16 port)
{
  s32 sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
  {
    fprintf(stderr, "Could not make socket\n");
    assert(false);
  }

  struct sockaddr_in host;
  memset(&host, 0, sizeof(struct sockaddr_in));
  inet_pton(AF_INET, IP, &host.sin_addr);
  host.sin_family = AF_INET;
  host.sin_port   = htons(port);

  s32 res = connect(sock, (struct sockaddr*) &host, sizeof(struct sockaddr));
  if (res < 0)
  {
    close(sock);
    fprintf(stderr, "Could not connect\n");
    assert(false);
  }

  FILE* ret = fdopen(sock, "a+");
  if (!ret)
  {
    close(sock);
    fprintf(stderr, "Could not open as a file\n");
    assert(false);
  }

  return ret;
}

u32 TCP_ListenTo(const string IP, u16 port)
{
  s32 sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
  {
    fprintf(stderr, "Could not make socket\n");
    assert(false);
  }

  struct sockaddr_in host;
  memset(&host, 0, sizeof(struct sockaddr_in));
  inet_pton(AF_INET, IP, &host.sin_addr);
  host.sin_family = AF_INET;
  host.sin_port   = htons(port);

  int v = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(int));

  s32 res = bind(sock, (struct sockaddr*) &host, sizeof(struct sockaddr));
  if (res < 0)
  {
    close(sock);
    fprintf(stderr, "Could not bind port\n");
    assert(false);
  }

  res = listen(sock, 10);
  if (res < 0)
  {
    close(sock);
    assert(false);
  }

  return (u32) sock;
}

u32 TCP_Listen(u16 port)
{
  return TCP_ListenTo("0.0.0.0", port);
}

FILE* TCP_Accept(u32 sock)
{
  s32 client = accept(sock, NULL, NULL);
  if (client < 0)
  {
    fprintf(stderr, "Could not accept client\n");
    assert(false);
  }

  FILE* ret = fdopen(client, "a+");
  if (!ret)
  {
    fprintf(stderr, "Could not open as a file\n");
    close(client);
    assert(false);
  }

  return ret;
}

void TCP_Close(u32 sock)
{
  shutdown(sock, SHUT_RDWR);
  close(sock);
}
