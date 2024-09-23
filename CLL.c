#include "CLL.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>

// ansi escape sequences don't work in cmd by default
void fixWindowsConsole()
{
  HANDLE handle;
  DWORD options;

  handle = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(handle, &options);
  SetConsoleMode(handle, options | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

  handle = GetStdHandle(STD_ERROR_HANDLE);
  GetConsoleMode(handle, &options);
  SetConsoleMode(handle, options | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

static struct
{
  bool initialized;
  FILE *stream;
  bool colors;
  pthread_t mainThread;
  pthread_mutex_t logLock;
} _;

bool _isMainThread()
{
  return _.mainThread == pthread_self();
}

void CLL_init()
{
  if (_.initialized)
    return;

  _.initialized = true;
  _.stream = stdout;
  _.colors = false;
  _.mainThread = pthread_self();
  pthread_mutex_init(&_.logLock, NULL);

#ifdef _WIN32
  fixWindowsConsole();
#endif
}

void CLL_terminate()
{
  if (_.initialized && _isMainThread())
  {
    _.initialized = false;
    pthread_mutex_destroy(&_.logLock);
  }
}

void CLL_setStream(FILE *stream)
{
  if (_.initialized && _isMainThread())
    _.stream = stream;
}

void CLL_setColors(bool colors)
{
  if (_.initialized && _isMainThread())
    _.colors = colors;
}

const char *__getLogTypeStr(enum CLL_LogType type)
{
  static const char *TYPES[] = {
    "INFO", "DEBUG", "WARNING", "ERROR"
  };

  static const char *COLORED_TYPES[] = {
    "\033[32mINFO\033[0m", "\033[36mDEBUG\033[0m",
    "\033[33mWARNING\033[0m", "\033[31mERROR\033[0m"
  };

  if (_.colors)
    return COLORED_TYPES[type];

  return TYPES[type];
}

void __logMetaInfo(enum CLL_LogType type, const char *func)
{
  const time_t t = time(NULL);
  const struct tm *lt = localtime(&t);

  fprintf(
    _.stream,
    "[ %4d-%02d-%02d %02d:%02d:%02d %s ",
    lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
    lt->tm_hour, lt->tm_min, lt->tm_sec,
    func);

  fprintf(_.stream, "%s ] ", __getLogTypeStr(type));
}

void __CLL_log(enum CLL_LogType type, const char *func, const char *fmt, ...)
{
  if (!_.initialized)
    return;

  pthread_mutex_lock(&_.logLock);  // entire log function should be atomic
  
  __logMetaInfo(type, func);

  // log message itself
  va_list args;
  va_start(args, fmt);
  vfprintf(_.stream, fmt, args);
  va_end(args);

  fprintf(_.stream, "\n");

  pthread_mutex_unlock(&_.logLock);  // Now we're safe
}
