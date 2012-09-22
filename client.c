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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "socket.h"
#include "comm.h"

int main(int argc, char** argv)
{
  char* interface = "127.0.0.1";
  u32   port      = 4242;

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "i:p:")) != -1)
    switch (c)
    {
    case 'i':
      interface = optarg;
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

  FILE* server = TCP_Connect(interface, port);
  if (!server)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return 1;
  }

  while (42)
  {
    Command cmd = { FORWARD, 1.0 };
    Command_Send(server, cmd);
    usleep(1000000 / FRAMERATE);
  }

  fclose(server);

  return 0;
}
