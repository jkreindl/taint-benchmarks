"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

contributed by Shakhno DV, Shakhno AV
"""

from polyglot import import_value
from math import sqrt
benchmarkName = "n-body"

Taint = import_value("taint")

solar_mass = 0.0
pi = 3.141592653589793
solar_mass = (4 * pi * pi)
days_per_year = 365.24

x = [0.0, 0.0, 0.0, 0.0, 0.0]
y = [0.0, 0.0, 0.0, 0.0, 0.0]
z = [0.0, 0.0, 0.0, 0.0, 0.0]
vx = [0.0, 0.0, 0.0, 0.0, 0.0]
vy = [0.0, 0.0, 0.0, 0.0, 0.0]
vz = [0.0, 0.0, 0.0, 0.0, 0.0]
mass = [0.0, 0.0, 0.0, 0.0, 0.0]


def get_x(nBody):
    return x[nBody]


def set_x(nBody, value):
    x[nBody] = value


def get_y(nBody):
    return y[nBody]


def set_y(nBody, value):
    y[nBody] = value


def get_z(nBody):
    return z[nBody]


def set_z(nBody, value):
    z[nBody] = value


def get_vx(nBody):
    return vx[nBody]


def set_vx(nBody, value):
    vx[nBody] = value


def get_vy(nBody):
    return vy[nBody]


def set_vy(nBody, value):
    vy[nBody] = value


def get_vz(nBody):
    return vz[nBody]


def set_vz(nBody, value):
    vz[nBody] = value


def get_mass(nBody):
    return mass[nBody]


def set_mass(nBody, value):
    mass[nBody] = value


def init():
    SUN = 0
    JUPITER = 1
    SATURN = 2
    URANUS = 3
    NEPTUNE = 4

    set_x(SUN, Taint.add(0.0))
    set_y(SUN, Taint.add(0.0))
    set_z(SUN, Taint.add(0.0))
    set_vx(SUN, 0)
    set_vy(SUN, 0)
    set_vz(SUN, 0)
    set_mass(SUN, solar_mass)
    set_x(JUPITER, Taint.add(4.84143144246472090e+00))
    set_y(JUPITER, Taint.add(-1.16032004402742839e+00))
    set_z(JUPITER, Taint.add(-1.03622044471123109e-01))
    set_vx(JUPITER, 1.66007664274403694e-03 * days_per_year)
    set_vy(JUPITER, 7.69901118419740425e-03 * days_per_year)
    set_vz(JUPITER, -6.90460016972063023e-05 * days_per_year)
    set_mass(JUPITER, 9.54791938424326609e-04 * solar_mass)
    set_x(SATURN, Taint.add(8.34336671824457987e+00))
    set_y(SATURN, Taint.add(4.12479856412430479e+00))
    set_z(SATURN, Taint.add(-4.03523417114321381e-01))
    set_vx(SATURN, -2.76742510726862411e-03 * days_per_year)
    set_vy(SATURN, 4.99852801234917238e-03 * days_per_year)
    set_vz(SATURN, 2.30417297573763929e-05 * days_per_year)
    set_mass(SATURN, 2.85885980666130812e-04 * solar_mass)
    set_x(URANUS, Taint.add(1.28943695621391310e+01))
    set_y(URANUS, Taint.add(-1.51111514016986312e+01))
    set_z(URANUS, Taint.add(-2.23307578892655734e-01))
    set_vx(URANUS, 2.96460137564761618e-03 * days_per_year)
    set_vy(URANUS, 2.37847173959480950e-03 * days_per_year)
    set_vz(URANUS, -2.96589568540237556e-05 * days_per_year)
    set_mass(URANUS, 4.36624404335156298e-05 * solar_mass)
    set_x(NEPTUNE, Taint.add(1.53796971148509165e+01))
    set_y(NEPTUNE, Taint.add(-2.59193146099879641e+01))
    set_z(NEPTUNE, Taint.add(1.79258772950371181e-01))
    set_vx(NEPTUNE, 2.68067772490389322e-03 * days_per_year)
    set_vy(NEPTUNE, 1.62824170038242295e-03 * days_per_year)
    set_vz(NEPTUNE, -9.51592254519715870e-05 * days_per_year)
    set_mass(NEPTUNE, 5.15138902046611451e-05 * solar_mass)


def tearDown():
    for i in range(5):
        set_x(i, 0.0)
        set_y(i, 0.0)
        set_z(i, 0.0)
        set_vx(i, 0.0)
        set_vy(i, 0.0)
        set_vz(i, 0.0)
        set_mass(i, 0.0)


def advance():
    for i in range(5):
        x1 = get_x(i)
        y1 = get_y(i)
        z1 = get_z(i)
        for j in range(i + 1, 5):
            dx = x1 - get_x(j)
            R = dx * dx
            dy = y1 - get_y(j)
            R += dy * dy
            dz = z1 - get_z(j)
            R += dz * dz
            R = sqrt(R)
            mag = 0.01 / (R * R * R)
            set_vx(i, get_vx(i) - dx * get_mass(j) * mag)
            set_vy(i, get_vy(i) - dy * get_mass(j) * mag)
            set_vz(i, get_vz(i) - dz * get_mass(j) * mag)
            set_vx(j, get_vx(j) + dx * get_mass(i) * mag)
            set_vy(j, get_vy(j) + dy * get_mass(i) * mag)
            set_vz(j, get_vz(j) + dz * get_mass(i) * mag)

    for i in range(5):
        set_x(i, get_x(i) + 0.01 * get_vx(i))
        set_y(i, get_y(i) + 0.01 * get_vy(i))
        set_z(i, get_z(i) + 0.01 * get_vz(i))


def energy():
    e = 0.0
    for i in range(5):
        e += 0.5 * get_mass(i) * (get_vx(i) * get_vx(i) + get_vy(i) * get_vy(i)
                                  + get_vz(i) * get_vz(i))
        for j in range(i + 1, 5):
            dx = get_x(i) - get_x(j)
            dy = get_y(i) - get_y(j)
            dz = get_z(i) - get_z(j)
            distance = sqrt(dx * dx + dy * dy + dz * dz)
            e -= (get_mass(i) * get_mass(j)) / distance
    return e


def offset_momentum():
    px = 0.0
    py = 0.0
    pz = 0.0
    for i in range(5):
        px += get_vx(i) * get_mass(i)
        py += get_vy(i) * get_mass(i)
        pz += get_vz(i) * get_mass(i)
    set_vx(0, -px / solar_mass)
    set_vy(0, -py / solar_mass)
    set_vz(0, -pz / solar_mass)


def benchmark():
    init()
    offset_momentum()

    for k in range(250000):
        advance()

    result = energy()

    result = Taint.remove(result)

    for i in range(5):
        Taint.assertTainted(get_x(i))
        Taint.assertTainted(get_y(i))
        Taint.assertTainted(get_z(i))

    tearDown()

    return result


def getExpectedResult():
    return -0.16908598899092508


def test():
    actualResult = benchmark()
    if actualResult == getExpectedResult():
        return 0
    else:
        return 1


def setup(arg):
    pass


try:
    assert isinstance(benchmarkName, str), "'benchmarkName' is invalid"
except NameError:
    raise AssertionError("'benchmark' is not defined")

try:
    assert callable(benchmark), "'benchmark' is not callable"
except NameError:
    raise AssertionError("'benchmark' is not defined")


def main():
    from polyglot import import_value
    benchmarkIO = import_value("benchmarkIO")
    setup(benchmarkIO)

    import time
    print("starting benchmark: " + benchmarkName)
    for iteration in range(0x7FFFFFFF):
        start = time.time()
        result = benchmark()
        end = time.time()
        s = end - start
        print("iteration " + str(iteration) + " took " +
              str(s) + " seconds and gave " + str(result))

    return 0


main()
