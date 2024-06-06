#include "CxxLogLib.h"
#include <time.h>
#include <stdlib.h>

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}

int main()
{
  CLL_init();
  CLL_setColors(false);

  FILE *logf = CLL_createLogFile("hellolog");
  CLL_createLogFile("aaa");
  delay(1);
  CLL_createLogFile("lkajsd");

  CLL_info("file not found");
  CLL_debug("file not found");
  CLL_warn("file not found");
  CLL_error("file not found");
  CLL_fatal("file not found");

  return 0;
}
