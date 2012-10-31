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

#ifndef COMMON_H 
#define COMMON_H 

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* TYPES */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed   char  s8;
typedef signed   short s16;
typedef signed   long  s32;

static const u8 false = 0;
static const u8 true  = 1;
typedef u8 bool;


/* MEMORY MANAGEMENT */

inline void* mrealloc(void* ptr, size_t size, const char* file, unsigned int line)
{
	void* ret = realloc(ptr, size);
	if (!ret)
	{
		fprintf(stderr, "mrealloc failed at %s:%u\n", file, line);
		exit(1);
	}
	return ret;
}

#define MALLOC(T,N)      (T*) mrealloc(NULL, sizeof(T) * (N), __FILE__, __LINE__)
#define REALLOC(PTR,T,N) (T*) mrealloc(PTR,  sizeof(T) * (N), __FILE__, __LINE__)


/* TRIGONOMETRY */

#define PI (3.14159265358979323846)

// positive float mod
inline float pfmod(float x, float m)
{
	return m ? x - m*floor(x/m) : x;
}
// centered float mod
inline float cfmod(float x, float m)
{
	return pfmod(x+m/2, m)-m/2;
}

// maps angle to [-PI, PI[
inline float normRad(float a)
{
	return cfmod(a, 2*PI);
}
// maps angle to [-180, 180[
inline float normDeg(float a)
{
	return cfmod(a, 360);
}

// angle conversion
inline float deg2rad(float a)
{
	return normRad(a * PI / 180.0);
}
inline float rad2deg(float a)
{
	return normDeg(a * 180.0 / PI);
}

#endif
