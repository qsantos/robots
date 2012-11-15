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
#include "math.h"

static inline float min(float a, float b)
{
	return a < b ? a : b;
}
static inline float max(float a, float b)
{
	return a > b ? a : b;
}

void* mainMove(void* param)
{
	(void) param;

	float margin = hypothenuse(robot.width, robot.height) / 2 + 1;

	// compute the distance to the closest wall and fetch its direction
	float north = robot.y;
	float south = game.height - robot.y;
	float west  = robot.x;
	float east  = game.width - robot.x;
	float closest = min( min(north, south) , min(west, east) );
	int step = closest == north ? 0 :
	           closest == east  ? 1 :
	           closest == south ? 2 :
                                      3;
	// head toward this wall
	Order_TurnGun(PI/2);
	Order_BlockTurn(cfmod(step*PI/2-robot.angle, 2*PI));
	while (1)
	{
		// circle around the map
		float d = step == 0 ? robot.y :
		          step == 1 ? game.width  - robot.x :
		          step == 2 ? game.height - robot.y :
		                      robot.x;
		Order_BlockAdvance(d - margin);
		Order_BlockTurn(PI/2);
		step = (step+1)%4;
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
	Order_Fire(1);
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
