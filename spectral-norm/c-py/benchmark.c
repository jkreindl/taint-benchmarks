// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/spectralnorm-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * Contributed by Sebastien Loisel
 */

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

double (*eval_A)(int, int);

void eval_A_times_u(int N, const double u[], double Au[])
{
  int i, j;
  for (i = __truffletaint_add_int(0); i < N; i++)
  {
    Au[i] = 0;
    for (j = 0; j < N; j++)
      Au[i] += eval_A(i, j) * u[j];
  }
}

void eval_At_times_u(int N, const double u[], double Au[])
{
  int i, j;
  for (i = 0; i < N; i++)
  {
    Au[i] = 0;
    for (j = 0; j < N; j++)
      Au[i] += eval_A(j, i) * u[j];
  }
}

void eval_AtA_times_u(int N, const double u[], double AtAu[])
{
  double v[N];
  eval_A_times_u(N, u, v);
  eval_At_times_u(N, v, AtAu);
}

void setup(void *arg) { eval_A = polyglot_eval("python", "def eval_A(i, j):\n"
                                                         "    return 1.0 / ((i + j) * (i + j + 1.0) / 2 + i + 1.0)\n"
                                                         "\n"
                                                         "eval_A\n"); }

double spectralNorm(int N)
{
  int i;
  double u[N], v[N], vBv, vv;
  for (i = 0; i < N; i++)
    u[i] = 1;
  for (i = 0; i < 10; i++)
  {
    eval_AtA_times_u(N, u, v);
    eval_AtA_times_u(N, v, u);
  }
  vBv = vv = 0;
  for (i = 0; i < N; i++)
  {
    vBv += u[i] * v[i];
    vv += v[i] * v[i];
  }

  double result = vBv + vv;

  __truffletaint_assert_double(result);
  result = __truffletaint_remove_double(result);

  return result;
}

double benchmark()
{
  double sum = 0;
  for (int i = 0; i < 10; i++)
    sum += spectralNorm(150);
  return sum;
}

double getExpectedResult() { return 12696259641.522675; }

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
         "spectral-norm"
         "\n");
  struct timeval start, end;
  for (unsigned int i = 0; i < iterations; i++)
  {
    gettimeofday(&start, ((void *)0));
    double result = benchmark();
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
