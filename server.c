#include "socket.h"
#include "comm.h"

int main(void)
{
  u32 socket = TCP_ListenTo("127.0.0.1", 4242);
  
  while (42)
  {
    FILE* in = TCP_Accept(socket);

    /* DEBUG */
/*    char c = fgetc(in);
    while (!feof(in))
    {
      putchar(c);
      c = fgetc(in);
      }*/
    State* s = File_GetState(in);
    printf("%lu\n", s->n_robots);
    for (u32 i = 0; i < s->n_robots; i++)
    {
      printf("x:      %f\n", s->robot[i].x);
      printf("y:      %f\n", s->robot[i].y);
      printf("angle:  %f\n", s->robot[i].angle);
      printf("energy: %f\n", s->robot[i].energy);
    }

    fclose(in);
  }

  TCP_Close(socket);

  return 0;
}
