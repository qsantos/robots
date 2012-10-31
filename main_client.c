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
#include <sys/timeb.h>

#include "socket.h"
#include "game.h"

s32   server = 0;
Robot robot;

#define ORDER(Name, CODE)                     \
void Order_##Name(float param)                \
{                                             \
	static Order order = { CODE, 0 };     \
	order.param = param;                  \
	write(server, &order, sizeof(Order)); \
}
ORDER(Advance, O_ADVANCE)
ORDER(Turn,    O_TURN   )
ORDER(TurnGun, O_TURNGUN)
ORDER(Fire,    O_FIRE   )

void handleEvents()
{
	EventCode eventCode;
	read(server, &eventCode, sizeof(EventCode));
	Robot  r;
	Bullet b;
	u32    u;
	switch (eventCode)
	{
		case E_TICK:
		{
/*
			static struct timeb last = { 0, 0, 0, 0 };
			struct timeb cur;
			ftime(&cur);
			float elapsed = (cur.time-last.time) + ((float)(cur.millitm-last.millitm)/1000);
			if (elapsed > 1)
			{
				Order_Fire(10);
				last = cur;
			}
*/
			break;
		}
		case E_DUMP:
			read(server, &robot, sizeof(Robot));
			break;
		case E_ROBOT:
			read(server, &r, sizeof(Robot));
			Order_Fire(10);
			break;
		case E_BULLET:
			read(server, &b, sizeof(Bullet));
			if (b.from != robot.id)
				Order_Advance(-100);
			break;
		case E_HIT:
			read(server, &b, sizeof(Bullet));
			read(server, &u, sizeof(u32));
			break;
		case E_HITBY:
			read(server, &b, sizeof(Bullet));
			break;
		case E_HITROBOT:
			read(server, &u, sizeof(u32));
			break;
		case E_HITWALL:
			break;
		case E_KABOUM:
			read(server, &r, sizeof(Robot));
			break;
	}
}

int main(int argc, char** argv)
{
	char* address = argc > 1 ? argv[1]             : "127.0.0.1";
	u16   port    = argc > 2 ? (u32) atoi(argv[2]) : 4242;

//	usage(); // TODO: check port and IP

	printf("Connecting to %s:%lu\n", address, port);
	server = TCP_Connect(address, port);
	if (server < 0)
	{
		fprintf(stderr, "Could not connect to the server\n");
		exit(1);
	}

	write(server, &MAGIC_WORD,     sizeof(u8));
	write(server, &VERSION_NUMBER, sizeof(u8));

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

	Order_Advance( 50);
	Order_Turn   ( 30);
	Order_TurnGun(-90);

	while (42)
		handleEvents();

	close(server);
	return 0;
}
