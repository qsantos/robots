#ifndef SOCKET_H
#define SOCKET_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <stdio.h>

#include "utils.h"

FILE* TCP_Connect (const string, u16);
u32   TCP_ListenTo(const string, u16);
u32   TCP_Listen  (u16);
FILE* TCP_Accept  (u32);
void  TCP_Close   (u32);

#endif
