"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/binarytrees-clang-1.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

contributed by Kevin Carson
compilation:
   gcc -O3 -fomit-frame-pointer -funroll-loops -static binary-trees.c -lm
   icc -O3 -ip -unroll -static binary-trees.c -lm

   *reset*
"""

from polyglot import import_value
from math import floor
from math import pow
benchmarkName = "binarytrees"

Taint = import_value("taint")


def shouldBeTainted(level):
    return (level & 0b11) == 0b11


class treeNode():
    pass


def getItem(level):
    item = 1
    if (shouldBeTainted(level)):
        item = Taint.add(item)
    return item


def NewTreeNode(level, left, right):
    node = treeNode()

    node.left = left
    node.right = right
    node.item = getItem(level)

    return node


def ItemCheck(tree):
    result = 0
    result += tree.item
    if (tree.left):
        result += ItemCheck(tree.left)
        result += ItemCheck(tree.right)
    return result


def BottomUpTree(depth):
    node = None
    if (depth > 0):
        node = NewTreeNode(depth, BottomUpTree(depth - 1),
                           BottomUpTree(depth - 1))
    else:
        node = NewTreeNode(depth, None, None)
    return node


def DeleteTree(depth, tree):
    if (tree.left):
        DeleteTree(depth - 1, tree.left)
        DeleteTree(depth - 1, tree.right)

    item = tree.item

    if (shouldBeTainted(depth)):
        Taint.assertTainted(item)
        tree.item = None
    else:
        Taint.assertNotTainted(item)


def benchmark():

    N = 0
    depth = 0
    minDepth = 0
    maxDepth = 0
    stretchDepth = 0

    stretchTree = None
    longLivedTree = None
    tempTree = None

    N = 14

    minDepth = 4

    if (minDepth + 2) > N:
        maxDepth = minDepth + 2
    else:
        maxDepth = N

    stretchDepth = maxDepth + 1

    result = 0
    check = 0

    stretchTree = BottomUpTree(stretchDepth)
    check = ItemCheck(stretchTree)
    Taint.assertTainted(check)
    result += check

    DeleteTree(stretchDepth, stretchTree)

    longLivedTree = BottomUpTree(maxDepth)

    for depth in range(minDepth, maxDepth + 1, 2):

        iterations = pow(2, maxDepth - depth + minDepth)
        iterations = floor(iterations)

        check = 0

        for i in range(1, iterations + 1):
            tempTree = BottomUpTree(depth)

            curCheck = ItemCheck(tempTree)

            if depth >= 0b11:
                Taint.assertTainted(curCheck)
            else:
                Taint.assertNotTainted(curCheck)

            check += curCheck
            DeleteTree(depth, tempTree)

        if (iterations > 0) and (depth >= 0b11):
            Taint.assertTainted(check)
        else:
            Taint.assertNotTainted(check)

        result += check

        check = 0

    check = ItemCheck(longLivedTree)
    result += check

    DeleteTree(maxDepth, longLivedTree)
    Taint.assertTainted(result)
    result = Taint.remove(result)

    return result


def getExpectedResult():
    return 3222190


def test():
    actualResult = benchmark()
    if actualResult == getExpectedResult():
        return 0
    else:
        return 1


def setup(arg):
    pass


try:
    assert isinstance(benchmarkName, str), "'benchmarkName' is invalid"
except NameError:
    raise AssertionError("'benchmark' is not defined")

try:
    assert callable(benchmark), "'benchmark' is not callable"
except NameError:
    raise AssertionError("'benchmark' is not defined")


def main():
    from polyglot import import_value
    benchmarkIO = import_value("benchmarkIO")
    setup(benchmarkIO)

    import time
    print("starting benchmark: " + benchmarkName)
    for iteration in range(0x7FFFFFFF):
        start = time.time()
        result = benchmark()
        end = time.time()
        s = end - start
        print("iteration " + str(iteration) + " took " +
              str(s) + " seconds and gave " + str(result))

    return 0


main()
