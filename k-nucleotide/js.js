// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-node-1.html

/* The Computer Language Benchmarks Game
   https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   Contributed by Jesse Millikan
   Modified by Matt Baker
   Ported, modified, and parallelized by Roman Pletnev
*/

var benchmarkName = "k-nucleotide";

const Taint = Polyglot.import("taint");
var ioObj;

function setup (arg) {
    ioObj = arg;
}

function RefNum (num) {
    num = Taint.addTaint(num);
    this.num = num;
}

function frequency (seq, length) {
    var freq = new Map(),
        n = seq.length - length + 1,
        key, cur, i = 0;
    for (; i < n; ++i) {
        key = seq.substr(i, length);
        cur = freq.get(key);
        cur === undefined ? freq.set(key, new RefNum(1)) : ++cur.num;
    }
    return freq;
}

function sort (seq, length) {
    Taint.assertTainted(seq);
    var f = frequency(seq, length),
        keys = Array.from(f.keys()),
        n = seq.length - length + 1,
        res = "";
    keys.sort((a, b) => Taint.removeTaint(f.get(b).num - f.get(a).num));
    for (var key of keys) {
        const count = f.get(key).num * 100 * 1000;
        const fraction = Math.floor(count / n);
        Taint.assertTainted(fraction);
        res += key + ' ' + (Math.floor(fraction / 1000)) + "." + (fraction % 1000).toString().padStart(3, "0") + '\n';
    }
    res += '\n';
    return res;
}

function find (seq, s) {
    Taint.assertTainted(seq);
    var f = frequency(seq, s.length);
    return (f.get(s).num || 0) + "\t" + Taint.removeTaint(s) + '\n';
}

function readSequence () {
    ioObj.openInputFile();
    let seq = "";
    let reading = false;
    while (ioObj.canReadLine()) {
        let line = ioObj.readLine().toUpperCase();
        if (reading) {
            seq += line;
        } else {
            // skip to third polynucleotide
            reading = line.substr(0, 6) === '>THREE';
        }
    }
    ioObj.closeInputFile();
    return seq;
}

function computeResult (seq) {
    seq = Taint.addTaint(seq);
    let res = sort(seq, 1);
    res += sort(seq, 2);
    res += find(seq, "GGT");
    res += find(seq, "GGTA");
    res += find(seq, "GGTATT");
    res += find(seq, "GGTATTTTAATT");
    res += find(seq, "GGTATTTTAATTTATAGT");
    Taint.assertTainted(res);
    res = Taint.removeTaint(res);
    return res;
}

function writeResult (res) {
    ioObj.openOutputFile();
    ioObj.write(res);
    ioObj.closeOutputFile();
}

function benchmark () {
    const seq = readSequence();
    const res = computeResult(seq);
    writeResult(res);
    return 0;
}

function getExpectedResult () {
    return 0;
}

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
