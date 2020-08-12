// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/mandelbrot-clang-2.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   contributed by Greg Buchholz

   for the debian (AMD) machine...
   compile flags:  -O3 -ffast-math -march=athlon-xp -funroll-loops

   for the gp4 (Intel) machine...
   compile flags:  -O3 -ffast-math -march=pentium4 -funroll-loops
*/

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

static inline void doLoop(double *Zr, double *Zi, double *Cr, double *Ci,
                          double *Tr, double *Ti)
{
  *Zi = 2.0 * (*Zr) * (*Zi) + (*Ci);
  *Zr = (*Tr) - (*Ti) + (*Cr);
  *Tr = (*Zr) * (*Zr);
  *Ti = (*Zi) * (*Zi);
}

static inline int shouldDoLoop(int i, int iter, double Tr, double Ti,
                               double limit)
{
  if (i >= iter)
  {
    return 0;
  }

  if ((Tr + Ti) > (limit * limit))
  {
    return 0;
  }

  return 1;
}

int mandelbrot(int n)
{
  int w, h, bit_num = 0;
  char byte_acc = 0;
  int i, iter = 50;
  double x, y, limit = 2.0;
  double Zr, Zi, Cr, Ci, Tr, Ti;

  w = h = n;

  int result = 0;

  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x)
    {
      Zi = __truffletaint_add_double(0.0);
      Zr = Ti = Tr = 0.0;
      Cr = (2.0 * x / w - 1.5);
      Ci = (2.0 * y / h - 1.0);

      for (i = 0; shouldDoLoop(i, iter, Tr, Ti, limit); ++i)
      {
        doLoop(&Zr, &Zi, &Cr, &Ci, &Tr, &Ti);
      }

      byte_acc <<= 1;
      if (Tr + Ti <= limit * limit)
      {
        byte_acc |= 0x01;
        if (__truffletaint_check_double(Tr) && __truffletaint_check_int(n))
          byte_acc = __truffletaint_add_char(byte_acc);
      }

      ++bit_num;

      if (bit_num == 8)
      {
        // putc(byte_acc, stdout);
        result += byte_acc;
        byte_acc = 0;
        bit_num = 0;
      }
      else if (x == w - 1)
      {
        byte_acc <<= (8 - w % 8);
        // putc(byte_acc, stdout);
        result += byte_acc;
        byte_acc = 0;
        bit_num = 0;
      }

      Zi = Zr = Ti = Tr = Ci = Cr = 0.0;
    }
  }

  return result;
}

int benchmark()
{
  int sum = 0;
  for (int i = 0; i < 10; i++)
  {
    int n = 250;
    if (i & 0x11)
      n = __truffletaint_add_int(n);
    sum += mandelbrot(n);
  }
  __truffletaint_assert_int(sum);
  sum = __truffletaint_remove_int(sum);
  return sum;
}

int getExpectedResult() { return -59230; }

void setup(void *arg) {}

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
         "mandelbrot"
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
