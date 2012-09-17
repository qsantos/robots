#include "comm.h"

#include <assert.h>

void File_SendRobot(FILE* f, Robot* r)
{
  assert(r);
  fwrite(r, sizeof(Robot), 1, f);
}

Robot* File_GetRobot(FILE* f)
{
  Robot* r = ALLOC(Robot, 1);
  fread(r, sizeof(Robot), 1, f);
  return r;
}

void File_SendState(FILE* f, State* s)
{
  assert(s);
  fwrite(&s->n_robots, sizeof(u32),   1,           f);
  fwrite(s->robot,     sizeof(Robot), s->n_robots, f);
}

State* File_GetState(FILE* f)
{
  State* s = ALLOC(State, 1);
  fread(&s->n_robots, sizeof(u32), 1, f);
  s->robot = ALLOC(Robot, s->n_robots);
  fread(s->robot, sizeof(Robot), s->n_robots, f);
  return s;
}

void File_SendCommand(FILE* f, Command* cmd)
{
  assert(cmd);
  fwrite(cmd, sizeof(Command), 1, f);
}

Command* File_GetCommand(FILE* f)
{
  Command* cmd = ALLOC(Command, 1);
  fread(cmd, sizeof(Command), 1, f);
  return cmd;
}
