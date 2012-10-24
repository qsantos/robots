#include "display.h"

#include <GL/glfw.h>
#include <SOIL/SOIL.h>
#include <unistd.h>

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

int winWidth  = 1024;
int winHeight = 768;
int x;
int y;

Display* Display_New(string IP, u16 port)
{
  if (!glfwInit())
    return NULL;
  if (!glfwOpenWindow(winWidth, winHeight, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
  {
    glfwTerminate();
    return NULL;
  }
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // two dimensionnal mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, winWidth, winHeight, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_DEPTH_TEST);

  glPointSize(3);

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

  Display* ret = ALLOC(Display, 1);
  ret->server = TCP_Connect(IP, port);
  if (ret->server < 0)
  {
    fprintf(stderr, "Could not connect to the server\n");
    return NULL;
  }
  ret->n_robots  = 0;
  ret->a_robots  = 0;
  ret->robot     = NULL;
  ret->n_bullets = 0;
  ret->a_bullets = 0;
  ret->bullet    = NULL;

  return ret;
}

void Display_Delete(Display* d)
{
  assert(d);
  free(d->bullet);
  free(d->robot);
  close(d->server);
  free(d);
  glfwTerminate();
}

void Display_Update(Display* d)
{
  assert(d);

  read(d->server, &d->game, sizeof(Game));

  u32 nn_robots;
  read(d->server, &nn_robots, sizeof(u32));
  if (nn_robots > d->a_robots)
  {
    d->a_robots = nn_robots;
    d->robot = REALLOC(d->robot, Robot, d->a_robots);
  }
  d->n_robots = nn_robots;
  read(d->server, d->robot, sizeof(Robot) * d->n_robots);

  u32 nn_bullets;
  read(d->server, &nn_bullets, sizeof(u32));
  if (nn_bullets > d->a_bullets)
  {
    d->a_bullets = nn_bullets;
    d->bullet = REALLOC(d->bullet, Bullet, d->a_bullets);
  }
  d->n_bullets = nn_bullets;
  read(d->server, d->bullet, sizeof(Bullet) * d->n_bullets);

  d->opened = glfwGetWindowParam(GLFW_OPENED);
}

void Display_Draw(Display* d)
{
  assert(d);

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0); // hack against pixel centered coordinates

  glfwGetMousePos(&x, &y);
  glTranslatef(winWidth / 2 - x, winHeight / 2 - y, 0);
  float zoom = pow(1.1, glfwGetMouseWheel());
  glScalef(zoom, zoom, zoom);

//  Texture_Draw(TEX_GROUND, d->game.width, d->game.height);

  for (u32 i = 0; i < d->n_robots; i++)
    Robot_Draw(&d->robot[i]);

  for (u32 i = 0; i < d->n_bullets; i++)
    Bullet_Draw(&d->bullet[i]);

  glfwSwapBuffers();
}

void Robot_Draw(Robot* r)
{
  assert(r);

  glPushMatrix();

  glTranslated(r->x, r->y, 0);
  glRotatef(rad2deg(r->angle), 0.0, 0.0, 1.0);
  Texture_Draw(TEX_CHASSIS, r->width, r->height);

  glTranslatef(0, 21, 0);
  glRotatef(rad2deg(r->gunAngle), 0, 0, 1);
  glTranslatef(0, -40, 0);
  Texture_Draw(TEX_GUN, tex_width[TEX_GUN], tex_height[TEX_GUN]);

  glPopMatrix();
}

void Bullet_Draw(Bullet* b)
{
  assert(b);

  glColor4f(1, 1, 1, 1);
  glBegin(GL_POINTS);
    glVertex2f(b->x, b->y);
  glEnd();
}

void Texture_Draw(u32 tex, float width, float height)
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
