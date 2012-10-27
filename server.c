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

#include "server.h"

#include <time.h>
#include <sys/epoll.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "socket.h"


#define FOREACH_ROBOT                                                \
	{                                                            \
		u32 i = 0;                                           \
		for (u32 i32 = 0; i32 < s->a_robots / 32 ; i32++)   \
		{                                                    \
			u32 bitfield = s->active_robots[i32];       \
			for (u32 j = 0; j < 32; i++, j++)            \
			{                                            \
				if (bitfield % 2)                    \
				{
#define DONE_ROBOT                                                   \
				}                                    \
				bitfield >>= 1;                      \
			}                                            \
		}                                                    \
	}

#define FOREACH_BULLET                                               \
	{                                                            \
		u32 i = 0;                                           \
		for (u32 i32 = 0; i32 < s->a_bullets / 32 ; i32++)   \
		{                                                    \
			u32 bitfield = s->active_bullets[i32];       \
			for (u32 j = 0; j < 32; i++, j++)            \
			{                                            \
				if (bitfield % 2)                    \
				{
#define DONE_BULLET                                                  \
				}                                    \
				bitfield >>= 1;                      \
			}                                            \
		}                                                    \
	}
 
static inline u32 enableRobot(Server* s)
{
	if (s->n_robots >= s->a_robots)
	{
		if (s->a_robots)
		{
			s->a_robots *= 2;
			s->active_robots = REALLOC(s->active_robots, u32, s->a_robots / 32);
			memset(&s->active_robots[s->a_robots/32/2], 0, s->a_robots/32*sizeof(u32)/2);
		}
		else
		{
			s->a_robots = 32;
			s->active_robots = REALLOC(s->active_robots, u32, s->a_robots / 32);
			memset(s->active_robots, 0, s->a_robots/32/sizeof(u32));
		}
		s->robots = REALLOC(s->robots, Robot, s->a_robots);
	}
	u32 i32 = 0;
	while (s->active_robots[i32] == (u32)-1)
		i32++;
	u32 bitfield = s->active_robots[i32];
	u32 i = i32 * 32;
	while (bitfield % 2)
	{
		i++;
		bitfield >>= 1;
	}
	s->active_robots[i/32] |= (1 << (i%32));
	s->n_robots++;
	
	return i;
}
static inline void disableRobot(Server* s, u32 i)
{
	s->active_robots[i/32] ^= (1 << (i%32));
	s->n_robots--;
}

static inline u32 enableBullet(Server* s)
{
	if (s->n_bullets >= s->a_bullets)
	{
		if (s->a_bullets)
		{
			s->a_bullets *= 2;
			s->active_bullets = REALLOC(s->active_bullets, u32, s->a_bullets / 32);
			memset(&s->active_bullets[s->a_bullets/32/2], 0, s->a_bullets/32*sizeof(u32)/2);
		}
		else
		{
			s->a_bullets = 32;
			s->active_bullets = REALLOC(s->active_bullets, u32, s->a_bullets / 32);
			memset(s->active_bullets, 0, s->a_bullets/32/sizeof(u32));
		}
		s->bullets = REALLOC(s->bullets, Bullet, s->a_bullets);
	}
	u32 i32 = 0;
	while (s->active_bullets[i32] == (u32)-1)
		i32++;
	u32 bitfield = s->active_bullets[i32];
	u32 i = i32 * 32;
	while (bitfield % 2)
	{
		i++;
		bitfield >>= 1;
	}
	s->active_bullets[i/32] |= (1 << (i%32));
	s->n_bullets++;
	
	return i;
}
static inline void disableBullet(Server* s, u32 i)
{
	s->active_bullets[i/32] ^= (1 << (i%32));
	s->n_bullets--;
}

Server* Server_New(string interface, u16 port, u32 n_clients)
{
	Server* ret = ALLOC(Server, 1);

	printf("Listening on %s:%u\n", interface, port);
	printf("Waiting for %lu clients\n", n_clients);
	ret->listener = TCP_ListenTo(interface, port);
	if (ret->listener < 0)
	{
		fprintf(stderr, "The server could not bind the adequate port\n");
		return NULL;
	}

	ret->client         = ALLOC(s32,   n_clients);

	ret->n_robots       = 0;
	ret->a_robots       = n_clients + (32 - (n_clients%32)); // must be a multiple of 32
	ret->active_robots  = ALLOC(u32,   ret->a_robots / 32);
	ret->robots         = ALLOC(Robot, ret->a_robots);

	ret->n_bullets      = 0;
	ret->a_bullets      = 0;
	ret->active_bullets = NULL;
	ret->bullets        = NULL;

	ret->game.width     = 1000;
	ret->game.height    = 1000;
	ret->game.n_slots   = n_clients;
	ret->game.n_clients = 0;

	return ret;
}

void Server_Delete(Server* s)
{
	assert(s);

	free(s->active_bullets);
	free(s->bullets);
	free(s->active_robots);
	free(s->robots);
	for (u32 i = 0; i < s->game.n_clients; i++)
		close(s->client[i]);
	free(s->client);
	close(s->display);
	close(s->listener);
	free(s);
}

void Server_Debug(Server* s)
{
	assert(s);

	printf("==================================\n");
	printf("Robots:\n");
	FOREACH_ROBOT
		Robot r = s->robots[i];
		printf("#%lu: (%f, %f) %f° %f° %f\n", i, r.x, r.y, r.angle, r.gunAngle, r.energy);
	DONE_ROBOT
	printf("\n");

	printf("Bullets:\n");
	FOREACH_BULLET
		Bullet* b = &s->bullets[i];
		printf("#%lu: (%f, %f), %f°, %f\n", i, b->x, b->y, b->angle, b->energy);
	DONE_BULLET
	printf("\n");

}

void Server_AcceptDisplay(Server* s)
{
	assert(s);

	s->display    = TCP_Accept(s->listener);
	s->display_fh = fdopen(s->display, "w");
}

void Server_AcceptClients(Server* s)
{
	for (u32 i = 0; i < s->game.n_slots; i++)
	{
		s->client[i] = TCP_Accept(s->listener);
		s->game.n_clients++;

		u8 hello[2];
		read(s->client[i],  hello,           sizeof(u8) * 2);
		write(s->client[i], &MAGIC_WORD,     sizeof(u8));
		write(s->client[i], &VERSION_NUMBER, sizeof(u8));
		write(s->client[i], &s->game,        sizeof(Game));
		for (u32 j = 0; j < i; j++)
		write(s->client[j], &s->game.n_clients, sizeof(u32));
	}
}

bool Server_HandleOrder(Server* s, u32 id)
{
	assert(s);

	Order order;
	if (read(s->client[id], &order,  sizeof(Order)) <= 0)
		return false;

	Robot* r = &s->robots[id];
	Bullet* b;
	u32 i;
	switch (order.code)
	{
	case O_ADVANCE:
		r->velocity = order.param;
		break;

	case O_TURN:
		r->turnSpeed = order.param;
		break;

	case O_TURNGUN:
		r->turnGunSpeed = order.param;
		break;

	case O_FIRE:
		// don't merge the two next lines: enableBullet may change s->bullets pointer
		i = enableBullet(s);
		b = &s->bullets[i];
		b->from   = r->id;
		b->angle  = r->angle + r->gunAngle;
		b->x      = r->x + 100 * sin(b->angle);
		b->y      = r->y - 100 * cos(b->angle);
		b->energy = order.param;
		break;
	}

	return true;
}

void Server_Tick(Server* s, float time)
{
	assert(s);

	FOREACH_ROBOT
		Robot* r = &s->robots[i];
		r->gunAngle += deg2rad(time * r->turnGunSpeed);
		if (r->velocity || r->turnSpeed)
		{
			Robot nr;
			memcpy(&nr, r, sizeof(Robot));
			nr.angle += deg2rad(time * r->turnSpeed);
			nr.x += time * r->velocity * sin(r->angle);
			nr.y -= time * r->velocity * cos(r->angle);

			bool collide = !GameContainsRobot(&s->game, &nr);
			for (u32 j = 0; j < s->n_robots && !collide; j++)
				if (j != i && RobotCollideRobot(&s->robots[j], &nr))
					collide = true;
			if (collide)
			{
				r->velocity  = 0;
				r->turnSpeed = 0;
			}
			else
				memcpy(r, &nr, sizeof(Robot));
		}
	DONE_ROBOT

	FOREACH_BULLET
		Bullet* b = &s->bullets[i];
		b->x += time * 100 * sin(b->angle);
		b->y -= time * 100 * cos(b->angle);
		if (!GameContainsPoint(&s->game, b->x, b->y))
			disableBullet(s, i);
		else
			for (u32 k = 0; k < s->n_robots; k++)
				if (RobotCollidePoint(&s->robots[k], b->x, b->y))
				{
					s->robots[b->from].energy += b->energy * 0.5;
					s->robots[k]      .energy -= b->energy;
					disableBullet(s, i);
					break;
				}
	DONE_BULLET
}

void Server_Dump(Server* s, FILE* f)
{
	assert(s);
	assert(f);

	static const u8 eventCode = E_DUMP;
	fwrite(&eventCode,    sizeof(u8),     1,           f);
	fwrite(&s->game,      sizeof(Game),   1,           f);
	fwrite(&s->n_robots,  sizeof(u32),    1,           f);
	fwrite(s->robots,     sizeof(Robot),  s->n_robots, f);
	fwrite(&s->n_bullets, sizeof(u32),    1,           f);
	
	u32 tmp = 0;
	FOREACH_BULLET
		tmp++;
		fwrite(&s->bullets[i], sizeof(Bullet), 1, f);
	DONE_BULLET
	
	fflush(f);
}

void Server_Loop(Server* s)
{
	assert(s);

	int fd_max = 0;
	for (u32 i = 0; i < s->game.n_clients; i++)
		if (s->client[i] > fd_max)
		fd_max = s->client[i];
	fd_max++;

	srandom(time(NULL));
	u32 curId = 0;
	for (u32 i = 0; i < s->game.n_clients; i++)
	{
		Robot nr =
		{
			curId++,
			random() % (u32)s->game.width,
			random() % (u32)s->game.height,
			73,
			116,
			deg2rad(random() % 360),
			0, 100.0, 0, 0, 0
		};
		// don't merge the two next lines: enableRobot maybe change s->robots pointer
		u32 id = enableRobot(s);    assert(i == id); // TODO
		Robot* r = &s->robots[id];
		memcpy(r, &nr, sizeof(Robot));
		write(s->client[i], r, sizeof(Robot));
	}
	for (u32 i = 0; i < s->game.n_clients; i++)
		write(s->client[i], &START_MESSAGE, sizeof(u8));


	int epollfd = epoll_create(s->game.n_clients);
	struct epoll_event  ev;
	for (u32 i = 0; i < s->game.n_clients; i++)
	{
		int flags = fcntl(s->client[i], F_GETFL, 0);
		fcntl(s->client[i], F_SETFL, flags | O_NONBLOCK);
		ev.events   = EPOLLIN;
		ev.data.u32 = i;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, s->client[i], &ev);
	}

	struct timeb last;
	struct timeb cur;
	ftime(&cur);
	struct epoll_event events[10];
	while (42)
	{
		int n_events = epoll_wait(epollfd, events, 10, 1000 / FRAMERATE);
		for (s32 i = 0; i < n_events; i++)
		{
			u32 client = events[i].data.u32;
			while (Server_HandleOrder(s, client));
		}

		last = cur;
		ftime(&cur);
		Server_Tick(s, (cur.time-last.time)+((float)(cur.millitm-last.millitm) / 1000));
		Server_Debug(s);
		Server_Dump(s, s->display_fh);
	}
}
