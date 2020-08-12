"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fasta-clang-1.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

by Paul Hsieh
"""

from polyglot import import_value
from math import floor
benchmarkName = "fasta"

Taint = import_value("taint")


def writeFastaHeader(id, desc):
    pass


def writeFasta(str, fromIndex):

    for i in range(fromIndex, len(str)):
        ch = str[i]

        if ch == 'G':
            Taint.assertTainted(ch)
            continue

        if ch == 'T':
            Taint.assertTainted(ch)
            continue

        if ch == 'g':
            Taint.assertTainted(ch)
            continue

        if ch == 't':
            Taint.assertTainted(ch)
            continue

        if ch == '\n':
            return

        Taint.assertNotTainted(ch)


last = 42


def gen_random(max):
    global last

    IM = 139968
    IA = 3877
    IC = 29573

    last = (last * IA + IC) % IM
    return max * last / IM


class aminoacids():
    pass


def makeCumulative(genelist, count):
    cp = 0.0

    for i in range(count):
        cp += genelist[i].p
        genelist[i].p = cp


def selectRandom(genelist, count):
    r = gen_random(1)
    i = 0
    lo = 0
    hi = 0

    if r < genelist[0].p:
        return genelist[0].c

    lo = 0
    hi = count - 1

    while hi > (lo + 1):
        i = (hi + lo) / 2
        i = floor(i)
        if r < genelist[i].p:
            hi = i
        else:
            lo = i

    return genelist[hi].c


def makeRandomFasta(id, desc, genelist, count, n):
    LINE_LENGTH = 60

    todo = n
    m = 0

    writeFastaHeader(id, desc)

    while todo > 0:
        pick = []
        if todo < LINE_LENGTH:
            m = todo
        else:
            m = LINE_LENGTH
        for i in range(m):
            pick.append(selectRandom(genelist, count))
        pick.append('\0')
        writeFasta(pick, 0)
        todo -= LINE_LENGTH


def makeRepeatFasta(id, desc, s, n):
    LINE_LENGTH = 60

    todo = n
    k = 0
    kn = len(s)
    m = 0

    ss = []
    for i in range(kn):
        ss.append(s[i])

    writeFastaHeader(id, desc)

    while todo > 0:
        if todo < LINE_LENGTH:
            m = todo
        else:
            m = LINE_LENGTH

        while m >= kn - k:
            writeFasta(s, k)
            m -= kn - k
            k = 0

        ss[k + m] = '\0'
        writeFasta(ss, k)
        ss[k + m] = s[m + k]
        k += m
        todo -= LINE_LENGTH


iub = []
homosapiens = []
alu = []


def setupBaseData():
    global homosapiens
    homosapiens = []
    for i in range(4):
        elt = aminoacids()
        homosapiens.append(elt)
        elt.c = '\0'
        elt.p = 0.0

    homosapiens[0].c = 'a'
    homosapiens[0].p = 0.3029549426680
    homosapiens[1].c = 'c'
    homosapiens[1].p = 0.1979883004921
    homosapiens[2].c = Taint.add('g')
    homosapiens[2].p = 0.1975473066391
    homosapiens[3].c = Taint.add('t')
    homosapiens[3].p = 0.3015094502008

    global iub
    iub = []
    for i in range(15):
        elt = aminoacids()
        iub.append(elt)
        elt.c = '\0'
        elt.p = 0.0
    iub[0].c = 'a'
    iub[0].p = 0.27
    iub[1].c = 'c'
    iub[1].p = 0.12
    iub[2].c = Taint.add('g')
    iub[2].p = 0.12
    iub[3].c = Taint.add('t')
    iub[3].p = 0.27
    iub[4].c = 'B'
    iub[4].p = 0.02
    iub[5].c = 'D'
    iub[5].p = 0.02
    iub[6].c = 'H'
    iub[6].p = 0.02
    iub[7].c = 'K'
    iub[7].p = 0.02
    iub[8].c = 'M'
    iub[8].p = 0.02
    iub[9].c = 'N'
    iub[9].p = 0.02
    iub[10].c = 'R'
    iub[10].p = 0.02
    iub[11].c = 'S'
    iub[11].p = 0.02
    iub[12].c = 'V'
    iub[12].p = 0.02
    iub[13].c = 'W'
    iub[13].p = 0.02
    iub[14].c = 'Y'
    iub[14].p = 0.02

    global alu
    ALU_LENGTH = 287
    alu_init = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"
    alu = []
    for i in range(ALU_LENGTH):
        ch = alu_init[i]
        if (ch == 'G'):
            ch = Taint.add('G')
        elif (ch == 'T'):
            ch = Taint.add('T')
        alu.append(ch)


def benchmark():
    n = 1000000

    setupBaseData()

    makeCumulative(iub, 15)
    makeCumulative(homosapiens, 4)

    makeRepeatFasta("ONE", "Homo sapiens alu", alu, n * 2)
    makeRandomFasta("TWO", "IUB ambiguity codes", iub, 15, n * 3)
    makeRandomFasta("THREE", "Homo sapiens frequency", homosapiens, 4, n * 5)

    return 0


def getExpectedResult():
    return 0


def test():
    benchmark()
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
