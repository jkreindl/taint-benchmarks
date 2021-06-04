// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/binarytrees-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   contributed by Kevin Carson
   compilation:
       gcc -O3 -fomit-frame-pointer -funroll-loops -static binary-trees.c -lm
       icc -O3 -ip -unroll -static binary-trees.c -lm

   *reset*
*/

#define BENCHMARK_NAME "binarytrees"
#define BENCHMARK_RESULT_TYPE 2
#include "../common.h"

#include <math.h>
#include <stdlib.h>

#define shouldBeTainted(level) ((level & 0b11) == 0b11)

struct tn;
typedef struct tn treeNode;

struct tn {
  treeNode *left;
  treeNode *right;
  unsigned item;
};

const char *JS_CODE_GET_ITEM =
    "var Taint = Polyglot.import(\"taint\");\n"
    "\n"
    "(\n"
    "function getItem(level) {\n"
    "    if ((level & 0b11) == 0b11) {"
    "        var tainted = Taint.addTaint(1);\n"
    "        return tainted;\n"
    "    } else {\n"
    "        return 1;\n"
    "    }\n"
    "}\n"
    ")\n";

unsigned (*getItem)(unsigned level);

treeNode *NewTreeNode(unsigned level, treeNode *left, treeNode *right) {
  treeNode *new;

  new = (treeNode *)malloc(sizeof(treeNode));

  new->left = left;
  new->right = right;
  new->item = getItem(level);

  return new;
} /* NewTreeNode() */

long ItemCheck(treeNode *tree) {
  long result = 0L;
  result += tree->item;
  if (tree->left != NULL) {
    result += ItemCheck(tree->left);
    result += ItemCheck(tree->right);
  }
  return result;
} /* ItemCheck() */

treeNode *BottomUpTree(unsigned depth) {
  if (depth > 0)
    return NewTreeNode(depth, BottomUpTree(depth - 1), BottomUpTree(depth - 1));
  else
    return NewTreeNode(depth, NULL, NULL);
} /* BottomUpTree() */

void DeleteTree(unsigned depth, treeNode *tree) {
  if (tree->left != NULL) {
    DeleteTree(depth - 1, tree->left);
    DeleteTree(depth - 1, tree->right);
  }

  unsigned item = tree->item;

  if (shouldBeTainted(depth)) {
    __truffletaint_assert_unsigned_int(item);
    tree->item = 0u;
  } else {
    __truffletaint_assertnot_unsigned_int(item);
  }

  free(tree);
} /* DeleteTree() */

long benchmark() {
  unsigned N, depth, minDepth, maxDepth, stretchDepth;
  treeNode *stretchTree, *longLivedTree, *tempTree;

  N = 15;

  minDepth = 4;

  if ((minDepth + 2) > N)
    maxDepth = minDepth + 2;
  else
    maxDepth = N;

  stretchDepth = maxDepth + 1;

  long result = 0;
  long check = 0;

  stretchTree = BottomUpTree(stretchDepth);
  check = ItemCheck(stretchTree);
  __truffletaint_assert_long(check);
  result += check;

  DeleteTree(stretchDepth, stretchTree);

  longLivedTree = BottomUpTree(maxDepth);

  for (depth = minDepth; depth <= maxDepth; depth += 2) {
    long i, iterations;

    iterations = pow(2, maxDepth - depth + minDepth);

    check = 0;

    for (i = 1; i <= iterations; i++) {
      tempTree = BottomUpTree(depth);

      long curCheck = ItemCheck(tempTree);

      if (depth >= 0b11)
        __truffletaint_assert_long(curCheck);
      else
        __truffletaint_assertnot_long(curCheck);

      check += curCheck;
      DeleteTree(depth, tempTree);
    } /* for(i = 1...) */

    if ((iterations > 0) && (depth >= 0b11))
      __truffletaint_assert_long(check);
    else
      __truffletaint_assertnot_long(check);

    result += check;

    check = 0;
  } /* for(depth = minDepth...) */

  check = ItemCheck(longLivedTree);
  result += check;

  DeleteTree(maxDepth, longLivedTree);
  __truffletaint_assert_long(result);
  result = __truffletaint_remove_long(result);

  return result;
} /* benchmark() */

void setup(void *arg) { getItem = polyglot_eval("js", JS_CODE_GET_ITEM); }

long getExpectedResult() { return 6444382; }
