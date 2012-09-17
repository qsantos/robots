#ifndef ROBOT_H
#define ROBOT_H

#include <stdio.h>

#include "utils.h"

typedef struct __attribute__((packed))
{
  double x;
  double y;
  double angle;
  double energy;
} Robot;

void   File_SendRobot(FILE*, Robot*);
Robot* File_GetRobot (FILE*);

typedef struct __attribute__((packed))
{
  u32    n_robots;
  Robot* robot;
} State;

void   File_SendState(FILE*, State*);
State* File_GetState (FILE*);

typedef enum
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    FIRE,
} Command_Type;

typedef struct __attribute__((packed))
{
  Command_Type type;
  double       amount;
} Command;

void     File_SendCommand(FILE*, Command*);
Command* File_GetCommand (FILE*);

#endif
