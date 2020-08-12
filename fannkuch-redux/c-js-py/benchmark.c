// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fannkuchredux-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * converted to C by Joseph Piché
 * from Java version by Oleg Mazurov and Isaac Gouy
 *
 */

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

int (*max)(int, int);

int *(*createArray)(int);
int (*getElement)(void *, int);
void (*setElement)(void *, int, int);

void setup(void *arg)
{
  max = polyglot_eval("python", "def max(a, b):\n"
                                "    if a > b:\n"
                                "        return a\n"
                                "    return b\n"
                                "\n"
                                "max\n");
  createArray = polyglot_eval("js", "function createArray(n) {\n"
                                    "    var arr = [];\n"
                                    "    for (var i = 0; i < n; i++) {\n"
                                    "        arr.push(0);\n"
                                    "    }"
                                    "    return arr;\n"
                                    "}\n"
                                    "(createArray)\n");
  getElement = polyglot_eval("python", "def getElement(arr, i):\n"
                                       "    return arr[i]\n"
                                       "\n"
                                       "getElement\n");
  setElement = polyglot_eval("python", "def setElement(arr, i, value):\n"
                                       "    arr[i] = value\n"
                                       "\n"
                                       "setElement\n");
}

int fannkuchredux(int n)
{
  int *perm = createArray(n);
  int *perm1 = createArray(n);
  int *count = createArray(n);
  int maxFlipsCount = 0;
  int permCount = 0;
  int checksum = 0;

  int i;

  for (i = 0; i < n; i += 1)
    setElement(perm1, i, i);

  for (i = 0; i < n; i += 3)
    setElement(perm1, i, __truffletaint_add_int(getElement(perm1, i)));

  int r = n;

  while (1)
  {
    while (r != 1)
    {
      setElement(count, r - 1, r);
      r -= 1;
    }

    for (i = 0; i < n; i += 1)
      setElement(perm, i, getElement(perm1, i));
    int flipsCount = __truffletaint_add_int(0);
    int k;

    while (!((k = getElement(perm, 0)) == 0))
    {
      int k2 = (k + 1) >> 1;
      for (i = 0; i < k2; i++)
      {
        int temp = getElement(perm, i);
        setElement(perm, i, getElement(perm, k - i));
        setElement(perm, k - i, temp);
      }
      flipsCount += 1;
    }

    maxFlipsCount = max(maxFlipsCount, flipsCount);
    checksum += permCount % 2 == 0 ? flipsCount : -flipsCount;

    // the flipsCount must be tainted now
    if (maxFlipsCount == flipsCount)
      __truffletaint_assert_int(checksum);

    /* Use incremental change to generate another permutation */
    while (1)
    {
      if (r == n)
      {
        for (int idx = 0; idx < n; idx += 3)
          __truffletaint_assert_int(getElement(perm1, idx));
        for (int idx = 1; idx < n; idx += 3)
          __truffletaint_assertnot_int(getElement(perm1, idx));
        for (int idx = 2; idx < n; idx += 3)
          __truffletaint_assertnot_int(getElement(perm1, idx));

        return __truffletaint_remove_int(maxFlipsCount);
      }

      int perm0 = getElement(perm1, 0);
      i = 0;
      while (i < r)
      {
        int j = i + 1;
        setElement(perm1, i, getElement(perm1, j));
        i = j;
      }
      setElement(perm1, r, perm0);
      setElement(count, r, getElement(count, r) - 1);
      if (getElement(count, r) > 0)
        break;
      r++;
    }
    permCount++;
  }
}

int benchmark()
{
  int result = fannkuchredux(9);
  return result;
}

int getExpectedResult() { return 30; }

void *createDefaultBenchmarkArg()
{
  return polyglot_import("benchmarkIO");
}

#include <sys/time.h>
int main(int argc, char **argv)
{
  setup(createDefaultBenchmarkArg());

  unsigned int iterations = 0xFFFFFFFF;
  if (argc == 2)
  {
    int explicitIterations = atoi(argv[1]);
    if (explicitIterations < 0)
    {
      fprintf(stderr, "invalid iteration count: " + explicitIterations);
      exit(1);
    }
    else
    {
      iterations = explicitIterations;
    }
  }

  printf("starting benchmark: "
         "fannkuch-redux"
         "\n");
  struct timeval start, end;
  for (unsigned int i = 0; i < iterations; i++)
  {
    gettimeofday(&start, ((void *)0));
    int result = benchmark();
    gettimeofday(&end, ((void *)0));
    double s = (double)(end.tv_usec - start.tv_usec) / 1000000 +
               (double)(end.tv_sec - start.tv_sec);
    printf(
        "iteration %d took %f seconds and gave "
        "%f"
        "\n",
        i, s, result);
  }
  return 0;
}
