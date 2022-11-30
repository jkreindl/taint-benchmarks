// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/pidigits-node-3.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * contributed by Denis Gribov
 *    a translation of the C program contributed by Mr Ledhug
 */

const benchmarkName = "pidigits";

const Taint = Polyglot.import("taint");
var ioObj;

function setup(arg) {
    ioObj = arg;
}

const chr_0 = "0".charCodeAt(0);

function benchmark() {
    const n = 2500;

    ioObj.openOutputFile();

    // Int32
    let i = 0,
        k = 0,
        d = 0,
        k2 = 0,
        d3 = 0,
        d4 = 0;

    // BigInt
    let tmp1 = Taint.add(0n), // mpz_init(tmp1)
        tmp2 = Taint.add(0n), // mpz_init(tmp2)
        acc = Taint.add(0n), // mpz_init_set_ui(acc, 0)
        den = Taint.add(1n), // mpz_init_set_ui(den, 1)
        num = Taint.add(1n); // mpz_init_set_ui(num, 1)

    while (i < n) {
        k++;

        //#region inline nextTerm(k)
        k2 = k * 2 + 1;
        acc += num * 2n; // mpz_addmul_ui(acc, num, 2)
        acc *= BigInt(k2); // mpz_mul_ui(acc, acc, k2)
        den *= BigInt(k2); // mpz_mul_ui(den, den, k2)
        num *= BigInt(k); // mpz_mul_ui(num, num, k)
        //#endregion inline nextTerm(k)

        if (num > acc /* mpz_cmp(num, acc) > 0 */) continue;

        //#region inline extractDigit(3);
        tmp1 = num * 3n; // mpz_mul_ui(tmp1, num, nth);
        tmp2 = tmp1 + acc; // mpz_add(tmp2, tmp1, acc);
        tmp1 = tmp2 / den; // mpz_tdiv_q(tmp1, tmp2, den);
        d3 = Number(tmp1) >>> 0; // mpz_get_ui(tmp1)
        //#region inline extractDigit(3);

        d = d3;

        //#region inline extractDigit(4);
        tmp1 = num * 4n; // mpz_mul_ui(tmp1, num, nth);
        tmp2 = tmp1 + acc; // mpz_add(tmp2, tmp1, acc);
        tmp1 = tmp2 / den; // mpz_tdiv_q(tmp1, tmp2, den);
        d4 = Number(tmp1) >>> 0; // mpz_get_ui(tmp1)
        //#region inline extractDigit(4);

        if (d !== d4) continue;

        ioObj.write(Math.floor(Taint.remove(d) + chr_0));

        if (++i % 10 === 0) {
            ioObj.write("\t:");
            ioObj.writeNumber(i);
            ioObj.write("\n");
        }

        //#region inline eliminateDigit(d)
        acc -= den * BigInt(d); // mpz_submul_ui(acc, den, d)
        acc *= 10n; // mpz_mul_ui(acc, acc, 10)
        num *= 10n; // mpz_mul_ui(num, num, 10)
        //#endregion inline eliminateDigit(d)
    }

    Taint.assertNotTainted(i);
    Taint.assertNotTainted(k);
    Taint.assertTainted(d);
    Taint.assertTainted(d3);
    Taint.assertTainted(d4);

    Taint.assertTainted(tmp1);
    Taint.assertTainted(tmp2);
    Taint.assertTainted(acc);
    Taint.assertTainted(den);
    Taint.assertTainted(num);

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

