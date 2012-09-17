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
#include "comm.h"

int main(void)
{
  u32 socket = TCP_ListenTo("127.0.0.1", 4242);
  
  while (42)
  {
    FILE* in = TCP_Accept(socket);

    Robot bugs_bunny[] =
      {
	{ 42.0, 0.0, 90.0, 100.0 },
	{ 36.0, 1.0, 0.0,  26.0  },
      };
    State state = { 2, bugs_bunny };
  
    State_Send(in, &state);
    Commands* c = Commands_Get(in);

    State_Update(&state, 1, c);
    State_Send(in, &state);

    Commands_Free(c);

    fclose(in);
  }

  TCP_Close(socket);

  return 0;
}
