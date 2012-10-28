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

#ifndef ALLOCATE_ARRAY_H
#define ALLOCATE_ARRAY_H

// In this file are grouped some macros which aims to simplify the use of
// dynamic bitfield-driven (allowing to release and reuse cells) arrays

// In general:
//   * P    is a prefix which says where data is found (e.g. P robots means P=P )
//   * TYPE is the type of the stored values
//   * NAME is the name of the variable or the parameter to be used
//   * I    is the variable name to use when iterating or finding a position

// give the definition headers of the necessary variables (or parameters)
#define DEF(TYPE, NAME)      \
	TYPE* NAME;          \
	u32*  active_##NAME; \
	u32   n_##NAME;      \
	u32   a_##NAME;

// initialize the values of these variables
#define INIT(P, NAME)           \
	P NAME          = NULL; \
	P active_##NAME = 0;    \
	P n_##NAME      = 0;    \
	P a_##NAME      = 0;

// free the variables used to describe the array
#define FREE(P, NAME)          \
	free(P NAME);          \
	free(P active_##NAME);

// any code used between FOREACH and DONE will be executed on avery active instance
#define FOREACH(P, NAME, I)                                    \
{                                                              \
	u32 I = 0;                                             \
	for (u32 I##32 = 0; I##32 < P a_##NAME / 32 ; I##32++) \
	{                                                      \
		u32 bitfield = P active_##NAME[I##32];         \
		for (u32 I##b = 0; I##b < 32; I++, I##b++)     \
		{                                              \
			if (bitfield % 2)                      \
			{
//				code (P NAME[I] is active)
#define DONE                                                   \
			}                                      \
			bitfield >>= 1;                        \
		}                                              \
	}                                                      \
}

// search for a new available memory cell in the array
// Note: the cell is marked as used, so it is assumed it will be used after this
#define ENABLE(P, TYPE, NAME, I)                                                                   \
{                                                                                                  \
	if (P n_##NAME >= P a_##NAME)                                                              \
	{                                                                                          \
		if (P a_##NAME)                                                                    \
		{                                                                                  \
			P a_##NAME *= 2;                                                           \
			P active_##NAME = REALLOC(P active_##NAME, u32, P a_##NAME / 32);          \
			memset(&P active_##NAME[P a_##NAME/32/2], 0, P a_##NAME/32*sizeof(u32)/2); \
		}                                                                                  \
		else                                                                               \
		{                                                                                  \
			P a_##NAME = 32;                                                           \
			P active_##NAME = REALLOC(P active_##NAME, u32, P a_##NAME / 32);          \
			memset(P active_##NAME, 0, P a_##NAME/32*sizeof(u32));                     \
		}                                                                                  \
		P NAME = REALLOC(P NAME, TYPE, P a_##NAME);                                        \
	}                                                                                          \
	u32 I##32 = 0;                                                                             \
	while (P active_##NAME[I##32] == (u32)-1)                                                  \
		I##32++;                                                                           \
	u32 bitfield = P active_##NAME[I##32];                                                     \
	I = I##32 * 32;                                                                            \
	while (bitfield % 2)                                                                       \
	{                                                                                          \
		I++;                                                                               \
		bitfield >>= 1;                                                                    \
	}                                                                                          \
	P active_##NAME[I/32] |= (1 << (I%32));                                                    \
	P n_##NAME++;                                                                              \
}

// free a cell in the array
#define DISABLE(P, NAME, I)                     \
{                                               \
	P active_##NAME[I/32] ^= (1 << (I%32)); \
	P n_##NAME--;                           \
}

#endif
