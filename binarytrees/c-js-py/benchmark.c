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

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

struct tn;
typedef struct tn treeNode;

struct tn
{
  treeNode *left;
  treeNode *right;
  unsigned item;
};
const char *PYTHON_CODE_GET_ITEM =
    "from polyglot import import_value\n"
    "Taint = import_value(\"taint\")\n"
    "\n"
    "def getItem(level):\n"
    "    item = 1\n"
    "    if (level & 0b11) == 0b11:\n"
    "        item = Taint."
    "add"
    "(item)\n"
    "    return item\n"
    "\n"
    "getItem\n";

const char *JS_CODE_CHECK_ITEM =
    "const Taint = Polyglot.import(\"taint\");"
    "function checkItem(item, level) {\n"
    "    if ((level & 0b11) == 0b11) {\n"
    "        Taint."
    "assertTainted"
    "(item);\n"
    "    }\n"
    "}\n"
    "(checkItem)\n";

unsigned (*getItem)(unsigned level);
void (*checkItem)(unsigned item, unsigned level);

treeNode *NewTreeNode(unsigned level, treeNode *left, treeNode *right)
{
  treeNode *new;

  new = (treeNode *)malloc(sizeof(treeNode));

  new->left = left;
  new->right = right;
  new->item = getItem(level);

  return new;
} /* NewTreeNode() */

long ItemCheck(treeNode *tree)
{
  long result = 0L;
  result += tree->item;
  if (tree->left != ((void *)0))
  {
    result += ItemCheck(tree->left);
    result += ItemCheck(tree->right);
  }
  return result;
} /* ItemCheck() */

treeNode *BottomUpTree(unsigned depth)
{
  if (depth > 0)
    return NewTreeNode(depth, BottomUpTree(depth - 1), BottomUpTree(depth - 1));
  else
    return NewTreeNode(depth, ((void *)0), ((void *)0));
} /* BottomUpTree() */

void DeleteTree(unsigned depth, treeNode *tree)
{
  if (tree->left != ((void *)0))
  {
    DeleteTree(depth - 1, tree->left);
    DeleteTree(depth - 1, tree->right);
  }

  unsigned item = tree->item;

  checkItem(tree->item, depth);
  tree->item = 0u;

  free(tree);
} /* DeleteTree() */

long benchmark()
{
  unsigned N, depth, minDepth, maxDepth, stretchDepth;
  treeNode *stretchTree, *longLivedTree, *tempTree;

  N = 14;

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

  for (depth = minDepth; depth <= maxDepth; depth += 2)
  {
    long i, iterations;

    iterations = pow(2, maxDepth - depth + minDepth);

    check = 0;

    for (i = 1; i <= iterations; i++)
    {
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

void setup(void *arg)
{
  getItem = polyglot_eval("python", PYTHON_CODE_GET_ITEM);
  checkItem = polyglot_eval("js", JS_CODE_CHECK_ITEM);
}

long getExpectedResult() { return 3222190; }

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
         "binarytrees"
         "\n");
  struct timeval start, end;
  for (unsigned int i = 0; i < iterations; i++)
  {
    gettimeofday(&start, ((void *)0));
    long result = benchmark();
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
