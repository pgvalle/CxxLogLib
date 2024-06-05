#include "CxxLogLib.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef _WIN32
#include <windows.h>
#endif

static const char *COLORS[] = {
  "\033[35m",
  "\033[31m",
  "\033[33m",
  "\033[33m",
  "\033[32m"
};

static const char *LEVELS[] = {
  " INFO  ", " DEBUG ", " WARN  ", " ERROR ", " FATAL "
};

static FILE *__stream;
static bool __colors;

void CLL_init()
{
  __stream = stdout;
  __colors = false;

  // windows bullshit to make ansi escape sequences work
#ifndef _WIN32
  const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD dwMode;
  GetConsoleMode(hOut, &dwMode);

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}

void CLL_setLoggingStream(FILE *stream)
{
  __stream = stream;
}

void CLL_setColors(bool colors)
{
  __colors = colors;
}

void __CLL_log(enum CLL_LogLevel level, const char *func, int line,
               const char *file, const char *format, ...)
{
  time_t t;
  char timeString[25] = "";
  time(&t);
  memcpy(timeString, ctime(&t), 24);

  if (__colors)
  {
    fprintf(__stream, "%s [%s%s\033[0m] %s:%d:%s -> ", timeString,
            COLORS[level], LEVELS[level], file, line, func);
  }
  else
  {
    fprintf(__stream, "%s [%s] %s:%d:%s -> ", timeString,
            COLORS[level], LEVELS[level], file, line, func);
  }

  va_list args;
  va_start(args, format);
  vfprintf(__stream, format, args);
  va_end(args);

  fprintf(__stream, "\n");
  fflush(__stream);

  if (level == CLL_LOG_FATAL)
  {
    exit(-1);
  }
}
