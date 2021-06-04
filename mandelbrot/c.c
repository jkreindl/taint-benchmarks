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

#define BENCHMARK_NAME "mandelbrot"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

static inline void doLoop(double *Zr, double *Zi, double *Cr, double *Ci,
                          double *Tr, double *Ti) {
  *Zi = 2.0 * (*Zr) * (*Zi) + (*Ci);
  *Zr = (*Tr) - (*Ti) + (*Cr);
  *Tr = (*Zr) * (*Zr);
  *Ti = (*Zi) * (*Zi);
}

static inline int shouldDoLoop(int i, int iter, double Tr, double Ti,
                               double limit) {
  if (i >= iter) {
    return false;
  }

  if ((Tr + Ti) > (limit * limit)) {
    return false;
  }

  return true;
}

int mandelbrot(int n) {
  int w, h, bit_num = 0;
  char byte_acc = 0;
  int i, iter = 50;
  double x, y, limit = 2.0;
  double Zr, Zi, Cr, Ci, Tr, Ti;

  w = h = n;

  int result = 0;

  for (y = 0; y < h; ++y) {
    for (x = 0; x < w; ++x) {
      Zi = __truffletaint_add_double(0.0);
      Zr = Ti = Tr = 0.0;
      Cr = (2.0 * x / w - 1.5);
      Ci = (2.0 * y / h - 1.0);

      for (i = 0; shouldDoLoop(i, iter, Tr, Ti, limit); ++i) {
        doLoop(&Zr, &Zi, &Cr, &Ci, &Tr, &Ti);
      }

      byte_acc <<= 1;
      if (Tr + Ti <= limit * limit) {
        byte_acc |= 0x01;
        if (__truffletaint_check_double(Tr) && __truffletaint_check_int(n))
          byte_acc = __truffletaint_add_char(byte_acc);
      }

      ++bit_num;

      if (bit_num == 8) {
        // putc(byte_acc, stdout);
        result += byte_acc;
        byte_acc = 0;
        bit_num = 0;
      } else if (x == w - 1) {
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

int benchmark() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    int n = 400;
    if (i & 0x11)
      n = __truffletaint_add_int(n);
    sum += mandelbrot(n);
  }
  __truffletaint_assert_int(sum);
  sum = __truffletaint_remove_int(sum);
  return sum;
}

int getExpectedResult() { return -56750; }

void setup(void *arg) {}
