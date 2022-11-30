// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-node-1.html

/* The Computer Language Benchmarks Game
   https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   Contributed by Jesse Millikan
   Modified by Matt Baker
   Ported, modified, and parallelized by Roman Pletnev
*/

const benchmarkName = "k-nucleotide";

const Taint = Polyglot.import("taint");
var ioObj;

function setup(arg) {
    ioObj = arg;
}

function RefNum(num) {
    this.num = Taint.add(num);
}

function frequency(seq, length) {
    let freq = new Map();
    let n = seq.length - length + 1;
    for (let i = 0; i < n; ++i) {
        let key = seq.substr(i, length);
        key = Taint.remove(key);
        const cur = freq.get(key);
        cur === undefined ? freq.set(key, new RefNum(1)) : ++cur.num;
    }
    return freq;
}

function sort(seq, length) {
    Taint.assertTainted(seq);
    const f = frequency(seq, length);
    const keys = Array.from(f.keys());
    const n = seq.length - length + 1;
    let res = "";
    keys.sort((a, b) => Taint.remove(f.get(b).num - f.get(a).num));
    for (let key of keys) {
        key = Taint.remove(key);
        const count = f.get(key).num * 100 * 1000;
        const fraction = Math.floor(count / n);
        Taint.assertTainted(fraction);
        res += key + ' ' + (Math.floor(fraction / 1000)) + "." + (fraction % 1000).toString().padStart(3, "0") + '\n';
    }
    res += '\n';
    return res;
}

function find(seq, s) {
    Taint.assertTainted(seq);
    const f = frequency(seq, s.length);
    return (f.get(s).num || 0) + "\t" + Taint.remove(s) + '\n';
}

function readSequence() {
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

function computeResult(seq) {
    seq = Taint.add(seq);
    let res = sort(seq, 1);
    res += sort(seq, 2);
    res += find(seq, "GGT");
    res += find(seq, "GGTA");
    res += find(seq, "GGTATT");
    res += find(seq, "GGTATTTTAATT");
    res += find(seq, "GGTATTTTAATTTATAGT");
    Taint.assertTainted(res);
    res = Taint.remove(res);
    return res;
}

function writeResult(res) {
    ioObj.openOutputFile();
    ioObj.write(res);
    ioObj.closeOutputFile();
}

function benchmark() {
    const seq = readSequence();
    const res = computeResult(seq);
    writeResult(res);
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

