// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/regexredux-node-4.html

/* The Computer Language Benchmarks Game
   https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   regex-dna program contributed by Jesse Millikan
   Base on the Ruby version by jose fco. gonzalez
   fixed by Matthew Wilson
   ported to Node.js and sped up by Roman Pletnev
   converted from regex-dna program
   fixed by Josh Goldfoot
   multi thread by Andrey Filatkin
   sequential by Isaac Gouy
*/

const benchmarkName = "regex-redux";

const Taint = Polyglot.import("taint");
var ioObj;

function setup(arg) {
    ioObj = arg;
}

function readData() {
    let data = Taint.add("");
    ioObj.openInputFile();
    while (ioObj.canReadLine()) {
        data += ioObj.readLine();
        data += "\n";
    }
    ioObj.closeInputFile();
    return data;
}

function benchmark() {
    const regExps = [
        /agggtaaa|tttaccct/ig,
        /[cgt]gggtaaa|tttaccc[acg]/ig,
        /a[act]ggtaaa|tttacc[agt]t/ig,
        /ag[act]gtaaa|tttac[agt]ct/ig,
        /agg[act]taaa|ttta[agt]cct/ig,
        /aggg[acg]aaa|ttt[cgt]ccct/ig,
        /agggt[cgt]aa|tt[acg]accct/ig,
        /agggta[cgt]a|t[acg]taccct/ig,
        /agggtaa[cgt]|[acg]ttaccct/ig
    ];

    let data = readData();
    Taint.assertTainted(data);
    const initialLen = data.length;

    data = data.replace(/^>.*\n|\n/mg, "");
    const cleanedLen = data.length;

    ioObj.openOutputFile();

    for (let j = 0; j < regExps.length; j++) {
        const re = regExps[j];
        const m = data.match(re);

        Taint.assertTainted(m);

        ioObj.write(`${re.source} ${Taint.remove(m ? m.length : 0)}\n`);
    }

    let newData = data
        .replace(/tHa[Nt]/g, '<4>')
        .replace(/aND|caN|Ha[DS]|WaS/g, '<3>')
        .replace(/a[NSt]|BY/g, '<2>')
        .replace(/<[^>]*>/g, '|')
        .replace(/\|[^|][^|]*\|/g, '-');

    Taint.assertTainted(newData);
    const endLen = newData.length;

    Taint.assertTainted(initialLen);
    Taint.assertTainted(cleanedLen);
    Taint.assertTainted(endLen);

    ioObj.write(`\n${Taint.remove(initialLen)}\n${Taint.remove(cleanedLen)}\n${Taint.remove(endLen)}`);

    ioObj.closeOutputFile();

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

