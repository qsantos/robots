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

#include <ctype.h>
#include <unistd.h>

#include "socket.h"
#include "game.h"

s32 server = 0;
#define ORDER(Name, CODE)                   \
void Order_##Name(float speed)              \
{                                           \
  static const u8 code = CODE;              \
  write(server, &code, sizeof(u8));         \
  write(server, &speed, sizeof(float));     \
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
  if (server < 0)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return 1;
  }
  
  u8 server_hello[2];
  read(server, &server_hello, sizeof(u8) * 2);
  
  Game game;
  read(server, &game, sizeof(Game));
  
  while (game.n_clients < game.n_slots)
    read(server, &game.n_clients, sizeof(u32));
  
  Robot r;
  read(server, &r, sizeof(Robot));
  
  u8 start;
  read(server, &start, sizeof(u8));

  Order_Advance( 1.0);
  Order_Turn   ( 0.1);
  Order_TurnGun(-0.3);
    
  while (42)
  {
    usleep(1000000 / FRAMERATE);
  }

  close(server);
  return 0;
}
