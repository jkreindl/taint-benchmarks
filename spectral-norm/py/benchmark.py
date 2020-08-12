"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/spectralnorm-node-1.html##

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

contributed by Ian Osgood
modified for Node.js by Isaac Gouy
"""

from polyglot import import_value
benchmarkName = "spectral-norm"

Taint = import_value("taint")


def A(i, j):
    return 1 / ((i + j) * (i + j + 1) / 2 + i + 1)


def Au(u, v):
    for i in range(Taint.add(len(u))):
        t = 0
        for j in range(len(u)):
            t += A(i, j) * u[j]
        v[i] = t


def Atu(u, v):
    for i in range(len(u)):
        t = 0
        for j in range(len(u)):
            t += A(j, i) * u[j]
        v[i] = t


def AtAu(u, v, w):
    Au(u, w)
    Atu(w, v)


def spectralnorm(n):
    u = []
    v = []
    w = []

    for i in range(n):
        u.append(0)
        v.append(0)
        w.append(0)

    vv = 0
    vBv = 0
    for i in range(n):
        u[i] = 1
        v[i] = w[i] = 0

    for i in range(10):
        AtAu(u, v, w)
        AtAu(v, u, w)

    for i in range(n):
        vBv += u[i] * v[i]
        vv += v[i] * v[i]

    result = vBv + vv

    Taint.assertTainted(result)
    result = Taint.remove(result)

    return result


def benchmark():
    sum = 0
    for i in range(10):
        sum += spectralnorm(150)

    return sum


def getExpectedResult():
    return 12696259641.522675


def test():
    actualResult = spectralnorm(150)
    expectedResult = 1269625964.1522675
    if actualResult == expectedResult:
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
