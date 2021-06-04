// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/mandelbrot-clang-2.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   contributed by Greg Buchholz
   
   for the debian (AMD) machine...
   compile flags:  -O3 -ffast-math -march=athlon-xp -funroll-loops

   for the gp4 (Intel) machine...
   compile flags:  -O3 -ffast-math -march=pentium4 -funroll-loops
*/

var benchmarkName = "mandelbrot";

function LoopBodyData () {
    this.Zi = 0.0;
    this.Zr = 0.0;
    this.Ti = 0.0;
    this.Tr = 0.0;
    this.Ci = 0.0;
    this.Cr = 0.0;
}

function doLoop (bodyData) {
    bodyData.Zi = 2.0 * bodyData.Zr * bodyData.Zi + bodyData.Ci;
    bodyData.Zr = bodyData.Tr - bodyData.Ti + bodyData.Cr;
    bodyData.Tr = bodyData.Zr * bodyData.Zr;
    bodyData.Ti = bodyData.Zi * bodyData.Zi;
}

function shouldDoLoop (i, bodyData, limit) {
    if (i >= 50) {
        return false;
    }

    if ((bodyData.Tr + bodyData.Ti) > (limit * limit)) {
        return false;
    }

    return true;
}

function mandelbrot (n) {
    var w, h, bit_num = 0;
    var byte_acc = 0;
    var i;
    var x, y, limit = 2.0;

    w = h = n;

    var result = 0;

    const Taint = Polyglot.import("taint");

    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            var bodyData = new LoopBodyData();
            bodyData.Zi = Taint.addTaint(0.0);
            bodyData.Cr = (2.0 * x / w - 1.5);
            bodyData.Ci = (2.0 * y / h - 1.0);

            for (i = 0; shouldDoLoop(i, bodyData, limit); ++i) {
                doLoop(bodyData);
            }

            byte_acc <<= 1;
            if (bodyData.Tr + bodyData.Ti <= limit * limit) {
                byte_acc |= 0x01;
                if (Taint.checkTainted(bodyData.Tr)) {
                    if (Taint.checkTainted(n)) {
                        byte_acc = Taint.addTaint(byte_acc);
                    }
                }
            }

            bit_num++;

            if (bit_num == 8) {
                // putc(byte_acc, stdout);
                result += byte_acc;
                byte_acc = 0;
                bit_num = 0;
            } else if (x == (w - 1)) {
                byte_acc <<= (8 - w % 8);
                // putc(byte_acc, stdout);
                result += byte_acc;
                byte_acc = 0;
                bit_num = 0;
            }
        }
    }

    return result;
}

function benchmark () {
    const Taint = Polyglot.import("taint");
    var sum = 0;
    for (var i = 0; i < 10; i++) {
        var n = 400;
        if (i & 0x11)
            n = Taint.addTaint(n);
        sum += mandelbrot(n);
    }
    Taint.assertTainted(sum);
    sum = Taint.removeTaint(sum);
    return sum;
}

function getExpectedResult () {
    // in c, byte_acc is a `char`, and
    // overflows a couple of times in the
    // benchmark, while this does not
    // happen in js. as a result,
    // the result value differs in c and js
    return 20213330;
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
