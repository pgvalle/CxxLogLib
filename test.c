#include "CxxLogLib.h"

int main()
{
  CLL_init();

  CLL_info("file not found");
  CLL_debug("file not found");
  CLL_warn("file not found");
  CLL_error("file not found");
  CLL_fatal("file not found");

  return 0;
}
