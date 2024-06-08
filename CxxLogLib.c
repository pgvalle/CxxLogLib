#include "CxxLogLib.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>

#ifndef _WIN32
#include <windows.h>
#endif

static struct
{
  bool initialized;

  FILE *stream;
  bool colors;

  pthread_t mainThread;
  pthread_mutex_t logMutex;
} _;

void CLL_init()
{
  // Call CLL_init only once. To call it again, call CLL_quit before.
  if (_.initialized)
  {
    return;
  }

  _.initialized = true;

  _.stream = stdout;
  _.colors = false;

  // windows bullshit to make ansi escape sequences work
#ifndef _WIN32
  const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD dwMode;
  GetConsoleMode(hOut, &dwMode);

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif

  _.mainThread = pthread_self();
  pthread_mutex_init(&_.logMutex, NULL);
}


void CLL_quit()
{
  if (_.initialized && _.mainThread == pthread_self())
  {
    _.initialized = false;
    pthread_mutex_destroy(&_.logMutex);
  }
}


void CLL_setLogStream(FILE *stream)
{
  if (_.initialized && _.mainThread == pthread_self())
  {
    _.stream = stream;
  }
}


void CLL_setColors(bool colors)
{
  if (_.initialized && _.mainThread == pthread_self())
  {
    _.colors = colors;
  }
}


static const char *LOG_TYPES[] = {
  "INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
};

static const char *LOG_TYPES_COLORS[] = {
  "\033[32m",
  "\033[36m",
  "\033[33m",
  "\033[31m",
  "\033[35m"
};

void __CLL_log(enum CLL_LogType type, const char *func, int line,
               const char *file, const char *format, ...)
{
  const time_t t = time(NULL);
  struct tm *lt = localtime(&t);

  pthread_mutex_lock(&_.logMutex);  // writing to _.stream is critical

  // time, date and code info
  fprintf(_.stream, "[%02dh%02dm%02ds %4d.%02d.%02d] [%s:%d in %s] ",
          lt->tm_hour, lt->tm_min, lt->tm_sec,
          lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
          file, line, func);
  
  // Log type
  if (_.colors)
  {
    fprintf(_.stream, "[%s%s\033[0m]: ",
            LOG_TYPES_COLORS[type], LOG_TYPES[type]);
  }
  else
  {
    fprintf(_.stream, "[%s]: ", LOG_TYPES[type]);
  }

  va_list args;
  va_start(args, format);
  vfprintf(_.stream, format, args);
  va_end(args);

  fprintf(_.stream, "\n");
  fflush(_.stream);

  pthread_mutex_unlock(&_.logMutex);  // done writting to _.stream

  if (type == CLL_FATAL)
  {
    exit(-1);
  }
}
