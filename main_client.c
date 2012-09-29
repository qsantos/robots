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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "socket.h"
#include "game.h"

FILE* server = NULL;
#define ORDER(Name, CODE)                   \
void Order_##Name(float speed)              \
{                                           \
  static const u8 code = CODE;              \
  fwrite(&code,  sizeof(u8),    1, server); \
  fwrite(&speed, sizeof(float), 1, server); \
  fflush(server);                           \
}

ORDER(Advance, ADVANCE)
ORDER(Turn,    TURN   )
ORDER(TurnGun, TURNGUN)
ORDER(Fire,    FIRE   )

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

  printf("Connecting to %s:%lu\n", interface, port);
  server = TCP_Connect(interface, port);
  if (!server)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return 1;
  }
  
  u8 server_hello[2];
  fread(&server_hello, 1, 2, server);
  
  Game game;
  fread(&game, sizeof(Game), 1, server);
  
  while (game.n_clients < game.n_slots)
    fread(&game.n_clients, sizeof(u32), 1, server);
  
  Robot r;
  fread(&r, sizeof(Robot), 1, server);
  
  u8 start;
  fread(&start, 1, 1, server);

  Order_Advance( 1.0);
  Order_Turn   ( 0.1);
  Order_TurnGun(-0.3);
    
  while (42)
  {
    usleep(1000000 / FRAMERATE);
  }

  fclose(server);
  return 0;
}
