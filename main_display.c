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
enum
{
	TEX_GROUND,
	TEX_CHASSIS,
	TEX_GUN,
	TEX_NB
};
const char* tex_name  [TEX_NB] = { "img/grass.png", "img/chassis.png", "img/gun.png" };
int         texture   [TEX_NB];
int         tex_width [TEX_NB];
int         tex_height[TEX_NB];

int winId;
int winWidth  = 1024;
int winHeight = 768;
int mouseX    = 0;
int mouseY    = 0;
float zoom    = 1;

s32     server;

Game    game;
u32     n_robots  = 0;
u32     a_robots  = 0;
Robot*  robot     = NULL;
u32     n_bullets = 0;
u32     a_bullets = 0;
Bullet* bullet    = NULL;

void drawTexture(u32 tex, float width, float height)
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
		glTranslated(r->x, r->y, 0);
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
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.375, 0.375, 0); // hack against pixel centered coordinates

	glTranslatef(winWidth / 2 - mouseX, winHeight / 2 - mouseY, 0);
	glScalef(zoom, zoom, zoom);

//  drawTexture(TEX_GROUND, game.width, game.height);

	for (u32 i = 0; i < n_robots; i++)
		drawRobot(&robot[i]);

	for (u32 i = 0; i < n_bullets; i++)
		drawBullet(&bullet[i]);
	
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}

void cb_idleFunc()
{
	if (read(server, &game, sizeof(Game)) <= 0) return;

	u32 nn_robots;
	if (read(server, &nn_robots, sizeof(u32)) <= 0) return;
	if (nn_robots > a_robots)
	{
		a_robots = nn_robots;
		robot = REALLOC(robot, Robot, a_robots);
	}
	n_robots = nn_robots;
	if (n_robots && read(server, robot, sizeof(Robot) * n_robots) <= 0) return;

	u32 nn_bullets;
	if (read(server, &nn_bullets, sizeof(u32)) <= 0) return;
	if (nn_bullets > a_bullets)
	{
		a_bullets = nn_bullets;
		bullet = REALLOC(bullet, Bullet, a_bullets);
	}
	n_bullets = nn_bullets;
	if (n_bullets && read(server, bullet, sizeof(Bullet) * n_bullets) <= 0) return;
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
//  int flags = fcntl(server, F_GETFL, 0);
//  fcntl(server, F_SETFL, flags | O_NONBLOCK);

	glutInit(&argc, argv);
	glutInitWindowSize(winWidth, winHeight);
	winId = glutCreateWindow("Robot battle");
	
	glutDisplayFunc      (&cb_displayFunc);
	glutIdleFunc         (&cb_idleFunc);
	glutMouseFunc        (&cb_mouseFunc);
	glutMotionFunc       (&cb_motionFunc);
	glutPassiveMotionFunc(&cb_passiveMotionFunc);

	glInit();
	glutMainLoop();

	free(bullet);
	free(robot);
	close(server);
	return 0;
}
