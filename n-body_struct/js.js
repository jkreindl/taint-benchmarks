// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

const benchmarkName = "n-body_struct";

const Taint = Polyglot.import("taint");

var solar_mass;

const bodies = [
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
];

const x = 0;
const y = 1;
const z = 2;
const vx = 3;
const vy = 4;
const vz = 5;
const mass = 6;
const BODY_FIELDS = 7;

const SUN = 0 * BODY_FIELDS;
const JUPITER = 1 * BODY_FIELDS;
const SATURN = 2 * BODY_FIELDS;
const URANUS = 3 * BODY_FIELDS;
const NEPTUNE = 4 * BODY_FIELDS;

function init() {
    const pi = 3.141592653589793;
    solar_mass = (4 * pi * pi);
    const days_per_year = 365.24;

    bodies[SUN + x] = Taint.add(0.0);
    bodies[SUN + y] = Taint.add(0.0);
    bodies[SUN + z] = Taint.add(0.0);
    bodies[SUN + vx] = 0;
    bodies[SUN + vy] = 0;
    bodies[SUN + vz] = 0;
    bodies[SUN + mass] = solar_mass;
    bodies[JUPITER + x] = Taint.add(4.84143144246472090e+00);
    bodies[JUPITER + y] = Taint.add(-1.16032004402742839e+00);
    bodies[JUPITER + z] = Taint.add(-1.03622044471123109e-01);
    bodies[JUPITER + vx] = 1.66007664274403694e-03 * days_per_year;
    bodies[JUPITER + vy] = 7.69901118419740425e-03 * days_per_year;
    bodies[JUPITER + vz] = -6.90460016972063023e-05 * days_per_year;
    bodies[JUPITER + mass] = 9.54791938424326609e-04 * solar_mass;
    bodies[SATURN + x] = Taint.add(8.34336671824457987e+00);
    bodies[SATURN + y] = Taint.add(4.12479856412430479e+00);
    bodies[SATURN + z] = Taint.add(-4.03523417114321381e-01);
    bodies[SATURN + vx] = -2.76742510726862411e-03 * days_per_year;
    bodies[SATURN + vy] = 4.99852801234917238e-03 * days_per_year;
    bodies[SATURN + vz] = 2.30417297573763929e-05 * days_per_year;
    bodies[SATURN + mass] = 2.85885980666130812e-04 * solar_mass;
    bodies[URANUS + x] = Taint.add(1.28943695621391310e+01);
    bodies[URANUS + y] = Taint.add(-1.51111514016986312e+01);
    bodies[URANUS + z] = Taint.add(-2.23307578892655734e-01);
    bodies[URANUS + vx] = 2.96460137564761618e-03 * days_per_year;
    bodies[URANUS + vy] = 2.37847173959480950e-03 * days_per_year;
    bodies[URANUS + vz] = -2.96589568540237556e-05 * days_per_year;
    bodies[URANUS + mass] = 4.36624404335156298e-05 * solar_mass;
    bodies[NEPTUNE + x] = Taint.add(1.53796971148509165e+01);
    bodies[NEPTUNE + y] = Taint.add(-2.59193146099879641e+01);
    bodies[NEPTUNE + z] = Taint.add(1.79258772950371181e-01);
    bodies[NEPTUNE + vx] = 2.68067772490389322e-03 * days_per_year;
    bodies[NEPTUNE + vy] = 1.62824170038242295e-03 * days_per_year;
    bodies[NEPTUNE + vz] = -9.51592254519715870e-05 * days_per_year;
    bodies[NEPTUNE + mass] = 5.15138902046611451e-05 * solar_mass;
}

function tearDown() {
    for (let i = 0; i < 5; i++) {
        const body = i * BODY_FIELDS;
        bodies[body + x] = 0.0;
        bodies[body + y] = 0.0;
        bodies[body + z] = 0.0;
        bodies[body + vx] = 0.0;
        bodies[body + vy] = 0.0;
        bodies[body + vz] = 0.0;
        bodies[body + mass] = 0.0;
    }
}

function advance() {
    for (let i = 0; i < 5; ++i) {
        const body = i * BODY_FIELDS;
        for (let j = i + 1; j < 5; ++j) {
            const otherBody = j * BODY_FIELDS;
            const dx = bodies[body + x] - bodies[otherBody + x];
            let R = dx * dx;
            const dy = bodies[body + y] - bodies[otherBody + y];
            R += dy * dy;
            const dz = bodies[body + z] - bodies[otherBody + z];
            R += dz * dz;
            R = Math.sqrt(R);
            const mag = 0.01 / (R * R * R);
            bodies[body + vx] = bodies[body + vx] - dx * bodies[otherBody + mass] * mag;
            bodies[body + vy] = bodies[body + vy] - dy * bodies[otherBody + mass] * mag;
            bodies[body + vz] = bodies[body + vz] - dz * bodies[otherBody + mass] * mag;
            bodies[otherBody + vx] = bodies[otherBody + vx] + dx * bodies[body + mass] * mag;
            bodies[otherBody + vy] = bodies[otherBody + vy] + dy * bodies[body + mass] * mag;
            bodies[otherBody + vz] = bodies[otherBody + vz] + dz * bodies[body + mass] * mag;
        }
    }

    for (let i = 0; i < 5; ++i) {
        const body = i * BODY_FIELDS;
        bodies[body + x] = bodies[body + x] + 0.01 * bodies[body + vx];
        bodies[body + y] = bodies[body + y] + 0.01 * bodies[body + vy];
        bodies[body + z] = bodies[body + z] + 0.01 * bodies[body + vz];
    }
}

function energy() {
    let e = 0.0;
    for (let i = 0; i < 5; ++i) {
        const body = i * BODY_FIELDS;
        e += 0.5 * bodies[body + mass] * (bodies[body + vx] * bodies[body + vx] + bodies[body + vy] * bodies[body + vy] + bodies[body + vz] * bodies[body + vz]);
        for (let j = i + 1; j < 5; ++j) {
            const otherBody = j * BODY_FIELDS;
            const dx = bodies[body + x] - bodies[otherBody + x];
            const dy = bodies[body + y] - bodies[otherBody + y];
            const dz = bodies[body + z] - bodies[otherBody + z];
            const distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            e -= (bodies[body + mass] * bodies[otherBody + mass]) / distance;
        }
    }
    return e;
}

function offset_momentum() {
    let px = 0.0,
        py = 0.0,
        pz = 0.0;
    for (let i = 0; i < 5; ++i) {
        const body = i * BODY_FIELDS;
        px += bodies[body + vx] * bodies[body + mass];
        py += bodies[body + vy] * bodies[body + mass];
        pz += bodies[body + vz] * bodies[body + mass];
    }
    bodies[SUN + vx] = -px / solar_mass;
    bodies[SUN + vy] = -py / solar_mass;
    bodies[SUN + vz] = -pz / solar_mass;
}

function benchmark() {
    init();
    offset_momentum();

    for (let k = 0; k <= 2000000; ++k) {
        advance();
    }

    let result = energy();
    result = Taint.remove(result);

    for (let i = 0; i < 5; i++) {
        const body = i * BODY_FIELDS;
        Taint.assertTainted(bodies[body + x]);
        Taint.assertTainted(bodies[body + y]);
        Taint.assertTainted(bodies[body + z]);
    }

    tearDown();

    return result;
}

function getExpectedResult() {
    return -0.16902646009754382;
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

