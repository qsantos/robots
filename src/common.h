/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Quentin SANTOS
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

#include <stdlib.h>
#include <stdio.h>

inline void licence()
{
	printf(
		"robots Copyright (C) 2012-2013 Quentin Santos\n"
		"This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n"
		"This is free software, and you are welcome to redistribute it\n"
		"under certain conditions; type `show c' for details.\n"
	);
}

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

#endif
