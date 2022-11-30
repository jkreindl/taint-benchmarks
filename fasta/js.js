// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fasta-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * by Paul Hsieh
 */

const benchmarkName = "fasta";

const Taint = Polyglot.import("taint");

function writeFastaHeader(id, desc) {
    // printf(">%s %s\n", id, desc);
}

function writeFasta(str, from) {
    for (let i = from; i < str.length; i++) {
        const ch = str[i];

        if (ch == 'G') {
            Taint.assertTainted(ch);
            continue;
        }

        if (ch == 'T') {
            Taint.assertTainted(ch);
            continue;
        }

        if (ch == 'g') {
            Taint.assertTainted(ch);
            continue;
        }

        if (ch == 't') {
            Taint.assertTainted(ch);
            continue;
        }

        if (ch == '\n') {
            return;
        }

        Taint.assertNotTainted(ch);
    }

    // puts(str);
}


var last = 42;

function gen_random(max) {

    const IM = 139968;
    const IA = 3877;
    const IC = 29573;

    return max * (last = (last * IA + IC) % IM) / IM;
}

function aminoacids() {
    this.c = '0';
    this.p = 0.0;
}

/* Weighted selection from alphabet */

function makeCumulative(genelist, count) {
    let cp = 0.0;

    for (let i = 0; i < count; i++) {
        cp += genelist[i].p;
        genelist[i].p = cp;
    }
}

function selectRandom(genelist, count) {
    const r = gen_random(1);

    if (r < genelist[0].p)
        return genelist[0].c;

    let lo = 0;
    let hi = count - 1;

    while (hi > (lo + 1)) {
        let i = (hi + lo) / 2;
        i = Math.floor(i);
        if (r < genelist[i].p)
            hi = i;
        else
            lo = i;
    }
    return genelist[hi].c;
}

/* Generate and write FASTA format */


function makeRandomFasta(id, desc, genelist, count, n) {
    const LINE_LENGTH = 60;

    let todo = n;

    writeFastaHeader(id, desc);

    for (; todo > 0; todo -= LINE_LENGTH) {
        var pick = [];
        let m;
        if (todo < LINE_LENGTH)
            m = todo;
        else
            m = LINE_LENGTH;
        for (let i = 0; i < m; i++)
            pick[i] = selectRandom(genelist, count);
        pick[m] = '\0';
        writeFasta(pick, 0);
    }
}

function makeRepeatFasta(id, desc, s, n) {
    const LINE_LENGTH = 60;

    let todo = n;
    let k = 0;
    const kn = s.length;

    var ss = [];
    for (var i = 0; i < kn; i++) {
        ss[i] = s[i];
    }

    writeFastaHeader(id, desc);

    for (; todo > 0; todo -= LINE_LENGTH) {
        let m;
        if (todo < LINE_LENGTH)
            m = todo;
        else
            m = LINE_LENGTH;

        while (m >= kn - k) {
            writeFasta(s, k);
            m -= kn - k;
            k = 0;
        }

        ss[k + m] = '\0';
        writeFasta(ss, k);
        ss[k + m] = s[m + k];
        k += m;
    }
}

/* Main -- define alphabets, make 3 fragments */

var iub;
var homosapiens;
var alu;

function setupBaseData() {
    const Taint = Polyglot.import("taint");

    homosapiens = [];
    for (let i = 0; i < 4; i++) homosapiens[i] = new aminoacids();
    homosapiens[0].c = 'a';
    homosapiens[0].p = 0.3029549426680;
    homosapiens[1].c = 'c';
    homosapiens[1].p = 0.1979883004921;
    homosapiens[2].c = Taint.add('g');
    homosapiens[2].p = 0.1975473066391;
    homosapiens[3].c = Taint.add('t');
    homosapiens[3].p = 0.3015094502008;

    iub = [];
    for (let i = 0; i < 15; i++) iub[i] = new aminoacids();
    iub[0].c = 'a';
    iub[0].p = 0.27;
    iub[1].c = 'c';
    iub[1].p = 0.12;
    iub[2].c = Taint.add('g');
    iub[2].p = 0.12;
    iub[3].c = Taint.add('t');
    iub[3].p = 0.27;
    iub[4].c = 'B';
    iub[4].p = 0.02;
    iub[5].c = 'D';
    iub[5].p = 0.02;
    iub[6].c = 'H';
    iub[6].p = 0.02;
    iub[7].c = 'K';
    iub[7].p = 0.02;
    iub[8].c = 'M';
    iub[8].p = 0.02;
    iub[9].c = 'N';
    iub[9].p = 0.02;
    iub[10].c = 'R';
    iub[10].p = 0.02;
    iub[11].c = 'S';
    iub[11].p = 0.02;
    iub[12].c = 'V';
    iub[12].p = 0.02;
    iub[13].c = 'W';
    iub[13].p = 0.02;
    iub[14].c = 'Y';
    iub[14].p = 0.02;

    const ALU_LENGTH = 287;
    const alu_init =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";
    alu = [];
    for (let i = 0; i < ALU_LENGTH; i++) {
        let ch = alu_init[i];
        if (ch == 'G') {
            ch = Taint.add('G');
        } else if (ch == 'T') {
            ch = Taint.add('T');
        }
        alu[i] = ch;
    }
}

function benchmark() {
    const n = 1000000;

    setupBaseData();

    makeCumulative(iub, 15);
    makeCumulative(homosapiens, 4);

    makeRepeatFasta("ONE", "Homo sapiens alu", alu, n * 2);
    makeRandomFasta("TWO", "IUB ambiguity codes", iub, 15, n * 3);
    makeRandomFasta("THREE", "Homo sapiens frequency", homosapiens, 4, n * 5);

    return 0;
}

function getExpectedResult() {
    return 0;
}

function main() {
    const result = benchmark();
    return result;
}

function setup(arg) { }

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

