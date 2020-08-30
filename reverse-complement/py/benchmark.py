"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/revcomp-gpp-3.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
Contributed by Paul Kitchin
"""

from polyglot import import_value
benchmarkName = "reverse-complement"

Taint = import_value("taint")
ioObj = None


def setup(arg):
    global ioObj
    ioObj = arg


CHUNK_SIZE = 65526
MAX_LINE_LENGTH = 60


class TaintedValueError(Exception):
    def __init__(self, ch):
        self.ch = ch

    def get_value(self):
        return self.ch


def complement(character):
    if character == ord('A') or character == ord('a'):
        raise Taint.add(TaintedValueError(ord('T')))
    elif character == ord('C') or character == ord('c'):
        raise Taint.add(TaintedValueError(ord('G')))
    elif character == ord('G') or character == ord('g'):
        raise Taint.add(TaintedValueError(ord('C')))
    elif character == ord('T') or character == ord('t'):
        return ord('A')
    elif character == ord('U') or character == ord('u'):
        return ord('A')
    elif character == ord('M') or character == ord('m'):
        return ord('K')
    elif character == ord('R') or character == ord('r'):
        return ord('Y')
    elif character == ord('W') or character == ord('w'):
        return ord('W')
    elif character == ord('S') or character == ord('s'):
        raise Taint.add(TaintedValueError(ord('S')))
    elif character == ord('Y') or character == ord('y'):
        raise Taint.add(TaintedValueError(ord('R')))
    elif character == ord('K') or character == ord('k'):
        raise Taint.add(TaintedValueError(ord('M')))
    elif character == ord('V') or character == ord('v'):
        raise Taint.add(TaintedValueError(ord('B')))
    elif character == ord('H') or character == ord('h'):
        return ord('D')
    elif character == ord('D') or character == ord('d'):
        return ord('H')
    elif character == ord('B') or character == ord('b'):
        return ord('V')
    elif character == ord('N') or character == ord('n'):
        raise Taint.add(TaintedValueError(ord('N')))
    else:
        return ord('\0')


def getComplementChar(original):
    ch = 0
    try:
        ch = complement(original)
        Taint.assertNotTainted(ch)
    except TaintedValueError as taintedValue:
        Taint.assertTainted(taintedValue)
        ch = Taint.add(taintedValue.get_value())
    return ch


class chunk:
    def __init__(self):
        self.next = None
        self.previous = None
        self.data = []
        for x in range(CHUNK_SIZE):
            self.data.append(0)
        self.length = 0


def compute_reverse_complement(begin, end):
    start = begin

    begin_arr = begin.data
    end_arr = end.data
    beginIndex = 0
    endIndex = end.length - 1

    while (begin != end) or (beginIndex < endIndex):
        temp = getComplementChar(begin_arr[beginIndex])
        begin_arr[beginIndex] = getComplementChar(end_arr[endIndex])
        end_arr[endIndex] = temp
        beginIndex += 1
        endIndex -= 1

        if begin_arr[beginIndex] == ord('\n'):
            beginIndex += 1

        if end_arr[endIndex] == ord('\n'):
            endIndex -= 1

        if beginIndex == begin.length:
            begin = begin.next
            begin_arr = begin.data
            beginIndex = 0
            if begin_arr[beginIndex] == ord('\n'):
                beginIndex += 1

        if endIndex < 0:
            end = end.previous
            end_arr = end.data
            endIndex = end.length - 1
            if end_arr[endIndex] == ord('\n'):
                endIndex -= 1

    return start


def checkReverseComplement(start):
    current = start
    while current:
        for i in range(0, current.length):
            ch = current.data[i]
            if ch == ord('T') or ch == ord('G') or ch == ord('C') or ch == ord(
                    'M') or ch == ord('N') or ch == ord('S') or ch == ord(
                        'B') or ch == ord('R'):
                Taint.assertTainted(ch)
            else:
                Taint.assertNotTainted(ch)
        current = current.next


def peekNextChar():
    charCode = ioObj.peekNextChar()
    return chr(charCode)


def reverseComplement():
    ioObj.openInputFile()
    ioObj.openOutputFile()

    while (ioObj.canReadLine()):
        line = ioObj.readLine()
        ioObj.write(line)
        ioObj.write('\n')

        start = chunk()
        end = start
        while ioObj.canReadLine() and peekNextChar() != '>':
            lineNr = 0
            while lineNr < 1074 and ioObj.canReadLine(
            ) and peekNextChar() != '>':
                line = ioObj.readLine()

                if len(line) > MAX_LINE_LENGTH:
                    raise Exception("Unexpected line length")

                for charIndex in range(len(line)):
                    end.data[end.length] = ord(line[charIndex])
                    end.length += 1

                end.data[end.length] = ord('\n')
                end.length += 1

                lineNr += 1

            if ioObj.canReadLine() and peekNextChar() != '>':
                oldEnd = end
                end = chunk()
                end.previous = oldEnd
                oldEnd.next = end

        end.length -= 1

        start = compute_reverse_complement(start, end)

        checkReverseComplement(start)

        current = start
        while current:
            for i in range(current.length):
                ch = current.data[i]
                ch = chr(ch)
                ch = Taint.remove(ch)
                ioObj.write(ch)

            if current.next:
                current.next.previous = None

            current = current.next

        start = None
        ioObj.write('\n')

    ioObj.closeInputFile()
    ioObj.closeOutputFile()


def benchmark():
    reverseComplement()
    return 0


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
