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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <stdio.h>
#include <sys/select.h>

#include "socket.h"
#include "comm.h"

int main(void)
{
  s32 server = TCP_ListenTo("127.0.0.1", 4242);
  if (server < 0)
  {
    fprintf(stderr, "The server could not bind the adequate port\n");
    return 1;
  }
  
  u32 n_clients = 2;

  int*   fd = ALLOC(int,   n_clients);
  FILE** fh = ALLOC(FILE*, n_clients);
  for (u32 i = 0; i < n_clients; i++)
  {
    fh[i] = TCP_Accept(server);
    fd[i] = fileno(fh[i]);
  }
  int fd_max = fd[n_clients - 1] + 1;

  Robot bugs_bunny[] =
  {
    { 42.0, 0.0, 90.0, 100.0 },
    { 36.0, 1.0, 0.0,  26.0  },
  };
  Bullet* bullets = NULL;
  State state = { 2, 0, bugs_bunny, bullets };
  
  fd_set fds;
  while (42)
  {
    FD_ZERO(&fds);
    for (u32 i = 0; i < n_clients; i++)
      FD_SET(fd[i], &fds);

    select(fd_max, &fds, NULL, NULL, NULL);

    for (u32 i = 0; i < n_clients; i++)
      if (FD_ISSET(fd[i], &fds))
      {
	Commands* c = Commands_Get(fh[i]);
	State_Update(&state, i, c);
	Commands_Free(c);
      }

    State_Debug(&state);
  }

  for (u32 i = 0; i < n_clients; i++)
    fclose(fh[i]);
  free(fh);
  free(fd);

  TCP_Close(server);

  return 0;
}
