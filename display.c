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
#include <math.h>
#include <ctype.h>
#include <GL/glfw.h>
#include <SOIL/SOIL.h>

#include "comm.h"
#include "socket.h"

// texture information
enum
{
  TEX_GROUND = 0,
  TEX_CHASSIS,
  TEX_GUN,
  TEX_NB
};
const char* tex_name  [TEX_NB] = { "img/grass.png", "img/chassis.png", "img/gun.png" };
int         texture   [TEX_NB];
int         tex_width [TEX_NB];
int         tex_height[TEX_NB];
void drawTexture(int tex)
{
  glBindTexture(GL_TEXTURE_2D, texture[tex]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,            1.0);
  glVertex2f  (0,              0 );
  glTexCoord2f(1.0,            1.0);
  glVertex2f  (tex_width[tex], 0);
  glTexCoord2f(1.0,            0.0);
  glVertex2f  (tex_width[tex], tex_height[tex]);
  glTexCoord2f(0.0,            0.0);
  glVertex2f  (0,              tex_height[tex]);
  glEnd();
}

float width  = 1920;
float height = 1080;
int   x      = 0;
int   y      = 0;

void Robot_Display(Robot* r)
{
  glPushMatrix();
  
  glTranslated(r->x, r->y, 0);
  glRotatef(rad2deg(r->angle), 0.0, 0.0, 1.0);
  glTranslatef(-tex_width[TEX_CHASSIS]/2, -tex_width[TEX_CHASSIS]/2, 0);
  drawTexture(TEX_CHASSIS);
  
  glTranslatef(tex_width[TEX_CHASSIS]/2, 78, 0);
  glRotatef(rad2deg(r->gunAngle), 0, 0, 1);
  glTranslatef(-tex_width[TEX_GUN]/2, -100, 0);
  drawTexture(TEX_GUN);

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
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width[i]);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height[i]);
  }
  
  return 1;
}

void glDisplay(State* s)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0); // hack against pixel centered coordinates
  
  glfwGetMousePos(&x, &y);
  glTranslatef(width / 2 - x, height / 2 - y, 0);
  float zoom = pow(1.1, glfwGetMouseWheel());
  glScalef(zoom, zoom, zoom);

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
  // "background"
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
  // two dimensionnal mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);

  // we won't need this either
  glDisable(GL_DEPTH_TEST);

  // enables transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //textures
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
