#include "CxxLogLib.h"
#include <time.h>
#include <stdlib.h>

int main()
{
  CLL_init();
  CLL_setColors(false);
  
  FILE *logf = CLL_createLogFile("hellolog");
  CLL_setLogStream(logf);

  CLL_info("file not found");
  CLL_debug("file not found");
  CLL_warning("file not found");
  CLL_error("file not found");
  CLL_fatal("file not found");

  fclose(logf);

  return 0;
}
