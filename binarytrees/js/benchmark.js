// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/binarytrees-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

   contributed by Kevin Carson
   compilation:
       gcc -O3 -fomit-frame-pointer -funroll-loops -static binary-trees.c -lm
       icc -O3 -ip -unroll -static binary-trees.c -lm

   *reset*
*/

var benchmarkName = "binarytrees";

function shouldBeTainted(level) {
    return (level & 0b11) == 0b11;
}

function treeNode() {
    this.left = undefined;
    this.right = undefined;
    this.item = undefined;
}

function getItem(level) {
    const Taint = Polyglot.import("taint");
    var item = 1;
    if (shouldBeTainted(level)) {
        item = Taint.add(item);
    }
    return item;
}

function NewTreeNode(level, left, right) {
    var node = new treeNode();

    node.left = left;
    node.right = right;
    node.item = getItem(level);

    return node;
} /* NewTreeNode() */

function ItemCheck(tree) {
    var result = 0;
    result += tree.item;
    if (tree.left != undefined) {
        result += ItemCheck(tree.left);
        result += ItemCheck(tree.right);
    }
    return result;
} /* ItemCheck() */

function BottomUpTree(depth) {
    var node = undefined;
    if (depth > 0) {
        node = NewTreeNode(
            depth,
            BottomUpTree(depth - 1),
            BottomUpTree(depth - 1));
    } else {
        node = NewTreeNode(depth, undefined, undefined);
    }
    return node;
} /* BottomUpTree() */

function DeleteTree(depth, tree) {
    if (tree.left != undefined) {
        DeleteTree(depth - 1, tree.left);
        DeleteTree(depth - 1, tree.right);
    }

    const Taint = Polyglot.import("taint");

    var item = tree.item;

    if (shouldBeTainted(depth)) {
        Taint.assertTainted(item);
        tree.item = undefined;
    } else {
        Taint.assertNotTainted(item);
    }
} /* DeleteTree() */

function benchmark() {
    const Taint = Polyglot.import("taint");

    var N, depth, minDepth, maxDepth, stretchDepth;
    var stretchTree, longLivedTree, tempTree;

    N = 14;

    minDepth = 4;

    if ((minDepth + 2) > N)
        maxDepth = minDepth + 2;
    else
        maxDepth = N;

    stretchDepth = maxDepth + 1;

    var result = 0;
    var check = 0;

    stretchTree = BottomUpTree(stretchDepth);
    check = ItemCheck(stretchTree);
    Taint.assertTainted(check);
    result += check;

    DeleteTree(stretchDepth, stretchTree);

    longLivedTree = BottomUpTree(maxDepth);

    for (depth = minDepth; depth <= maxDepth; depth += 2) {
        var i, iterations;

        iterations = Math.pow(2, maxDepth - depth + minDepth);

        check = 0;

        for (i = 1; i <= iterations; i++) {
            tempTree = BottomUpTree(depth);

            var curCheck = ItemCheck(tempTree);

            if (depth >= 0b11)
                Taint.assertTainted(curCheck);
            else
                Taint.assertNotTainted(curCheck);

            check += curCheck;
            DeleteTree(depth, tempTree);
        } /* for(i = 1...) */

        if ((iterations > 0) && (depth >= 0b11))
            Taint.assertTainted(check);
        else
            Taint.assertNotTainted(check);

        result += check;

        check = 0;
    } /* for(depth = minDepth...) */

    check = ItemCheck(longLivedTree);
    result += check;

    DeleteTree(maxDepth, longLivedTree);
    Taint.assertTainted(result);
    result = Taint.remove(result);

    return result;
} /* benchmark() */

function getExpectedResult() {
    return 3222190;
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

