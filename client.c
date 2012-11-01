/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Quentin SANTOS
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

#include "client.h"

#include <unistd.h>

#include "socket.h"

Game  game;
int   server = 0;
Robot robot;

#define ORDER(Name, CODE)                     \
void Order_##Name(float param)                \
{                                             \
	static Order order = { CODE, 0 };     \
	order.param = param;                  \
	write(server, &order, sizeof(Order)); \
}
ORDER(Advance, O_ADVANCE)
ORDER(Turn,    O_TURN)
ORDER(TurnGun, O_TURNGUN)
ORDER(Fire,    O_FIRE)

void (*cb_Start   )(Robot*              ) = NULL;
void (*cb_Tick    )(Robot*              ) = NULL;
void (*cb_Dump    )(Robot*              ) = NULL;
void (*cb_Robot   )(Robot*, Robot*      ) = NULL;
void (*cb_Bullet  )(Robot*, Bullet*     ) = NULL;
void (*cb_Hit     )(Robot*, Bullet*, u32) = NULL;
void (*cb_HitBy   )(Robot*, Bullet*     ) = NULL;
void (*cb_HitRobot)(Robot*, u32         ) = NULL;
void (*cb_HitWall )(Robot*              ) = NULL;
void (*cb_Kaboum  )(Robot*, Robot*      ) = NULL;

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
			if (cb_Tick) cb_Tick(&robot);
			break;
		case E_DUMP:
			read(server, &robot, sizeof(Robot));
			if (cb_Dump) cb_Dump(&robot);
			break;
		case E_ROBOT:
			read(server, &r, sizeof(Robot));
			if (cb_Robot) cb_Robot(&robot, &r);
			break;
		case E_BULLET:
			read(server, &b, sizeof(Bullet));
			if (cb_Bullet) cb_Bullet(&robot, &b);
			break;
		case E_HIT:
			read(server, &b, sizeof(Bullet));
			read(server, &u, sizeof(u32));
			if (cb_Hit) cb_Hit(&robot, &b, u);
			break;
		case E_HITBY:
			read(server, &b, sizeof(Bullet));
			if (cb_HitBy) cb_HitBy(&robot, &b);
			break;
		case E_HITROBOT:
			read(server, &u, sizeof(u32));
			if (cb_HitRobot) cb_HitRobot(&robot, u);
			break;
		case E_HITWALL:
			if (cb_HitWall) cb_HitWall(&robot);
			break;
		case E_KABOUM:
			read(server, &r, sizeof(Robot));
			if (cb_Kaboum) cb_Kaboum(&robot, &r);
			break;
	}
}

static void usage(int argc, char** argv)
{
	(void) argc;
	printf
	(
		"Usage: %s [ADDRESS [PORT]]\n"
		"\n"
		"  ADDRESS  the node of the game server\n"
		"  PORT     the port to connect to\n"
		,
		argv[0]
	);
}

int autoClient(int argc, char** argv)
{
	const char* node    = argc > 1 ? argv[1] : "::1";
	const char* service = argc > 2 ? argv[2] : "4242";

	printf("Connecting to %s port %s\n", node, service);
	server = TCP_Connect(node, service);
	if (server < 0)
	{
		fprintf(stderr, "Could not connect to the server\n");
		usage(argc, argv);
		exit(1);
	}
	printf("Connected\n");

	write(server, &MAGIC_WORD,     sizeof(u8));
	write(server, &VERSION_NUMBER, sizeof(u8));

	u8 server_hello[2];
	read(server, &server_hello, sizeof(u8) * 2);

	read(server, &game, sizeof(Game));

	while (game.n_clients < game.n_slots)
		read(server, &game.n_clients, sizeof(u32));

	Robot r;
	read(server, &r, sizeof(Robot));

	u8 start;
	read(server, &start, sizeof(u8));

	if (cb_Start) cb_Start(&robot);

	while (42)
		handleEvents();

	close(server);
	return 0;
}
