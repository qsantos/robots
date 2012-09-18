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

#include "socket.h"
#include "comm.h"

int main(void)
{
  FILE* toto = TCP_Connect("127.0.0.1", 4242);
  if (!toto)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return 1;
  }

  Command c = { FORWARD, 1.0 };
  Command_Send(toto, c);

  fclose(toto);

  return 0;
}
