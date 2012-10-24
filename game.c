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

#include "game.h"

inline static float dot(float x0, float y0, float x1, float y1)
{
  return x0*x1 + y0*y1;
}
bool RobotCollidePoint(Robot* r, float x, float y)
{
  float vx = x - r->x;
  float vy = y - r->y;
  float wx =   r->width  / 2 * cos(r->angle);
  float wy =   r->width  / 2 * sin(r->angle);
  float hx = - r->height / 2 * sin(r->angle);
  float hy =   r->height / 2 * cos(r->angle);
  bool a = fabs(dot(vx, vy, wx, wy)) <= dot(wx, wy, wx, wy);
  bool b = fabs(dot(vx, vy, hx, hy)) <= dot(hx, hy, hx, hy);
  return a && b;
}

bool RobotCollideRobot(Robot* a, Robot* b)
{
  float wx =   b->width  / 2 * cos(b->angle);
  float wy =   b->width  / 2 * sin(b->angle);
  float hx = - b->height / 2 * sin(b->angle);
  float hy =   b->height / 2 * cos(b->angle);
  bool tl = RobotCollidePoint(a, b->x - wx - hx, b->y - wy - hy);
  bool tr = RobotCollidePoint(a, b->x + wx - hx, b->y + wy - hy);
  bool bl = RobotCollidePoint(a, b->x - wx + hx, b->y - wy + hy);
  bool br = RobotCollidePoint(a, b->x + wx + hx, b->y + wy + hy);
  return tl && tr && bl && br;
}
