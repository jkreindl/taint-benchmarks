// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/revcomp-gpp-3.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
// Contributed by Paul Kitchin

const benchmarkName = "reverse-complement";

const Taint = Polyglot.import("taint");
var ioObj;

function setup(arg) {
    ioObj = arg;
}

const CHUNK_SIZE = 65526;
const MAX_LINE_LENGTH = 60;

function complement(character) {
    switch (character) {
        case 'A':
        case 'a':
            throw Taint.add('T');
        case 'C':
        case 'c':
            throw Taint.add('G');
        case 'G':
        case 'g':
            throw Taint.add('C');
        case 'T':
        case 't':
            return 'A';
        case 'U':
        case 'u':
            return 'A';
        case 'M':
        case 'm':
            return 'K';
        case 'R':
        case 'r':
            return 'Y';
        case 'W':
        case 'w':
            return 'W';
        case 'S':
        case 's':
            throw Taint.add('S');
        case 'Y':
        case 'y':
            throw Taint.add('R');
        case 'K':
        case 'k':
            throw Taint.add('M');
        case 'V':
        case 'v':
            throw Taint.add('B');
        case 'H':
        case 'h':
            return 'D';
        case 'D':
        case 'd':
            return 'H';
        case 'B':
        case 'b':
            return 'V';
        case 'N':
        case 'n':
            throw Taint.add('N');
        default:
            return '\0';
    }
}

function getComplementChar(original) {
    var ch;
    try {
        ch = complement(original);
        Taint.assertNotTainted(ch);
    } catch (taintedChar) {
        ch = taintedChar;
        Taint.assertTainted(ch);
    }
    return ch;
}

function chunk() {
    this.next = null;
    this.previous = null;
    this.data = [].fill('\0', 0, CHUNK_SIZE);
    this.length = 0;
}

function compute_reverse_complement(begin, end) {
    var start = begin;

    var begin_arr = begin.data;
    var end_arr = end.data;
    var beginIndex = 0;
    var endIndex = end.length - 1;

    while (begin != end || beginIndex < endIndex) {
        const temp = getComplementChar(begin_arr[beginIndex]);
        begin_arr[beginIndex++] = getComplementChar(end_arr[endIndex]);
        end_arr[endIndex--] = temp;

        if (begin_arr[beginIndex] == '\n') {
            beginIndex++;
        }

        if (end_arr[endIndex] == '\n') {
            --endIndex;
        }

        if (beginIndex == begin.length) {
            begin = begin.next;
            begin_arr = begin.data;
            beginIndex = 0;
            if (begin_arr[beginIndex] == '\n') {
                beginIndex++;
            }
        }

        if (endIndex < 0) {
            end = end.previous;
            end_arr = end.data;
            endIndex = end.length - 1;
            if (end_arr[endIndex] == '\n') {
                --endIndex;
            }
        }
    }

    return start;
}

function checkReverseComplement(start) {
    for (let current = start; current != null; current = current.next) {
        for (let i = 0; i < current.length; i++) {
            const ch = current.data[i];
            switch (ch) {
                case 'T':
                case 'G':
                case 'C':
                case 'M':
                case 'N':
                case 'S':
                case 'B':
                case 'R':
                    Taint.assertTainted(ch);
                    break;
                default:
                    Taint.assertNotTainted(ch);
                    break;
            }
        }
    }
}

function peekNextChar() {
    const charCode = ioObj.peekNextChar();
    return String.fromCharCode(charCode);
}

function reverseComplement() {

    ioObj.openInputFile();
    ioObj.openOutputFile();

    while (ioObj.canReadLine()) {

        let line = ioObj.readLine();
        ioObj.write(line);
        ioObj.write('\n');

        let start = new chunk();
        let end = start;
        while (ioObj.canReadLine() && peekNextChar() != '>') {
            for (var lineNr = 0; lineNr < 1074 && ioObj.canReadLine() && peekNextChar() != '>'; lineNr++) {

                line = ioObj.readLine();

                if (line.length > MAX_LINE_LENGTH) {
                    throw "Unexpected line length";
                }

                for (var charIndex = 0; charIndex < line.length; charIndex++) {
                    end.data[end.length++] = line.charAt(charIndex);
                }

                end.data[end.length++] = '\n';
            }

            if (ioObj.canReadLine() && peekNextChar() != '>') {
                var oldEnd = end;
                end = new chunk();
                end.previous = oldEnd;
                oldEnd.next = end;
            }
        }
        end.length--;

        start = compute_reverse_complement(start, end);

        checkReverseComplement(start);

        for (var current = start; current != null; current = current.next) {
            for (var i = 0; i < current.length; i++) {
                var ch = current.data[i]
                ch = Taint.remove(ch)
                ioObj.write(ch);
            }
            if (current.next != null) {
                current.next.previous = null;
            }
        }
        start = null;
        ioObj.write('\n');
    }

    ioObj.closeInputFile();
    ioObj.closeOutputFile();
}

function benchmark() {
    reverseComplement();
    return 0;
}

function getExpectedResult() {
    return 0;
}

console.assert(typeof benchmark == 'function', "'benchmark' is not a function");
console.assert(typeof benchmarkName == 'string', "'benchmarkName' is not defined or invalid");

function main() {
    const benchmarkIO = Polyglot.import("benchmarkIO");
    setup(benchmarkIO);

    console.log("starting benchmark: " + benchmarkName);
    for (let iteration = 0; iteration < 2000; iteration++) {
        const start = new Date();
        const result = benchmark();
        const end = new Date();
        const s = (end.getTime() - start.getTime()) / 1000.00;
        console.log(`iteration ${iteration} took ${s} seconds and gave ${result}`);
    }

    return 0;
}

main();

