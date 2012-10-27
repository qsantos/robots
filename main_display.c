/*\
 *  This is an awesome programm simulating awesome batlles of awesome robot tanks
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
#include <GL/freeglut.h>
#include <fcntl.h>
#include <SOIL/SOIL.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <string.h>

#include "socket.h"
#include "game.h"

#define GLUT_KEY_ESC    (27)
#define GLUT_WHEEL_UP   (3)
#define GLUT_WHEEL_DOWN (4)

static inline float max(float a, float b)
{
	return a > b ? a : b;
}

// texture information
typedef enum
{
	TEX_GROUND,
	TEX_CHASSIS,
	TEX_GUN,
	TEX_EXPLOSION,
	TEX_NB,
} Texture;

const char* tex_name  [TEX_NB] = { "img/grass.png", "img/chassis.png", "img/gun.png", "img/explosion.png" };
int         texture   [TEX_NB];
int         tex_width [TEX_NB];
int         tex_height[TEX_NB];

int   winId;
int   winWidth  = 1024;
int   winHeight = 768;
int   mouseX    = 0;
int   mouseY    = 0;
float zoom      = 1;

struct timeb lastDraw;
s32     server;
Game    game;
u32     n_robots     = 0;
u32     a_robots     = 0;
Robot*  robots       = NULL;
u32     n_bullets    = 0;
u32     a_bullets    = 0;
Bullet* bullet       = NULL;

#define EXPLOSION_DURATION (1.0f)
typedef struct
{
	float x;
	float y;
	float curTime;
	float radius;
} Explosion;
u32        n_explosions      = 0;
u32        a_explosions      = 0;
u32*       active_explosions = NULL;
Explosion* explosions        = NULL;
#define FOREACH_EXPLOSION(I)                                                \
	{                                                                   \
		u32 I = 0;                                                  \
		for (u32 I##32 = 0; I##32 < a_explosions / 32 ; I##32++) \
		{                                                           \
			u32 bitfield = active_explosions[I##32];         \
			for (u32 I##b = 0; I##b < 32; I++, I##b++)          \
			{                                                   \
				if (bitfield % 2)                           \
				{
//					code (explosions[I] are active)
#define DONE_EXPLOSION                                                      \
				}                                           \
				bitfield >>= 1;                             \
			}                                                   \
		}                                                           \
	}

static inline u32 enableExplosion()
{
	if (n_explosions >= a_explosions)
	{
		if (a_explosions)
		{
			a_explosions *= 2;
			active_explosions = REALLOC(active_explosions, u32, a_explosions / 32);
			memset(&active_explosions[a_explosions/32/2], 0, a_explosions/32*sizeof(u32)/2);
		}
		else
		{
			a_explosions = 32;
			active_explosions = REALLOC(active_explosions, u32, a_explosions / 32);
			memset(active_explosions, 0, a_explosions/32*sizeof(u32));
		}
		explosions = REALLOC(explosions, Explosion, a_explosions);
	}
	u32 i32 = 0;
	while (active_explosions[i32] == (u32)-1)
		i32++;
	u32 bitfield = active_explosions[i32];
	u32 i = i32 * 32;
	while (bitfield % 2)
	{
		i++;
		bitfield >>= 1;
	}
	active_explosions[i/32] |= (1 << (i%32));
	n_explosions++;
	
	return i;
}
static inline void disableExplosion(u32 i)
{
	active_explosions[i/32] ^= (1 << (i%32));
	n_explosions--;
}

void drawTexture(Texture tex, float width, float height)
{
	glBindTexture(GL_TEXTURE_2D, texture[tex]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex2f(- width / 2, - height / 2);
			glTexCoord2f(1.0, 1.0); glVertex2f(  width / 2, - height / 2);
			glTexCoord2f(1.0, 0.0); glVertex2f(  width / 2,   height / 2);
			glTexCoord2f(0.0, 0.0); glVertex2f(- width / 2,   height / 2);
		glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
#define TEXT_BUFFER (1024)
#define TEXT_FONT   GLUT_STROKE_ROMAN
void drawRobot(Robot* r)
{
	assert(r);

	glPushMatrix();
		glTranslatef(r->x, r->y, 0);
		glRotatef(rad2deg(r->angle), 0.0, 0.0, 1.0);
		drawTexture(TEX_CHASSIS, r->width, r->height);
		glPushMatrix();
			glTranslatef(0, 21, 0);
			glRotatef(rad2deg(r->gunAngle), 0, 0, 1);
			glTranslatef(0, -40, 0);
			drawTexture(TEX_GUN, tex_width[TEX_GUN], tex_height[TEX_GUN]);
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();
		unsigned char glText[TEXT_BUFFER];
		snprintf((string)glText, TEXT_BUFFER, "%f", r->energy);
		float textWidth = glutStrokeLength(TEXT_FONT, glText);
		float maxdim = max(r->height, r->width);
		float scale = maxdim / textWidth;
		textWidth *= scale;
		
		glColor4f(1, 1, 1, 1);
		glTranslatef(r->x - textWidth / 2, r->y + maxdim, 0);
		glScalef(scale, -scale, scale);
		glRasterPos2i(0, 0);
		glutStrokeString(TEXT_FONT, glText);
	glPopMatrix();

}

void drawExplosion(u32 i)
{
	Explosion* e = &explosions[i];
	u32 step = (u32)(16 * e->curTime / EXPLOSION_DURATION);
	u32 row = 3 - (step / 4);
	u32 col = step % 4;
	glPushMatrix();
		glTranslatef(e->x, e->y, 0);
		glBindTexture(GL_TEXTURE_2D, texture[TEX_EXPLOSION]);
			glBegin(GL_QUADS);
#define W (1.0/4.0)
				glTexCoord2f(W*col,     W*(row+1)); glVertex2f(- e->radius, - e->radius);
				glTexCoord2f(W*(col+1), W*(row+1)); glVertex2f(  e->radius, - e->radius);
				glTexCoord2f(W*(col+1), W*row);     glVertex2f(  e->radius,   e->radius);
				glTexCoord2f(W*col,     W*row);     glVertex2f(- e->radius,   e->radius);
			glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void drawBullet(Bullet* b)
{
	assert(b);

	glColor4f(1, 1, 1, 1);
	glBegin(GL_POINTS);
		glVertex2f(b->x, b->y);
	glEnd();
}

void cb_displayFunc()
{
	struct timeb cur;
	ftime(&cur);
	float elapsed = (cur.time-lastDraw.time) + ((float)(cur.millitm-lastDraw.millitm)/1000);
	lastDraw = cur;
	
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.375, 0.375, 0); // hack against pixel centered coordinates

	glTranslatef(winWidth / 2 - mouseX, winHeight / 2 - mouseY, 0);
	glScalef(zoom, zoom, zoom);

	glPushMatrix();
		glTranslatef(game.width / 2, game.height / 2, 0);
		drawTexture(TEX_GROUND, game.width, game.height);
	glPopMatrix();

	FOREACH_EXPLOSION(i)
		drawExplosion(i);
		if ((explosions[i].curTime += elapsed) >= EXPLOSION_DURATION)
			disableExplosion(i);
	DONE_EXPLOSION

	for (u32 i = 0; i < n_robots; i++)
		drawRobot(&robots[i]);

	for (u32 i = 0; i < n_bullets; i++)
		drawBullet(&bullet[i]);
	
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}

void cb_idleFunc()
{
	u8 eventCode;
	if (read(server, &eventCode, sizeof(u8)) <= 0) return;
	
	Robot r;
	switch (eventCode)
	{
	case E_DUMP:
		read(server, &game, sizeof(Game));

		u32 nn_robots;
		read(server, &nn_robots, sizeof(u32));
		if (nn_robots > a_robots)
		{
			a_robots = nn_robots;
			robots = REALLOC(robots, Robot, a_robots);
		}
		n_robots = nn_robots;
		read(server, robots, sizeof(Robot) * n_robots);

		u32 nn_bullets;
		read(server, &nn_bullets, sizeof(u32));
		if (nn_bullets > a_bullets)
		{
			a_bullets = nn_bullets;
			bullet = REALLOC(bullet, Bullet, a_bullets);
		}
		n_bullets = nn_bullets;
		read(server, bullet, sizeof(Bullet) * n_bullets);
		break;
	case E_KABOUM:
		read(server, &r, sizeof(Robot));
		u32 i = enableExplosion();
		Explosion* e = &explosions[i];
		e->x       = r.x;
		e->y       = r.y;
		e->curTime = 0;
		e->radius  = (r.width + r.height) / 4;
	}
}

void cb_mouseFunc(int button, int state, int x, int y)
{
	(void) state;
	(void) x;
	(void) y;
	
	if (button == GLUT_WHEEL_UP)
		zoom *= 1.1;
	else if (button == GLUT_WHEEL_DOWN)
		zoom /= 1.1;
}

void cb_motionFunc(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void cb_passiveMotionFunc(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void glInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glPointSize(3);

	// two dimensionnal mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winWidth, winHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);

	// enables transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//textures
	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < TEX_NB; i++)
	{
		printf("Loading %s\n", tex_name[i]);
		texture[i] = SOIL_load_OGL_texture(tex_name[i], SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		if (!texture[i])
		abort();

		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width[i]);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height[i]);
	}
}

int main(int argc, char** argv)
{
	char* interface = "127.0.0.1";
	u16   port      = 4242;

	opterr = 0;
	int c;
	while ((c = getopt(argc, argv, "i:p:")) != -1)
		switch (c)
		{
		case 'i':
			interface = optarg;
			break;

		case 'p':
			port = (u32) atoi(optarg);
			break;

		case '?':
			if ((optopt == 'i') || (optopt == 'n') || (optopt == 'p'))
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			return 1;

		default:
			abort();
		}

	server = TCP_Connect(interface, port);
	if (server < 0)
	{
		fprintf(stderr, "Could not connect to the server\n");
		abort();
	}

	glutInit(&argc, argv);
	glutInitWindowSize(winWidth, winHeight);
	winId = glutCreateWindow("Robot battle");
	
	glutDisplayFunc      (&cb_displayFunc);
	glutIdleFunc         (&cb_idleFunc);
	glutMouseFunc        (&cb_mouseFunc);
	glutMotionFunc       (&cb_motionFunc);
	glutPassiveMotionFunc(&cb_passiveMotionFunc);

	ftime(&lastDraw);
	glInit();
	glutMainLoop();

	free(active_explosions);
	free(explosions);
	free(bullet);
	free(robots);
	close(server);
	return 0;
}
