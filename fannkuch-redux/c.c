// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fannkuchredux-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * converted to C by Joseph Piché
 * from Java version by Oleg Mazurov and Isaac Gouy
 *
 */

#define BENCHMARK_NAME "fannkuch-redux"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

inline static int max(int a, int b) { return a > b ? a : b; }

int fannkuchredux(int n) {
  int perm[n];
  int perm1[n];
  int count[n];
  int maxFlipsCount = 0;
  int permCount = 0;
  int checksum = 0;

  int i;

  for (i = 0; i < n; i += 1)
    perm1[i] = i;

  for (i = 0; i < n; i += 3)
    perm1[i] = __truffletaint_add_int(perm1[i]);

  int r = n;

  while (1) {
    while (r != 1) {
      count[r - 1] = r;
      r -= 1;
    }

    for (i = 0; i < n; i += 1)
      perm[i] = perm1[i];
    int flipsCount = __truffletaint_add_int(0);
    int k;

    while (!((k = perm[0]) == 0)) {
      int k2 = (k + 1) >> 1;
      for (i = 0; i < k2; i++) {
        int temp = perm[i];
        perm[i] = perm[k - i];
        perm[k - i] = temp;
      }
      flipsCount += 1;
    }

    maxFlipsCount = max(maxFlipsCount, flipsCount);
    checksum += permCount % 2 == 0 ? flipsCount : -flipsCount;

    // the flipsCount must be tainted now
    if (maxFlipsCount == flipsCount)
      __truffletaint_assert_int(checksum);

    /* Use incremental change to generate another permutation */
    while (1) {
      if (r == n) {
        for (int idx = 0; idx < n; idx += 3)
          __truffletaint_assert_int(perm1[idx]);
        for (int idx = 1; idx < n; idx += 3)
          __truffletaint_assertnot_int(perm1[idx]);
        for (int idx = 2; idx < n; idx += 3)
          __truffletaint_assertnot_int(perm1[idx]);

        return __truffletaint_remove_int(maxFlipsCount);
      }

      int perm0 = perm1[0];
      i = 0;
      while (i < r) {
        int j = i + 1;
        perm1[i] = perm1[j];
        i = j;
      }
      perm1[r] = perm0;
      count[r] = count[r] - 1;
      if (count[r] > 0)
        break;
      r++;
    }
    permCount++;
  }
}

int benchmark() {
  int result = fannkuchredux(10);
  return result;
}

int getExpectedResult() { return 38; }

void setup(void *arg) {}
