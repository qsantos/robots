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
  FILE* toto = TCP_Connect("127.0.0.1", 4242);

  /* TEST */
  //fprintf(toto, "YEAAAAAAAAAAAAAAAAAAH\n");
  Robot bugs_bunny[] =
  {
    { 42.0, 0.0, 1.0, 100.0 },
    { 36.0, 1.0, 0.0, 26.0  },
  };
  State state = { 2, bugs_bunny };
  
  File_SendState(toto, &state);

  fclose(toto);

  return 0;
}
