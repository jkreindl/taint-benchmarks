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

const char *JS_DO_LOOP =
    "("
    "function doLoop(ZrPtr, ZiPtr, CrPtr, CiPtr, TrPtr, TiPtr) {\n"
    "\n"
    "    var Zi = 2.0 * ZrPtr[0] * ZiPtr[0] + CiPtr[0];\n"
    "    ZiPtr[0] = Zi;\n"
    "\n"
    "    var Zr = TrPtr[0] - TiPtr[0] + CrPtr[0];"
    "    ZrPtr[0] = Zr;\n"
    "\n"
    "    var Tr = Zr * Zr;\n"
    "    TrPtr[0] = Tr;\n"
    "\n"
    "    var Ti = Zi * Zi;\n"
    "    TiPtr[0] = Ti;\n"
    "}\n"
    ")\n";

void (*doLoop)(void *, void *, void *, void *, void *, void *);

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

#define dArg(var) polyglot_from_double_array(&var, 1)

      for (i = 0; shouldDoLoop(i, iter, Tr, Ti, limit); ++i) {
        doLoop(dArg(Zr), dArg(Zi), dArg(Cr), dArg(Ci), dArg(Tr), dArg(Ti));
      }

#undef dArg

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

void setup(void *arg) { doLoop = polyglot_eval("js", JS_DO_LOOP); }

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
