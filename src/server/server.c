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

#include "server.h"

#include <time.h>
#include <sys/epoll.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../socket.h"
#include "../math.h"

#define RADAR_SIGHT 1000

// returns the absolute-max value allowed
static inline float rule_max(float rule, float x)
{
	return fabs(x) < rule ? x : (x < 0 ? (-rule) : rule);
}

static inline void decreaseEnergy(Server* s, u32 i, float amount)
{
	s->robots[i].energy -= amount;
	if (s->robots[i].energy < 0)
	{
		static const EventCode event_code = E_KABOUM;
		write(s->display, &event_code,   sizeof(EventCode));
		write(s->display, &s->robots[i], sizeof(Robot));
		FOREACH(s->, robots, j)
			write(s->clients[j], &event_code,   sizeof(EventCode));
			write(s->clients[j], &s->robots[i], sizeof(Robot));
		DONE

		DISABLE(s->, robotOrders, i);
		DISABLE(s->, robots, i);
	}
}

Server* Server_New(int socket, u32 n_clients)
{
	Server* ret = MALLOC(Server, 1);

	ret->listener = socket;
	ret->clients  = MALLOC(int, n_clients);
	Game g = {1000, 1000, 100, PI/2, PI, -1, n_clients, 0};
	ret->game = g;

	INIT(ret->, robots);
	INIT(ret->, robotOrders);
	INIT(ret->, bullets);

	return ret;
}

void Server_Delete(Server* s)
{
	assert(s);

	FREE(s->, bullets);
	FREE(s->, robotOrders);
	FREE(s->, robots);
	for (u32 i = 0; i < s->game.n_clients; i++)
		close(s->clients[i]);
	free(s->clients);
	close(s->display);
	close(s->listener);
	free(s);
}

void Server_Debug(Server* s)
{
	assert(s);

	printf("==================================\n");
	printf("Robots:\n");
	FOREACH(s->, robots, i)
		Robot*      r = &s->robots[i];
		RobotOrder* o = &s->robotOrders[i];
		printf("#%lu: (%f, %f) %f° %f° %f > %f %f° %f°\n", i, r->x, r->y, rad2deg(r->angle), rad2deg(r->gunAngle), r->energy, o->advance, rad2deg(o->turn), rad2deg(o->turnGun));
	DONE
	printf("\n");

	printf("Bullets:\n");
	FOREACH(s->, bullets, i)
		Bullet* b = &s->bullets[i];
		printf("#%lu: (%f, %f), %f°, %f\n", i, b->x, b->y, rad2deg(b->angle), b->energy);
	DONE
	printf("\n");
}

void Server_AcceptDisplay(Server* s)
{
	assert(s);
	s->display = TCP_Accept(s->listener);
	printf("Display accepted\n");
	int flags = fcntl(s->display, F_GETFL, 0);
	fcntl(s->display, F_SETFL, flags | O_NONBLOCK);
}

void Server_AcceptClients(Server* s)
{
	assert(s);

	printf("Waiting for %lu clients\n", s->game.n_slots);
	for (u32 i = 0; i < s->game.n_slots; i++)
	{
		s->clients[i] = TCP_Accept(s->listener);
		printf("One client accepted\n");
		s->game.n_clients++;

		u8 hello[2];
		read(s->clients[i], hello, sizeof(u8) * 2);

		write(s->clients[i], &MAGIC_WORD,     sizeof(u8));
		write(s->clients[i], &VERSION_NUMBER, sizeof(u8));
		write(s->clients[i], &s->game,        sizeof(Game));
		for (u32 j = 0; j < i; j++)
			write(s->clients[j], &s->game.n_clients, sizeof(u32));
	}
}

// TODO: id is not cell num
bool Server_HandleOrder(Server* s, u32 id)
{
	assert(s);

	Order order;
	if (read(s->clients[id], &order,  sizeof(Order)) <= 0)
		return false;

	Robot*      r = &s->robots[id];
	RobotOrder* o = &s->robotOrders[id];
	float rule;
	switch (order.code)
	{
	case O_ADVANCE:
		o->advance = order.param;
		break;

	case O_TURN:
		o->turn = order.param;
		break;

	case O_TURNGUN:
		o->turnGun = order.param;
		break;

	case O_FIRE:
		rule = s->game.max_fireEnergy;
		if ((rule < 0 || (0 <= order.param && order.param <= rule)) && r->energy >= order.param)
		{
			r->energy -= order.param;
			// don't merge the next lines: ENABLE may change s->bullets pointer
			u32 i;
			ENABLE(s->, Bullet, bullets, i);
			Bullet* b = &s->bullets[i];
			b->from    = r->id;
			b->angle   = r->angle + r->gunAngle;
			b->x       = r->x + 100 * sin(b->angle);
			b->y       = r->y - 100 * cos(b->angle);
			b->energy  = order.param;
		}
		break;

	case O_VELOCITY:
		rule = s->game.max_velocity;
		if (rule < 0 || (0 <= order.param && order.param <= rule))
			r->velocity = order.param;
		break;

	case O_TURNSPEED:
		rule = s->game.max_turnSpeed;
		if (rule < 0 || (0 <= order.param && order.param <= rule))
			r->turnSpeed = order.param;
		break;

	case O_GUNSPEED:
		rule = s->game.max_gunSpeed;
		if (rule < 0 || (0 <= order.param && order.param <= rule))
			r->gunSpeed = order.param;
		break;

	}

	return true;
}

void Server_Tick(Server* s, float time)
{
	assert(s);

	FOREACH(s->, robots, i)
		Robot*      r = &s->robots[i];
		RobotOrder* o = &s->robotOrders[i];

		float gunAngle = rule_max(time * r->gunSpeed, o->turnGun);
		r->gunAngle += gunAngle;
		o->turnGun -= gunAngle;

		// if no collision occurs, we confirm the move by copying back the new data
		Robot nr;
		memcpy(&nr, r, sizeof(Robot));

		float angle = rule_max(time * r->turnSpeed, o->turn);
		nr.angle += angle;
		o->turn -= angle;

		float distance = rule_max(time * r->velocity, o->advance);
		nr.x += distance * sin(r->angle);
		nr.y -= distance * cos(r->angle);
		o->advance -= distance;

		bool collide = !GameContainsRobot(&s->game, &nr);
		if (collide)
		{
			static const EventCode eventCode = E_HITWALL;
			write(s->display,    &eventCode, sizeof(EventCode));
			write(s->display,    &i,         sizeof(u32));
			write(s->clients[i], &eventCode, sizeof(EventCode));
		}
		FOREACH(s->, robots, j)
			if (j != i)
			{
				float distance = distanceRobots(r, &s->robots[j]);
				float angle    = angleRobots   (r, &s->robots[j]);
				float radar    = normRad(r->angle + r->gunAngle);
				if (distance <= RADAR_SIGHT && fabs(radar - angle) <= PI/8)
				{
					static const EventCode eventCode = E_ROBOT;
					write(s->display,    &eventCode,    sizeof(EventCode));
					write(s->display,    &i,            sizeof(u32));
					write(s->display,    &s->robots[j], sizeof(Robot));
					write(s->clients[i], &eventCode,    sizeof(EventCode));
					write(s->clients[i], &s->robots[j], sizeof(Robot));
				}
				if (RobotCollideRobot(&s->robots[j], &nr))
				{
					collide = true;

					static const EventCode eventCode = E_HITROBOT;
					write(s->display,    &eventCode, sizeof(EventCode));
					write(s->display,    &i,         sizeof(u32));
					write(s->display,    &j,         sizeof(u32));
					write(s->clients[i], &eventCode, sizeof(EventCode));
					write(s->clients[i], &j,         sizeof(u32));
				}
			}
		DONE
		if (collide)
		{
			o->advance = 0;
			o->turn    = 0;
		}
		else
		{
			memcpy(r, &nr, sizeof(Robot));
		}
	DONE

	FOREACH(s->, bullets, i)
		Bullet* b = &s->bullets[i];
		b->x += time * 100 * sin(b->angle);
		b->y -= time * 100 * cos(b->angle);
		if (!GameContainsPoint(&s->game, b->x, b->y))
			DISABLE(s->, bullets, i)
		else
			FOREACH(s->, robots, j)
				Robot* r = &s->robots[j];
// TODO: a bullet might be disabled and then spotted...
				if (RobotCollidePoint(r, b->x, b->y))
				{
					{
						static const EventCode eventCode = E_HIT;
						write(s->display,          &eventCode, sizeof(EventCode));
						write(s->display,          &b->from,   sizeof(u32));
						write(s->display,          b,          sizeof(Bullet));
						write(s->display,          &j,         sizeof(u32));
						write(s->clients[b->from], &eventCode, sizeof(EventCode));
						write(s->clients[b->from], b,          sizeof(Bullet));
						write(s->clients[b->from], &j,         sizeof(u32));
					}
					{
						static const EventCode eventCode = E_HITBY;
						write(s->display,    &eventCode, sizeof(EventCode));
						write(s->display,    &j,         sizeof(u32));
						write(s->display,    b,          sizeof(Bullet));
						write(s->clients[j], &eventCode, sizeof(EventCode));
						write(s->clients[j], b,          sizeof(Bullet));
					}

					s->robots[b->from].energy += b->energy * 1.5;
					decreaseEnergy(s, j, b->energy);
					DISABLE(s->, bullets, i);
				}
				else
				{
					float distance = distanceBullet(r, b);
					float angle    = angleBullet   (r, b);
					float radar    = normRad(r->angle + r->gunAngle);
					if (distance <= RADAR_SIGHT && fabs(radar - angle) <= PI/8)
					{
						static const EventCode eventCode = E_BULLET;
						write(s->display,    &eventCode, sizeof(EventCode));
						write(s->display,    &j,         sizeof(u32));
						write(s->display,    b,          sizeof(Bullet));
						write(s->clients[j], &eventCode, sizeof(EventCode));
						write(s->clients[j], b,          sizeof(Bullet));
					}
				}
			DONE
	DONE

	{
		static const EventCode eventCode = E_TICK;
		write(s->display, &eventCode, sizeof(EventCode));
		FOREACH(s->, robots, i)
			write(s->clients[i], &eventCode, sizeof(EventCode));
		DONE
	}
}

void Server_Dump(Server* s)
{
	assert(s);

	static const EventCode eventCode = E_DUMP;

	write(s->display, &eventCode,   sizeof(EventCode));
	write(s->display, &s->game,     sizeof(Game));
	write(s->display, &s->n_robots, sizeof(u32));
	FOREACH(s->, robots, i)
		write(s->display, &s->robots[i], sizeof(Robot));
	DONE
	FOREACH(s->, robotOrders, i);
		write(s->display, &s->robotOrders[i], sizeof(RobotOrder));
	DONE
	write(s->display, &s->n_bullets, sizeof(u32));
	FOREACH(s->, bullets, i)
		write(s->display, &s->bullets[i], sizeof(Bullet));
	DONE

	FOREACH(s->, robots, i)
		write(s->clients[i], &eventCode,         sizeof(EventCode));
		write(s->clients[i], &s->robots[i],      sizeof(Robot));
		write(s->clients[i], &s->robotOrders[i], sizeof(RobotOrder));
	DONE
}

void Server_Loop(Server* s)
{
	assert(s);

	int fd_max = 0;
	for (u32 i = 0; i < s->game.n_clients; i++)
		if (s->clients[i] > fd_max)
		fd_max = s->clients[i];
	fd_max++;

	srandom(time(NULL));
	u32 curId = 0;
	float margin = hypothenuse(73, 116) / 2;
	for (u32 i = 0; i < s->game.n_clients; i++)
	{
		Robot nr =
		{
			curId++,
			randf(margin, s->game.width  - margin),
			randf(margin, s->game.height - margin),
			73,
			116,
			deg2rad(random() % 360),
			0, 100.0,
			s->game.max_velocity,
			s->game.max_turnSpeed,
			s->game.max_gunSpeed,
		};
		// don't merge the next lines: ENABLE may change s->robots pointer
		u32 id;
		u32 id2;
		ENABLE(s->, Robot,      robots, id);
		ENABLE(s->, RobotOrder, robotOrders, id2);
		assert(i == id);   // TODO
		assert(id == id2); // TODO

		Robot* r = &s->robots[id];
		memcpy(r, &nr, sizeof(Robot));
		memset(&s->robotOrders[id], 0, sizeof(RobotOrder));
		write(s->clients[i], r, sizeof(Robot));
	}
	for (u32 i = 0; i < s->game.n_clients; i++)
		write(s->clients[i], &START_MESSAGE, sizeof(u8));


	int epollfd = epoll_create(s->game.n_clients);
	for (u32 i = 0; i < s->game.n_clients; i++)
	{
		int flags = fcntl(s->clients[i], F_GETFL, 0);
		fcntl(s->clients[i], F_SETFL, flags | O_NONBLOCK);

		struct epoll_event ev;
		ev.events   = EPOLLIN;
		ev.data.u32 = i;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, s->clients[i], &ev);
	}

	struct timeb last;
	struct timeb cur;
	ftime(&cur);
	while (42)
	{
		struct epoll_event events[10];
		u32 n_events = epoll_wait(epollfd, events, 10, 1000 / FRAMERATE);
		for (u32 i = 0; i < n_events; i++)
		{
			u32 client = events[i].data.u32;
			while (Server_HandleOrder(s, client));
		}

		last = cur;
		ftime(&cur);
		Server_Tick(s, (cur.time-last.time)+((float)(cur.millitm-last.millitm) / 1000));
//		Server_Debug(s);
		Server_Dump(s);
	}
}
