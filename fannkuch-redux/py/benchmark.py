"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fannkuchredux-clang-1.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

converted to C by Joseph Piché
from Java version by Oleg Mazurov and Isaac Gouy
"""

from polyglot import import_value
benchmarkName = "fannkuch-redux"

Taint = import_value("taint")


def max(a, b):
    if (a > b):
        return a
    else:
        return b


def fannkuchredux(n):
    perm = []
    perm1 = []
    count = []
    maxFlipsCount = 0
    permCount = 0
    checksum = 0

    for i in range(n):
        perm1.append(i)
        perm.append(0)
        count.append(0)

    for i in range(0, n, 3):
        perm1[i] = Taint.add(perm1[i])

    r = n

    while True:
        while r != 1:
            count[r - 1] = r
            r -= 1

        for i in range(n):
            perm[i] = perm1[i]
        flipsCount = Taint.add(0)
        k = 0

        while True:
            k = perm[0]
            if (k == 0):
                break

            k2 = (k + 1) >> 1
            for i in range(k2):
                temp = perm[i]
                perm[i] = perm[k - i]
                perm[k - i] = temp

            flipsCount += 1
            Taint.assertTainted(flipsCount)

        maxFlipsCount = max(maxFlipsCount, flipsCount)
        if (permCount % 2 == 0):
            checksum += flipsCount
        else:
            checksum -= flipsCount

        if maxFlipsCount == flipsCount:
            Taint.assertTainted(checksum)

        while True:
            if (r == n):
                for idx in range(0, n, 3):
                    Taint.assertTainted(perm1[idx])
                for idx in range(1, n, 3):
                    Taint.assertNotTainted(perm1[idx])
                for idx in range(2, n, 3):
                    Taint.assertNotTainted(perm1[idx])

                return Taint.remove(maxFlipsCount)

            perm0 = perm1[0]
            i = 0
            while (i < r):
                j = i + 1
                perm1[i] = perm1[j]
                i = j

            perm1[r] = perm0
            count[r] = count[r] - 1
            if count[r] > 0:
                break
            r += 1

        permCount += 1


def benchmark():
    result = fannkuchredux(9)
    return result


def getExpectedResult():
    return 30


def test():
    actualResult = benchmark()
    if (actualResult == getExpectedResult()):
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
