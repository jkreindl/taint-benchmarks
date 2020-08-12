"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/mandelbrot-clang-2.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

contributed by Greg Buchholz

for the debian (AMD) machine...
compile flags: -O3 -ffast-math -march=athlon-xp -funroll-loops

for the gp4 (Intel) machine...
compile flags: -O3 -ffast-math -march=pentium4 -funroll-loops
"""

from polyglot import import_value
benchmarkName = "mandelbrot"

Taint = import_value("taint")


class LoopBodyData():
    pass


def doLoop(bodyData):
    bodyData.Zi = 2.0 * bodyData.Zr * bodyData.Zi + bodyData.Ci
    bodyData.Zr = bodyData.Tr - bodyData.Ti + bodyData.Cr
    bodyData.Tr = bodyData.Zr * bodyData.Zr
    bodyData.Ti = bodyData.Zi * bodyData.Zi


def shouldDoLoop(i, bodyData, limit):
    if i >= 50:
        return False

    if (bodyData.Tr + bodyData.Ti) > (limit * limit):
        return False

    return True


def mandelbrot(n):
    bit_num = 0
    byte_acc = 0
    limit = 2.0

    w = n
    h = n

    result = 0

    for y in range(h):
        for x in range(w):
            bodyData = LoopBodyData()
            bodyData.Zi = Taint.add(0.0)
            bodyData.Cr = (2.0 * x / w - 1.5)
            bodyData.Ci = (2.0 * y / h - 1.0)

            bodyData.Zr = 0.0
            bodyData.Ti = 0.0
            bodyData.Tr = 0.0

            i = 0
            while shouldDoLoop(i, bodyData, limit):
                doLoop(bodyData)
                i += 1

            byte_acc <<= 1
            if (bodyData.Tr + bodyData.Ti) <= (limit * limit):
                byte_acc |= 1
                if Taint.check(bodyData.Tr):
                    if Taint.check(n):
                        byte_acc = Taint.add(byte_acc)

            bit_num += 1

            if (bit_num == 8):
                result += byte_acc
                byte_acc = 0
                bit_num = 0
            elif (x == (w - 1)):
                byte_acc <<= (8 - w % 8)
                result += byte_acc
                byte_acc = 0
                bit_num = 0

    return result


def benchmark():
    sum = 0
    for i in range(10):
        n = 250
        if i & 17:
            n = Taint.add(n)
        sum += mandelbrot(n)

    Taint.assertTainted(sum)
    sum = Taint.remove(sum)
    return sum


def getExpectedResult():
    """
    in c, byte_acc is a `char`, and
    overflows a couple of times in the
    benchmark, while this does not
    happen in js. as a result,
    the result value differs in c and js
    """
    return 7927970


def test():
    actualResult = mandelbrot(250)
    if (actualResult != 792797):
        return 1

    actualResult = mandelbrot(Taint.add(250))

    Taint.assertTainted(actualResult)

    if (actualResult != 792797):
        return 1
    return 0


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
