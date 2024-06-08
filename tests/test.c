#include "CxxLogLib.h"
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

void msDelay(uint32_t milliseconds) {
  // Calculate the target time
  uint32_t target = (uint32_t)(clock() * 1000 / CLOCKS_PER_SEC) + milliseconds;

  // Busy wait until the target time is reached
  while ((uint32_t)(clock() * 1000 / CLOCKS_PER_SEC) < target);
}

void* threadFunction(void* arg)
{
  CLL_info("file not found");
  msDelay(1);
  CLL_debug("file not found");
  CLL_warning("file not found");
  CLL_error("file not found");

  return NULL;
}

int main()
{
  CLL_init();
  CLL_setColors(true);

  pthread_t thread;
  pthread_create(&thread, NULL, threadFunction, NULL);

  CLL_info("file not found");
  CLL_debug("file not found");
  msDelay(1);
  CLL_warning("file not found");
  msDelay(2);
  CLL_error("file not found");

  // Wait for the thread to finish
  pthread_join(thread, NULL);

  CLL_quit();

  return 0;
}
