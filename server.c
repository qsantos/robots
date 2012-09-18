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
#include <stdlib.h>
#include <sys/select.h>
#include <ctype.h>

#include "socket.h"
#include "comm.h"

int main(int argc, char** argv)
{
  char* interface = "127.0.0.1";
  u32   port      = 4242;
  u32   n_clients = 2;

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "i:n:p:")) != -1)
    switch (c)
    {
    case 'i':
      interface = optarg;
      break;

    case 'n':
      n_clients = (u32) atoi(optarg);
      break;

    case 'p':
      port = (u32) atoi(optarg);
      break;

    case '?':
      if ((optopt == 'i') || (optopt == 'n') || (optopt == 'p'))
	fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
	fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;

    default:
      abort();
    }

  printf("Listening on %s:%lu\n", interface, port);
  printf("Waiting for %lu clients\n", n_clients);
  s32 server = TCP_ListenTo(interface, port);
  if (server < 0)
  {
    fprintf(stderr, "The server could not bind the adequate port\n");
    return 1;
  }
  
  State* state = State_New(n_clients);
  for (u32 i = 0; i < n_clients; i++)
  {
    state->fh[i] = TCP_Accept(server);
    state->fd[i] = fileno(state->fh[i]);
  }
  int fd_max = state->fd[n_clients - 1] + 1;
  
  {
    Robot tmp = { 42.0, 0.0, 90.0, 100.0 };
    state->robot[0] = tmp;
  }
  {
    Robot tmp = { 36.0, 1.0, 0.0,  26.0  };
    state->robot[1] = tmp;
  }
  
  fd_set fds;
  while (42)
  {
    FD_ZERO(&fds);
    for (u32 i = 0; i < n_clients; i++)
      FD_SET(state->fd[i], &fds);

    select(fd_max, &fds, NULL, NULL, NULL);

    for (u32 i = 0; i < n_clients; i++)
      if (FD_ISSET(state->fd[i], &fds))
      {
	Commands* c = Commands_Get(state->fh[i]);
	State_Update(state, i, c);
	Commands_Delete(c);
      }

    State_Debug(state);
  }

  for (u32 i = 0; i < n_clients; i++)
    fclose(state->fh[i]);
  TCP_Close(server);
  State_Delete(state);
  return 0;
}
