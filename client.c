#include "socket.h"
#include "comm.h"

int main(void)
{
  FILE* toto = TCP_Connect("127.0.0.1", 4242);

  /* TEST */
  //fprintf(toto, "YEAAAAAAAAAAAAAAAAAAH\n");
  Robot bugs_bunny[] =
  {
    { 42.0, 0.0, 1.0, 100.0 },
    { 36.0, 1.0, 0.0, 26.0  },
  };
  State state = { 2, bugs_bunny };
  
  File_SendState(toto, &state);

  fclose(toto);

  return 0;
}
