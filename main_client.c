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

void cbStart(Robot* robot)
{
	(void) robot;
	Order_Advance( 50);
	Order_Turn   ( 30);
	Order_TurnGun(-90);
}

void cbRobot(Robot* robot, Robot* r)
{
	(void) robot;
	(void) r;
	Order_Fire(10);
}

void cbBullet(Robot* robot, Bullet* b)
{
	if (b->from != robot->id)
		Order_Advance(-100);
}

int main(int argc, char** argv)
{
	cb_Start  = cbStart;
	cb_Robot  = cbRobot;
	cb_Bullet = cbBullet;

	autoClient(argc, argv);
	return 0;
}
