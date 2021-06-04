// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/spectralnorm-node-1.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Ian Osgood
// modified for Node.js by Isaac Gouy

var benchmarkName = "spectral-norm";

function A (i, j) {
    return 1 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

function Au (u, v) {
    var Taint = Polyglot.import("taint");
    for (var i = Taint.addTaint(0); i < u.length; ++i) {
        var t = 0;
        for (var j = 0; j < u.length; ++j)
            t += A(i, j) * u[j];
        v[i] = t;
    }
}

function Atu (u, v) {
    for (var i = 0; i < u.length; ++i) {
        var t = 0;
        for (var j = 0; j < u.length; ++j)
            t += A(j, i) * u[j];
        v[i] = t;
    }
}

function AtAu (u, v, w) {
    Au(u, w);
    Atu(w, v);
}

function spectralnorm (n) {
    var i, u = [],
        v = [],
        w = [],
        vv = 0,
        vBv = 0;
    for (i = 0; i < n; ++i) {
        u[i] = 1;
        v[i] = w[i] = 0;
    }
    for (i = 0; i < 10; ++i) {
        AtAu(u, v, w);
        AtAu(v, u, w);
    }
    for (i = 0; i < n; ++i) {
        vBv += u[i] * v[i];
        vv += v[i] * v[i];
    }

    var result = vBv + vv;

    var Taint = Polyglot.import("taint");
    Taint.assertTainted(result);
    result = Taint.removeTaint(result);

    return result;
}

function benchmark () {
    var sum = 0;
    for (var i = 0; i < 10; i++) {
        sum += spectralnorm(500);
    }
    return sum;
}

function getExpectedResult () {
    return 12864590031.750621;
}

function setup (arg) {}

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
