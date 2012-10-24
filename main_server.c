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

#include "server.h"

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
  
  Server* server = Server_New(interface, port, n_clients);
  assert(server);

  Server_AcceptDisplay(server);
  Server_AcceptClients(server);

  printf("Starting simulation\n");
  Server_Loop(server);
  
  Server_Delete(server);
  return 0;
}
