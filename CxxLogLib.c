#include "CxxLogLib.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

static const char *LEVEL_STRINGS[] = {
  "INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
};

static FILE *__stream;

void CLL_init()
{
  __stream = stdout;

  // windows bullshit to make ansi escape sequences work
}

void CLL_setLoggingStream(FILE *stream)
{
  __stream = stream;
}

void __CLL_log(enum CLL_LogLevel level, const char *func, int line,
               const char *file, const char *format, ...)
{
  const char *levelString = LEVEL_STRINGS[(int)level];

  time_t t;
  char tStr[25] = "";
  time(&t);
  memcpy(tStr, ctime(&t), 24);

  fprintf(__stream, "%s | %-7s | %s:%d:%s -> ",
          tStr, levelString, file, line, func);

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
