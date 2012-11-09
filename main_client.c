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

#include <pthread.h>

#include "client.h"

void* mainMove(void* param)
{
	(void) param;

	Order_BlockTurn(-robot.angle);
	while (1)
	{
		Order_BlockAdvance(100);
		Order_BlockAdvance(-100);
	}

	return NULL;
}

pthread_t mainThread = 0;
void cbStart()
{
	pthread_create(&mainThread, NULL, mainMove, NULL);
	Order_Turn(-robot.angle);
}

void cbRobot(Robot* r)
{
	(void) r;
//	Order_Fire(1);
}

int main(int argc, char** argv)
{
	cb_Start  = cbStart;
	cb_Robot  = cbRobot;

	autoClient(argc, argv);

	pthread_cancel(mainThread);
	pthread_join(mainThread, NULL);
	return 0;
}
