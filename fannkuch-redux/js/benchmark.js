// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fannkuchredux-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * converted to C by Joseph Piché
 * from Java version by Oleg Mazurov and Isaac Gouy
 *
 */

var benchmarkName = "fannkuch-redux";

function max(a, b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

function fannkuchredux(n) {
    var perm = [];
    var perm1 = [];
    var count = [];
    var maxFlipsCount = 0;
    var permCount = 0;
    var checksum = 0;

    const Taint = Polyglot.import("taint");

    var i;

    for (i = 0; i < n; i += 1)
        perm1[i] = i;

    for (i = 0; i < n; i += 3)
        perm1[i] = Taint.add(perm1[i]);

    var r = n;

    while (true) {
        while (r != 1) {
            count[r - 1] = r;
            r -= 1;
        }

        for (i = 0; i < n; i += 1)
            perm[i] = perm1[i];
        var flipsCount = Taint.add(0);
        var k;

        while (true) {
            k = perm[0];
            if (k == 0) {
                break;
            }

            var k2 = (k + 1) >> 1;
            for (i = 0; i < k2; i++) {
                var temp = perm[i];
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

            var perm0 = perm1[0];
            i = 0;
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
    var result = fannkuchredux(9);
    return result;
}

function getExpectedResult() {
    return 30;
}

function test() {
    var actualResult = benchmark();
    if (actualResult == getExpectedResult()) {
        return 0;
    } else {
        return 1;
    }
}

function setup(arg) { }

console.assert(typeof benchmark == 'function', "'benchmark' is not a function");
console.assert(typeof benchmarkName == 'string', "'benchmarkName' is not defined or invalid");

function main() {
    var benchmarkIO = Polyglot.import("benchmarkIO");
    setup(benchmarkIO);

    console.log("starting benchmark: " + benchmarkName);
    for (var iteration = 0; iteration < 0xFFFFFFFF; iteration++) {
        var start = new Date();
        var result = benchmark();
        var end = new Date();
        var s = (end.getTime() - start.getTime()) / 1000.00;
        console.log("iteration " + iteration + " took " + s + " seconds and gave " + result);
    }

    return 0;
}

main();

