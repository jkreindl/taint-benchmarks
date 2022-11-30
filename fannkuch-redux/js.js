// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fannkuchredux-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * converted to C by Joseph Piché
 * from Java version by Oleg Mazurov and Isaac Gouy
 *
 */

const benchmarkName = "fannkuch-redux";

function max(a, b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

function fannkuchredux(n) {
    const perm = [];
    const perm1 = [];
    const count = [];
    let maxFlipsCount = 0;
    let permCount = 0;
    let checksum = 0;

    const Taint = Polyglot.import("taint");

    for (let i = 0; i < n; i += 1)
        perm1[i] = i;

    for (let i = 0; i < n; i += 3)
        perm1[i] = Taint.add(perm1[i]);

    let r = n;

    while (true) {
        while (r != 1) {
            count[r - 1] = r;
            r -= 1;
        }

        for (let i = 0; i < n; i += 1)
            perm[i] = perm1[i];
        let flipsCount = Taint.add(0);
        let k;

        while (true) {
            k = perm[0];
            if (k == 0) {
                break;
            }

            const k2 = (k + 1) >> 1;
            for (let i = 0; i < k2; i++) {
                const temp = perm[i];
                perm[i] = perm[k - i];
                perm[k - i] = temp;
            }
            flipsCount += 1;
            Taint.assertTainted(flipsCount);
        }

        maxFlipsCount = max(maxFlipsCount, flipsCount);
        if (permCount % 2 == 0) {
            checksum += flipsCount;
        } else {
            checksum -= flipsCount;
        }

        // the flipsCount must be tainted now
        if (maxFlipsCount == flipsCount)
            Taint.assertTainted(checksum);

        /* Use incremental change to generate another permutation */
        while (true) {
            if (r == n) {
                for (var idx = 0; idx < n; idx += 3)
                    Taint.assertTainted(perm1[idx]);
                for (var idx = 1; idx < n; idx += 3)
                    Taint.assertNotTainted(perm1[idx]);
                for (var idx = 2; idx < n; idx += 3)
                    Taint.assertNotTainted(perm1[idx]);

                return Taint.remove(maxFlipsCount);
            }

            const perm0 = perm1[0];
            let i = 0;
            while (i < r) {
                var j = i + 1;
                perm1[i] = perm1[j];
                i = j;
            }
            perm1[r] = perm0;
            count[r] = count[r] - 1;
            if (count[r] > 0)
                break;
            r++;
        }
        permCount++;
    }
}

/*int main(int argc, char *argv[])
{
    int n = argc > 1 ? atoi(argv[1]) : 7;
    printf("Pfannkuchen(%d) = %d\n", n, fannkuchredux(n));
    return 0;
}*/

function benchmark() {
    const result = fannkuchredux(10);
    return result;
}

function getExpectedResult() {
    return 38;
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

