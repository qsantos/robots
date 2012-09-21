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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <GL/glfw.h>
#include <SOIL/SOIL.h>

#include "comm.h"
#include "socket.h"

enum
{
  TEX_GROUND = 0,
  TEX_TANK,
  TEX_NB
};

int texture[TEX_NB];

const char* tex_name[] = { "img/grass.png", "img/tank.png" };

#define ROBOT_DIM 150

int width  = 1024;
int height = 768;

void Robot_Display(Robot* r)
{
  glPushMatrix();
  glTranslated(r->x, r->y, 0);
  glRotatef(-r->angle, 0.0, 0.0, 1.0);
  glTranslatef(-ROBOT_DIM/2, -ROBOT_DIM/2, 0);

  glBindTexture(GL_TEXTURE_2D, texture[TEX_TANK]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,       1.0);
  glVertex2f  (0,         0 );
  glTexCoord2f(1.0,       1.0);
  glVertex2f  (ROBOT_DIM, 0);
  glTexCoord2f(1.0,       0.0);
  glVertex2f  (ROBOT_DIM, ROBOT_DIM);
  glTexCoord2f(0.0,       0.0);
  glVertex2f  (0,         ROBOT_DIM);
  glEnd();

  glPopMatrix();
}

int load_textures(void)
{
  int i;
  for (i = 0; i < TEX_NB; i++)
  {
    printf("Loading %s\n", tex_name[i]);
    texture[i] = SOIL_load_OGL_texture(tex_name[i], SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
  
    if (!texture[i])
      abort();
  
    glBindTexture(GL_TEXTURE_2D, texture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  
  return 1;
}

void glDisplay(State* s)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Display background */
  glBindTexture(GL_TEXTURE_2D, texture[TEX_GROUND]);

  glBegin(GL_QUADS);

    glTexCoord2f(0.0,       1.0       );
    glVertex2f  (0,         0         );

    glTexCoord2f(1.0,       1.0       );
    glVertex2f  (width - 1, 0         );

    glTexCoord2f(1.0,       0.0       );
    glVertex2f  (width - 1, height - 1);

    glTexCoord2f(0.0,       0.0       );

    glVertex2f  (0,         height - 1);
  glEnd();

  for (u32 i = 0; i < s->n_robots; i++)
    Robot_Display(&s->robot[i]);
}

void glInit(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, 0, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0);

  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glEnable(GL_TEXTURE_2D);
  load_textures();
}

int main(int argc, char** argv)
{
  int running = GL_TRUE;

  if (!glfwInit())
    exit(EXIT_FAILURE);

  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glInit();

  char* interface = "127.0.0.1";
  u32   port      = 4242;

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
  
  FILE* server = TCP_Connect(interface, port);
  if (!server)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return 1;
  }

  while (running)
  {
    State* s = State_Get(server);
    glDisplay(s);
    free(s->bullet);
    free(s->robot);
    free(s);

    glfwSwapBuffers();
    running = glfwGetWindowParam(GLFW_OPENED);
  }
  
  fclose(server);
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
