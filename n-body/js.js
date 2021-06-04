// adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

var benchmarkName = "n-body";

var solar_mass;

var x = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_x (nBody) {
    return x[nBody];
}

function set_x (nBody, value) {
    x[nBody] = value;
}

var y = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_y (nBody) {
    return y[nBody];
}

function set_y (nBody, value) {
    y[nBody] = value;
}

var z = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_z (nBody) {
    return z[nBody];
}

function set_z (nBody, value) {
    z[nBody] = value;
}

var vx = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_vx (nBody) {
    return vx[nBody];
}

function set_vx (nBody, value) {
    vx[nBody] = value;
}

var vy = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_vy (nBody) {
    return vy[nBody];
}

function set_vy (nBody, value) {
    vy[nBody] = value;
}

var vz = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_vz (nBody) {
    return vz[nBody];
}

function set_vz (nBody, value) {
    vz[nBody] = value;
}

var mass = Array(0.0, 0.0, 0.0, 0.0, 0.0);

function get_mass (nBody) {
    return mass[nBody];
}

function set_mass (nBody, value) {
    mass[nBody] = value;
}

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

    set_x(SUN, Taint.addTaint(0.0));
    set_y(SUN, Taint.addTaint(0.0));
    set_z(SUN, Taint.addTaint(0.0));
    set_vx(SUN, 0);
    set_vy(SUN, 0);
    set_vz(SUN, 0);
    set_mass(SUN, solar_mass);
    set_x(JUPITER, Taint.addTaint(4.84143144246472090e+00));
    set_y(JUPITER, Taint.addTaint(-1.16032004402742839e+00));
    set_z(JUPITER, Taint.addTaint(-1.03622044471123109e-01));
    set_vx(JUPITER, 1.66007664274403694e-03 * days_per_year);
    set_vy(JUPITER, 7.69901118419740425e-03 * days_per_year);
    set_vz(JUPITER, -6.90460016972063023e-05 * days_per_year);
    set_mass(JUPITER, 9.54791938424326609e-04 * solar_mass);
    set_x(SATURN, Taint.addTaint(8.34336671824457987e+00));
    set_y(SATURN, Taint.addTaint(4.12479856412430479e+00));
    set_z(SATURN, Taint.addTaint(-4.03523417114321381e-01));
    set_vx(SATURN, -2.76742510726862411e-03 * days_per_year);
    set_vy(SATURN, 4.99852801234917238e-03 * days_per_year);
    set_vz(SATURN, 2.30417297573763929e-05 * days_per_year);
    set_mass(SATURN, 2.85885980666130812e-04 * solar_mass);
    set_x(URANUS, Taint.addTaint(1.28943695621391310e+01));
    set_y(URANUS, Taint.addTaint(-1.51111514016986312e+01));
    set_z(URANUS, Taint.addTaint(-2.23307578892655734e-01));
    set_vx(URANUS, 2.96460137564761618e-03 * days_per_year);
    set_vy(URANUS, 2.37847173959480950e-03 * days_per_year);
    set_vz(URANUS, -2.96589568540237556e-05 * days_per_year);
    set_mass(URANUS, 4.36624404335156298e-05 * solar_mass);
    set_x(NEPTUNE, Taint.addTaint(1.53796971148509165e+01));
    set_y(NEPTUNE, Taint.addTaint(-2.59193146099879641e+01));
    set_z(NEPTUNE, Taint.addTaint(1.79258772950371181e-01));
    set_vx(NEPTUNE, 2.68067772490389322e-03 * days_per_year);
    set_vy(NEPTUNE, 1.62824170038242295e-03 * days_per_year);
    set_vz(NEPTUNE, -9.51592254519715870e-05 * days_per_year);
    set_mass(NEPTUNE, 5.15138902046611451e-05 * solar_mass);
}

function tearDown () {
    for (var i = 0; i < 5; i++) {
        set_x(i, 0.0);
        set_y(i, 0.0);
        set_z(i, 0.0);
        set_vx(i, 0.0);
        set_vy(i, 0.0);
        set_vz(i, 0.0);
        set_mass(i, 0.0);
    }
}

function advance () {
    for (var i = 0; i < 5; ++i) {
        var x1 = get_x(i);
        var y1 = get_y(i);
        var z1 = get_z(i);
        for (var j = i + 1; j < 5; ++j) {
            var dx = x1 - get_x(j);
            var R = dx * dx;
            var dy = y1 - get_y(j);
            R += dy * dy;
            var dz = z1 - get_z(j);
            R += dz * dz;
            R = Math.sqrt(R);
            var mag = 0.01 / (R * R * R);
            set_vx(i, get_vx(i) - dx * get_mass(j) * mag);
            set_vy(i, get_vy(i) - dy * get_mass(j) * mag);
            set_vz(i, get_vz(i) - dz * get_mass(j) * mag);
            set_vx(j, get_vx(j) + dx * get_mass(i) * mag);
            set_vy(j, get_vy(j) + dy * get_mass(i) * mag);
            set_vz(j, get_vz(j) + dz * get_mass(i) * mag);
        }
    }

    for (var i = 0; i < 5; ++i) {
        set_x(i, get_x(i) + 0.01 * get_vx(i));
        set_y(i, get_y(i) + 0.01 * get_vy(i));
        set_z(i, get_z(i) + 0.01 * get_vz(i));
    }
}

function energy () {
    var e = 0.0;
    for (var i = 0; i < 5; ++i) {
        e += 0.5 * get_mass(i) * (get_vx(i) * get_vx(i) + get_vy(i) * get_vy(i) + get_vz(i) * get_vz(i));
        for (var j = i + 1; j < 5; ++j) {
            var dx = get_x(i) - get_x(j);
            var dy = get_y(i) - get_y(j);
            var dz = get_z(i) - get_z(j);
            var distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            e -= (get_mass(i) * get_mass(j)) / distance;
        }
    }
    return e;
}

function offset_momentum () {
    var px = 0.0,
        py = 0.0,
        pz = 0.0;
    for (var i = 0; i < 5; ++i) {
        px += get_vx(i) * get_mass(i);
        py += get_vy(i) * get_mass(i);
        pz += get_vz(i) * get_mass(i);
    }
    set_vx(0, -px / solar_mass);
    set_vy(0, -py / solar_mass);
    set_vz(0, -pz / solar_mass);
}

/*
int main(int argc, char **argv)
{
    int n = atoi(argv[1]);
    init();
    offset_momentum();
    printf("%.9f\n", energy());
    advance(n);
    printf("%.9f\n", energy());
    return 0;
}
*/

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
        Taint.assertTainted(get_x(i));
        Taint.assertTainted(get_y(i));
        Taint.assertTainted(get_z(i));
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
