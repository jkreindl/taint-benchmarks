#ifndef TAINT_SHOOTOUTS_COMMON_H
#define TAINT_SHOOTOUTS_COMMON_H

#if defined(__cplusplus)

extern "C"
{

#else

#define true 1
#define false 0

#endif // defined(__cplusplus)

#include <stdio.h>
#include <stdlib.h>

#define BENCHMARK_RESULT_TYPE_INT 1
#define BENCHMARK_RESULT_TYPE_LONG 2
#define BENCHMARK_RESULT_TYPE_FLOAT 3
#define BENCHMARK_RESULT_TYPE_DOUBLE 4

// import all taint intrinsics
#include <taint.h>

// include graalvm polyglot support
#include <graalvm/llvm/polyglot.h>

// ensure the benchmark name is defined
#ifndef BENCHMARK_NAME
#error BENCHMARK_NAME is not defined
#endif // BENCHMARK_NAME

#ifndef BENCHMARK_RESULT_TYPE
#error BENCHMARK_RESULT_TYPE is not defined
#endif // BENCHMARK_RESULT_TYPE

#if BENCHMARK_RESULT_TYPE == BENCHMARK_RESULT_TYPE_INT
#define BENCHMARK_RESULT_PRINT_MODIFIER "%d"
#define BENCHMARK_RESULT_TYPE_NAME int
#elif BENCHMARK_RESULT_TYPE == BENCHMARK_RESULT_TYPE_LONG
#define BENCHMARK_RESULT_PRINT_MODIFIER "%d"
#define BENCHMARK_RESULT_TYPE_NAME long
#elif BENCHMARK_RESULT_TYPE == BENCHMARK_RESULT_TYPE_FLOAT
#define BENCHMARK_RESULT_PRINT_MODIFIER "%.9g"
#define BENCHMARK_RESULT_TYPE_NAME float
#elif BENCHMARK_RESULT_TYPE == BENCHMARK_RESULT_TYPE_DOUBLE
#define BENCHMARK_RESULT_PRINT_MODIFIER "%.17g"
#define BENCHMARK_RESULT_TYPE_NAME double
#else
#error BENCHMARK_RESULT_TYPE has invalid value
#endif // BENCHMARK_RESULT_TYPE

  void setup(void *);
  BENCHMARK_RESULT_TYPE_NAME benchmark();
  BENCHMARK_RESULT_TYPE_NAME getExpectedResult();

  void *createDefaultBenchmarkArg(void)
  {
    // the BenchmarkIO is supplied by the benchmark harness to make slowdown of file access uniform across languages
    return polyglot_import("benchmarkIO");
  }

#define BENCHMARK_IO_OPEN_INPUT_FILE(ioObj) \
  polyglot_invoke(ioObj, "openInputFile")

#define BENCHMARK_IO_CAN_READ_LINE(ioObj) \
  polyglot_as_boolean(polyglot_invoke(ioObj, "canReadLine"))

#define __UTF_8_CHARACTER_BYTE_SIZE 4

#define BENCHMARK_IO_READ_LINE(ioObj, tmp, tmpLen, buf, bufSize, bytesWritten) \
  tmp = polyglot_invoke(ioObj, "readLine");                                    \
  tmpLen = polyglot_get_string_size(tmp);                                      \
  if (bufSize < ((tmpLen + 1) * __UTF_8_CHARACTER_BYTE_SIZE))                  \
  {                                                                            \
    bufSize = ((tmpLen + 1) * __UTF_8_CHARACTER_BYTE_SIZE);                    \
    if (buf)                                                                   \
    {                                                                          \
      free(buf);                                                               \
    }                                                                          \
    buf = (char *)calloc(bufSize, sizeof(char));                               \
  }                                                                            \
  bytesWritten = polyglot_as_string(tmp, buf, bufSize, "UTF-8");               \
  tmp = (void *)0L;                                                            \
  tmpLen = 0L;

#define BENCHMARK_IO_READ_LINE_CONST_BUFFER(ioObj, tmp, buf, bufSize, \
                                            bytesRead)                \
  tmp = polyglot_invoke(ioObj, "readLine");                           \
  bytesRead = polyglot_get_string_size(tmp);                          \
  bytesRead = polyglot_as_string(tmp, buf, bufSize, "UTF-8");         \
  tmp = (void *)0L;

#define BENCHMARK_IO_PEEK(ioObj) \
  polyglot_as_i32(polyglot_invoke(ioObj, "peekNextChar"))

#define BENCHMARK_IO_CLOSE_INPUT_FILE(ioObj) \
  polyglot_invoke(ioObj, "closeInputFile")

#define BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj) \
  polyglot_invoke(ioObj, "openOutputFile")

#define BENCHMARK_IO_WRITE(ioObj, buf, len) \
  polyglot_invoke(ioObj, "write", polyglot_from_string_n(buf, len, "UTF-8"))

#define BENCHMARK_IO_WRITE_CHAR(ioObj, ch) polyglot_invoke(ioObj, "write", ch)

#define BENCHMARK_IO_WRITE_NUMBER(ioObj, number) \
  polyglot_invoke(ioObj, "writeNumber", number)

#define BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj) \
  polyglot_invoke(ioObj, "closeOutputFile")

#include <sys/time.h>
  int main(int argc, char **argv)
  {
    setup(createDefaultBenchmarkArg());

    printf("starting benchmark: " BENCHMARK_NAME "\n");
    struct timeval start, end;
    for (unsigned int i = 0; i < 20000; i++)
    {
      gettimeofday(&start, NULL);
      BENCHMARK_RESULT_TYPE_NAME result = benchmark();
      gettimeofday(&end, NULL);
      double s = (double)(end.tv_usec - start.tv_usec) / 1000000 +
                 (double)(end.tv_sec - start.tv_sec);
      printf(
          "iteration %d took %f seconds and gave " BENCHMARK_RESULT_PRINT_MODIFIER
          "\n",
          i, s, result);
    }
    return 0;
  }

#undef BENCHMARK_RESULT_TYPE_NAME
#undef BENCHMARK_RESULT_PRINT_MODIFIER
#undef BENCHMARK_RESULTS_MATCH

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // TAINT_SHOOTOUTS_COMMON_H
