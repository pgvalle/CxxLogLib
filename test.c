#include "CxxLogLib.h"

int main()
{
  CLL_init();

  FILE *f = fopen("file.txt", "w");
  CLL_assert(f, "file not found");
  CLL_setLoggingStream(f);

  CLL_debug("rljasdçlfjkasdçlf");
  CLL_inform("rljasdçlfjkasdasdfasdfasdçlf");

  fclose(f);
  return 0;
}
