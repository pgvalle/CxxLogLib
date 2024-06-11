#include "../CxxLogLib.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

// Function to be executed by each thread
void thread_func(const char *text, int num_calls, int delay_ms)
{
  for (int i = 0; i < num_calls; ++i)
  {
    CLL_log((CLL_LogType)(rand() % 4), text);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
  }
}

int main()
{
  CLL_init();
  CLL_set_colors(true);

  CLL_warning("hello");

  // Create two threads
  std::thread thread1(thread_func, "Thread 1", 7, 10);
  std::thread thread2(thread_func, "Thread 2", 5, 12);

  // Wait for both threads to finish
  thread1.join();
  thread2.join();

  CLL_quit();

  std::cout << "Both threads have finished executing." << std::endl;
  return 0;
}
