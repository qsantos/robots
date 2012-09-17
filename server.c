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

    /* DEBUG */
/*    char c = fgetc(in);
    while (!feof(in))
    {
      putchar(c);
      c = fgetc(in);
      }*/
    State* s = File_GetState(in);
    printf("%lu\n", s->n_robots);
    for (u32 i = 0; i < s->n_robots; i++)
    {
      printf("x:      %f\n", s->robot[i].x);
      printf("y:      %f\n", s->robot[i].y);
      printf("angle:  %f\n", s->robot[i].angle);
      printf("energy: %f\n", s->robot[i].energy);
    }

    fclose(in);
  }

  TCP_Close(socket);

  return 0;
}
