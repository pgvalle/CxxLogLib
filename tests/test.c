#include "../CxxLogLib.h"
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

void ms_delay(uint32_t milliseconds)
{
  // Calculate the target time
  uint32_t target = (uint32_t)(clock() * 1000 / CLOCKS_PER_SEC) + milliseconds;

  // Busy wait until the target time is reached
  while ((uint32_t)(clock() * 1000 / CLOCKS_PER_SEC) < target);
}

void* thread_func(void* arg)
{
  CLL_info("file not found");
  ms_delay(10);
  CLL_debug("file not found");
  ms_delay(10);
  CLL_warning("file not found");
  ms_delay(10);
  CLL_error("file not found");
  ms_delay(10);
  CLL_error("file not found");

  return NULL;
}

int main()
{
  CLL_init();
  CLL_setColors(true);

  pthread_t thread;
  pthread_create(&thread, NULL, thread_func, NULL);

  CLL_info("file not found");
  ms_delay(12);
  CLL_debug("file not found");
  ms_delay(12);
  CLL_error("file not found");
  ms_delay(12);
  CLL_error("file not found");

  // Wait for the thread to finish
  pthread_join(thread, NULL);

  CLL_quit();

  return 0;
}
