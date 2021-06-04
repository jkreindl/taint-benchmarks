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

int (*max)(int, int);

int *(*createArray)();
int (*getElement)(void *, int);
void (*setElement)(void *, int, int);

void setup(void *arg) {
  max = polyglot_eval(
      "js",
      "(function max(a, b) { if (a > b) { return a; } else { return b; } })");
  createArray = polyglot_eval("js", "(function createArray() { return []; })");
  getElement =
      polyglot_eval("js", "(function getElement(arr, i) { return arr[i]; })");
  setElement = polyglot_eval(
      "js", "(function setElement(arr, i, value) { arr[i] = value; })");
}

int fannkuchredux(int n) {
  int *perm = createArray();
  int *perm1 = createArray();
  int *count = createArray();
  int maxFlipsCount = 0;
  int permCount = 0;
  int checksum = 0;

  int i;

  for (i = 0; i < n; i += 1)
    setElement(perm1, i, i);

  for (i = 0; i < n; i += 3)
    setElement(perm1, i, __truffletaint_add_int(getElement(perm1, i)));

  int r = n;

  while (1) {
    while (r != 1) {
      setElement(count, r - 1, r);
      r -= 1;
    }

    for (i = 0; i < n; i += 1)
      setElement(perm, i, getElement(perm1, i));
    int flipsCount = __truffletaint_add_int(0);
    int k;

    while (!((k = getElement(perm, 0)) == 0)) {
      int k2 = (k + 1) >> 1;
      for (i = 0; i < k2; i++) {
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
    while (1) {
      if (r == n) {
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
      while (i < r) {
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

int benchmark() {
  int result = fannkuchredux(10);
  return result;
}

int getExpectedResult() { return 38; }
