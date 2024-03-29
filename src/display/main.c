/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012-2013 Quentin SANTOS
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

#include <GL/freeglut.h>
#include <SOIL/SOIL.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#include "../socket.h"
#include "../game.h"
#include "../math.h"
#include "../allocateArray.h"
#include "simpleOGG.h"

#define GLUT_KEY_ESC    (27)
#define GLUT_WHEEL_UP   (3)
#define GLUT_WHEEL_DOWN (4)

// texture information
typedef enum
{
	TEX_GROUND,
	TEX_BACK,
	TEX_CHASSIS,
	TEX_GUN,
	TEX_EXPLOSION,
	TEX_NB,
} Texture;

static const char* tex_name  [TEX_NB] =
{
	"img/glass.png",
	"img/back.png",
	"img/chassis.png",
	"img/gun.png",
	"img/explosion.png",
};
static int texture   [TEX_NB];
// TODO: should not be used (currently for gun positionning)
static int tex_width [TEX_NB];
static int tex_height[TEX_NB];

static int   winId;
static int   winWidth  = 1024;
static int   winHeight = 768;
static int   mouseX    = 0;
static int   mouseY    = 0;

static float viewX = 0;
static float viewY = 0;
static float viewZoom  = 1;

static struct timespec lastDraw;
static int          server;
static Game         game;
static u32          n_robots    = 0;
static u32          a_robots    = 0;
static Robot*       robots      = NULL;
static RobotOrder*  robotOrders = NULL;
static u32          n_bullets   = 0;
static u32          a_bullets   = 0;
static Bullet*      bullets     = NULL;

#define EXPLOSION_DURATION (2.0f)
typedef struct
{
	float x;
	float y;
	float curTime;
	float radius;
} Explosion;
DEF(Explosion, explosions)

// TODO: read all robots, orderRobots or bullets in one time
static void handleEvent()
{
	EventCode eventCode;
	read(server, &eventCode, sizeof(EventCode));

	Robot  r;
	Bullet b;
	u32    u1;
	u32    u2;
	switch (eventCode)
	{
	case E_TICK:
		break;
	case E_DUMP:
		read(server, &game, sizeof(Game));

		u32 nn_robots;
		read(server, &nn_robots, sizeof(u32));
		if (nn_robots > a_robots)
		{
			a_robots = nn_robots;
			robots      = REALLOC(robots,      Robot,      a_robots);
			robotOrders = REALLOC(robotOrders, RobotOrder, a_robots);
		}
		n_robots = nn_robots;
		for (u32 i = 0; i < n_robots; i++)
			read(server, &robots[i], sizeof(Robot));
		for (u32 i = 0; i < n_robots; i++)
			read(server, &robotOrders[i], sizeof(RobotOrder));

		u32 nn_bullets;
		read(server, &nn_bullets, sizeof(u32));
		if (nn_bullets > a_bullets)
		{
			a_bullets = nn_bullets;
			bullets = REALLOC(bullets, Bullet, a_bullets);
		}
		n_bullets = nn_bullets;
		for (u32 i = 0; i < n_bullets; i++)
			read(server, &bullets[i], sizeof(Bullet));

		break;
	case E_ROBOT:
		read(server, &u1, sizeof(u32));
		read(server, &r,  sizeof(Robot));
		break;
	case E_BULLET:
		read(server, &u1, sizeof(u32));
		read(server, &b,  sizeof(Bullet));
		break;
	case E_HIT:
		read(server, &u1, sizeof(u32));
		read(server, &b,  sizeof(Bullet));
		read(server, &u2, sizeof(u32));
		break;
	case E_HITBY:
		read(server, &u1, sizeof(u32));
		read(server, &b,  sizeof(Bullet));
		break;
	case E_HITROBOT:
		read(server, &u1, sizeof(u32));
		read(server, &u2, sizeof(u32));
		break;
	case E_HITWALL:
		read(server, &u1, sizeof(u32));
		break;
	case E_KABOUM:
		read(server, &r,  sizeof(Robot));

		u32 i;
		ENABLE(, Explosion, explosions, i);
		Explosion* e = &explosions[i];
		e->x       = r.x;
		e->y       = r.y;
		e->curTime = 0;
		e->radius  = (r.width + r.height) / 4;

		AL_Play("music/explosion.ogg", 0);
		break;
	}
}

void* listener(void* params)
{
	(void) params;

	while (true)
		handleEvent();
	return NULL;
}

static void drawRepeatTexture(Texture tex, float width, float height, float nx, float ny)
{
	glBindTexture(GL_TEXTURE_2D, texture[tex]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, ny);  glVertex2f(- width / 2, - height / 2);
			glTexCoord2f(nx,  ny);  glVertex2f(  width / 2, - height / 2);
			glTexCoord2f(nx,  0.0); glVertex2f(  width / 2,   height / 2);
			glTexCoord2f(0.0, 0.0); glVertex2f(- width / 2,   height / 2);
		glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
static inline void drawTexture(Texture tex, float width, float height)
{
	drawRepeatTexture(tex, width, height, 1, 1);
}
#define TEXT_BUFFER (1024)
#define TEXT_FONT   GLUT_STROKE_ROMAN
static void drawRobot(Robot* r)
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
		snprintf((char*)glText, TEXT_BUFFER, "%f", r->energy);
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

static void drawExplosion(u32 i)
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

static void drawBullet(Bullet* b)
{
	assert(b);

	glColor4f(1, 1, 1, 1);
	glBegin(GL_POINTS);
		glVertex2f(b->x, b->y);
	glEnd();
}

static void cb_displayFunc()
{
	struct timespec cur;
	clock_gettime(CLOCK_REALTIME, &cur);
	float elapsed = (cur.tv_sec-lastDraw.tv_sec) + ((float)(cur.tv_nsec-lastDraw.tv_nsec)/1e9f);
	lastDraw = cur;

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.375, 0.375, 0); // hack against pixel centered coordinates

	glTranslatef(winWidth / 2, winHeight / 2, 0);
	glScalef(viewZoom, viewZoom, viewZoom);
	glTranslatef(-viewX, -viewY, 0);

	glPushMatrix();
		glTranslatef(game.width / 2, game.height / 2, 0);
		drawTexture      (TEX_BACK,   game.width, game.height);
		drawRepeatTexture(TEX_GROUND, game.width, game.height, game.width/100, game.height/100);
	glPopMatrix();

	FOREACH(, explosions, i)
		drawExplosion(i);
		if ((explosions[i].curTime += elapsed) >= EXPLOSION_DURATION)
			DISABLE(, explosions, i);
	DONE

	for (u32 i = 0; i < n_robots; i++)
		drawRobot(&robots[i]);

	for (u32 i = 0; i < n_bullets; i++)
		drawBullet(&bullets[i]);

	glPopMatrix();

	glPushMatrix();
		glTranslatef(5, 5, 0);
		glColor4ub(127, 200, 200, 127);
		glBegin(GL_QUADS);
			glVertex2f(0,   0);
			glVertex2f(100, 0);
			glVertex2f(100, 100);
			glVertex2f(0,   100);
		glEnd();

		glTranslatef(10, 24, 0);
		glColor4f(1, 1, 1, 1);
		static const float scale = 0.1;
		glScalef(scale, -scale, scale);
		glRasterPos2i(0, 0);
		for (u32 i = 0; i < n_robots; i++)
		{
			unsigned char glText[TEXT_BUFFER]; // TODO : check the macro
			snprintf((char*)glText, TEXT_BUFFER, "#%lu: %.2f\n", robots[i].id, robots[i].energy);
			glutStrokeString(TEXT_FONT, glText);
		}
	glPopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

static void cb_mouseFunc(int button, int state, int x, int y)
{
	(void) state;
	(void) x;
	(void) y;

	if (button == GLUT_WHEEL_UP)
		viewZoom *= 1.1;
	else if (button == GLUT_WHEEL_DOWN)
		viewZoom /= 1.1;
}

static void cb_motionFunc(int x, int y)
{
	if (mouseX == x && mouseY == y)
		return;

	viewX += (x - mouseX) / viewZoom;
	viewY += (y - mouseY) / viewZoom;
	mouseX = winWidth / 2;
	mouseY = winHeight / 2;
	glutWarpPointer(mouseX, mouseY);
}

static void cb_passiveMotionFunc(int x, int y)
{
	cb_motionFunc(x, y);
}

static void glInit()
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
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width[i]);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height[i]);
	}
}

// threaded sound start
int*   g_argc;
char** g_argv;
static void* soundStart(void* param)
{
	(void) param;

	AL_Init(g_argc, g_argv);
	AL_Play("music/waiting.ogg", 1);

	return NULL;
}

static void usage(int argc, char** argv)
{
	(void) argc;

	fprintf(stderr,
		"Usage: %s [ADDRESS [PORT]]\n"
		"\n"
		"  ADDRESS  the address of the game server\n"
		"  PORT     the port to connect to\n"
		,
		argv[0]
	);
}

int main(int argc, char** argv)
{
	g_argc = &argc;
	g_argv = argv;

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

	glutInit(&argc, argv);
	glutInitWindowSize(winWidth, winHeight);
	winId = glutCreateWindow("Robot battle");
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc      (&cb_displayFunc);
	glutMouseFunc        (&cb_mouseFunc);
	glutMotionFunc       (&cb_motionFunc);
	glutPassiveMotionFunc(&cb_passiveMotionFunc);

	INIT(, explosions);
	clock_gettime(CLOCK_REALTIME, &lastDraw);

	// server socket listening
	pthread_t listenerThread;
	pthread_create(&listenerThread, NULL, listener, NULL);

	// sound playing
	pthread_t soundThread;
	pthread_create(&soundThread, NULL, soundStart, NULL);

	glInit();
	glutMainLoop();

	printf("Terminating\n");

	// stop sound
	pthread_cancel(soundThread);
	pthread_join(soundThread, NULL);
	AL_Exit();

	// stop listening
	pthread_cancel(listenerThread);
	pthread_join(listenerThread, NULL);

	FREE(, explosions);
	free(bullets);
	free(robotOrders);
	free(robots);
	close(server);
	return 0;
}
