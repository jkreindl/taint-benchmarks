// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

var benchmarkName = "n-body_struct";

var solar_mass;

var bodies = [{
    x: 0.0,
    y: 0.0,
    z: 0.0,
    vx: 0.0,
    vy: 0.0,
    vz: 0.0,
    mass: 0.0
}, {
    x: 0.0,
    y: 0.0,
    z: 0.0,
    vx: 0.0,
    vy: 0.0,
    vz: 0.0,
    mass: 0.0
}, {
    x: 0.0,
    y: 0.0,
    z: 0.0,
    vx: 0.0,
    vy: 0.0,
    vz: 0.0,
    mass: 0.0
}, {
    x: 0.0,
    y: 0.0,
    z: 0.0,
    vx: 0.0,
    vy: 0.0,
    vz: 0.0,
    mass: 0.0
}, {
    x: 0.0,
    y: 0.0,
    z: 0.0,
    vx: 0.0,
    vy: 0.0,
    vz: 0.0,
    mass: 0.0
}];

function init () {
    const pi = 3.141592653589793;
    solar_mass = (4 * pi * pi);
    const days_per_year = 365.24;

    const SUN = 0;
    const JUPITER = 1;
    const SATURN = 2;
    const URANUS = 3;
    const NEPTUNE = 4;

    var Taint = Polyglot.import("taint");

    bodies[SUN].x = Taint.addTaint(0.0);
    bodies[SUN].y = Taint.addTaint(0.0);
    bodies[SUN].z = Taint.addTaint(0.0);
    bodies[SUN].vx = 0;
    bodies[SUN].vy = 0;
    bodies[SUN].vz = 0;
    bodies[SUN].mass = solar_mass;
    bodies[JUPITER].x = Taint.addTaint(4.84143144246472090e+00);
    bodies[JUPITER].y = Taint.addTaint(-1.16032004402742839e+00);
    bodies[JUPITER].z = Taint.addTaint(-1.03622044471123109e-01);
    bodies[JUPITER].vx = 1.66007664274403694e-03 * days_per_year;
    bodies[JUPITER].vy = 7.69901118419740425e-03 * days_per_year;
    bodies[JUPITER].vz = -6.90460016972063023e-05 * days_per_year;
    bodies[JUPITER].mass = 9.54791938424326609e-04 * solar_mass;
    bodies[SATURN].x = Taint.addTaint(8.34336671824457987e+00);
    bodies[SATURN].y = Taint.addTaint(4.12479856412430479e+00);
    bodies[SATURN].z = Taint.addTaint(-4.03523417114321381e-01);
    bodies[SATURN].vx = -2.76742510726862411e-03 * days_per_year;
    bodies[SATURN].vy = 4.99852801234917238e-03 * days_per_year;
    bodies[SATURN].vz = 2.30417297573763929e-05 * days_per_year;
    bodies[SATURN].mass = 2.85885980666130812e-04 * solar_mass;
    bodies[URANUS].x = Taint.addTaint(1.28943695621391310e+01);
    bodies[URANUS].y = Taint.addTaint(-1.51111514016986312e+01);
    bodies[URANUS].z = Taint.addTaint(-2.23307578892655734e-01);
    bodies[URANUS].vx = 2.96460137564761618e-03 * days_per_year;
    bodies[URANUS].vy = 2.37847173959480950e-03 * days_per_year;
    bodies[URANUS].vz = -2.96589568540237556e-05 * days_per_year;
    bodies[URANUS].mass = 4.36624404335156298e-05 * solar_mass;
    bodies[NEPTUNE].x = Taint.addTaint(1.53796971148509165e+01);
    bodies[NEPTUNE].y = Taint.addTaint(-2.59193146099879641e+01);
    bodies[NEPTUNE].z = Taint.addTaint(1.79258772950371181e-01);
    bodies[NEPTUNE].vx = 2.68067772490389322e-03 * days_per_year;
    bodies[NEPTUNE].vy = 1.62824170038242295e-03 * days_per_year;
    bodies[NEPTUNE].vz = -9.51592254519715870e-05 * days_per_year;
    bodies[NEPTUNE].mass = 5.15138902046611451e-05 * solar_mass;
}

function tearDown () {
    for (var i = 0; i < 5; i++) {
        bodies[i].x = 0.0;
        bodies[i].y = 0.0;
        bodies[i].z = 0.0;
        bodies[i].vx = 0.0;
        bodies[i].vy = 0.0;
        bodies[i].vz = 0.0;
        bodies[i].mass = 0.0;
    }
}

function advance () {
    for (var i = 0; i < 5; ++i) {
        var body = bodies[i];
        for (var j = i + 1; j < 5; ++j) {
            var dx = body.x - bodies[j].x;
            var R = dx * dx;
            var dy = body.y - bodies[j].y;
            R += dy * dy;
            var dz = body.z - bodies[j].z;
            R += dz * dz;
            R = Math.sqrt(R);
            var mag = 0.01 / (R * R * R);
            body.vx = body.vx - dx * bodies[j].mass * mag;
            body.vy = body.vy - dy * bodies[j].mass * mag;
            body.vz = body.vz - dz * bodies[j].mass * mag;
            bodies[j].vx = bodies[j].vx + dx * body.mass * mag;
            bodies[j].vy = bodies[j].vy + dy * body.mass * mag;
            bodies[j].vz = bodies[j].vz + dz * body.mass * mag;
        }
    }

    for (var i = 0; i < 5; ++i) {
        bodies[i].x = bodies[i].x + 0.01 * bodies[i].vx;
        bodies[i].y = bodies[i].y + 0.01 * bodies[i].vy;
        bodies[i].z = bodies[i].z + 0.01 * bodies[i].vz;
    }
}

function energy () {
    var e = 0.0;
    for (var i = 0; i < 5; ++i) {
        e += 0.5 * bodies[i].mass * (bodies[i].vx * bodies[i].vx + bodies[i].vy * bodies[i].vy + bodies[i].vz * bodies[i].vz);
        for (j = i + 1; j < 5; ++j) {
            var dx = bodies[i].x - bodies[j].x;
            var dy = bodies[i].y - bodies[j].y;
            var dz = bodies[i].z - bodies[j].z;
            var distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            e -= (bodies[i].mass * bodies[j].mass) / distance;
        }
    }
    return e;
}

function offset_momentum () {
    var px = 0.0,
        py = 0.0,
        pz = 0.0;
    for (var i = 0; i < 5; ++i) {
        px += bodies[i].vx * bodies[i].mass;
        py += bodies[i].vy * bodies[i].mass;
        pz += bodies[i].vz * bodies[i].mass;
    }
    bodies[0].vx = -px / solar_mass;
    bodies[0].vy = -py / solar_mass;
    bodies[0].vz = -pz / solar_mass;
}

function benchmark () {
    init();
    offset_momentum();

    for (var k = 0; k <= 2000000; ++k) {
        advance();
    }

    var result = energy();

    var Taint = Polyglot.import("taint");
    result = Taint.removeTaint(result);

    for (var i = 0; i < 5; i++) {
        Taint.assertTainted(bodies[i].x);
        Taint.assertTainted(bodies[i].y);
        Taint.assertTainted(bodies[i].z);
    }

    tearDown();

    return result;
}

function getExpectedResult () {
    return -0.16902646009754382;
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
