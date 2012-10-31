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
#include "server.h"

void usage(int argc, char** argv)
{
	(void) argc;
	printf
	(
		"Usage: %s [ADDRESS [PORT [N_CLIENTS]]]\n"
		"\n"
		"  INTERFACE  the interface to listen to\n"
		"  PORT       the port to listen to\n"
		"  N_CLIENTS  the number of slots for robot clients\n"
		,
		argv[0]
	);
}

int main(int argc, char** argv)
{
	const char* node      = argc > 1 ? argv[1]       : "::1";
	const char* service   = argc > 2 ? argv[2]       : "4242";
	u32         n_clients = argc > 3 ? atoi(argv[3]) : 2;

	printf("Listening on %s port %s\n", node, service);
	int listener = TCP_ListenTo(node, service);
	if (listener < 0)
	{
		fprintf(stderr, "The server could not bind the adequate port\n");
		usage(argc, argv);
		exit(1);
	}

	Server* server = Server_New(listener, n_clients);
	assert(server);

	Server_AcceptDisplay(server);
	Server_AcceptClients(server);

	printf("Starting simulation\n");
	Server_Loop(server);

	Server_Delete(server);
	return 0;
}
