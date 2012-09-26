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

#ifndef SOCKET_H
#define SOCKET_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <stdio.h>

#include "common.h"

FILE* TCP_Connect (const string, u16);
s32   TCP_ListenTo(const string, u16);
s32   TCP_Listen  (u16);
FILE* TCP_Accept  (u32);
void  TCP_Close   (u32);

#endif
