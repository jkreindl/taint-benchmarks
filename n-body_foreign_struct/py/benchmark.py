"""
adapted from https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

contributed by Shakhno DV, Shakhno AV
"""

from polyglot import import_value
from math import sqrt
benchmarkName = "n-body_foreign_struct"

Taint = import_value("taint")

solar_mass = 0.0
pi = 3.141592653589793
solar_mass = (4 * pi * pi)
days_per_year = 365.24


class Body:
    pass


bodies = [Body(), Body(), Body(), Body(), Body()]


def init():
    SUN = 0
    JUPITER = 1
    SATURN = 2
    URANUS = 3
    NEPTUNE = 4

    bodies[SUN].x = Taint.add(0.0)
    bodies[SUN].y = Taint.add(0.0)
    bodies[SUN].z = Taint.add(0.0)
    bodies[SUN].vx = 0
    bodies[SUN].vy = 0
    bodies[SUN].vz = 0
    bodies[SUN].mass = solar_mass
    bodies[JUPITER].x = Taint.add(4.84143144246472090e+00)
    bodies[JUPITER].y = Taint.add(-1.16032004402742839e+00)
    bodies[JUPITER].z = Taint.add(-1.03622044471123109e-01)
    bodies[JUPITER].vx = 1.66007664274403694e-03 * days_per_year
    bodies[JUPITER].vy = 7.69901118419740425e-03 * days_per_year
    bodies[JUPITER].vz = -6.90460016972063023e-05 * days_per_year
    bodies[JUPITER].mass = 9.54791938424326609e-04 * solar_mass
    bodies[SATURN].x = Taint.add(8.34336671824457987e+00)
    bodies[SATURN].y = Taint.add(4.12479856412430479e+00)
    bodies[SATURN].z = Taint.add(-4.03523417114321381e-01)
    bodies[SATURN].vx = -2.76742510726862411e-03 * days_per_year
    bodies[SATURN].vy = 4.99852801234917238e-03 * days_per_year
    bodies[SATURN].vz = 2.30417297573763929e-05 * days_per_year
    bodies[SATURN].mass = 2.85885980666130812e-04 * solar_mass
    bodies[URANUS].x = Taint.add(1.28943695621391310e+01)
    bodies[URANUS].y = Taint.add(-1.51111514016986312e+01)
    bodies[URANUS].z = Taint.add(-2.23307578892655734e-01)
    bodies[URANUS].vx = 2.96460137564761618e-03 * days_per_year
    bodies[URANUS].vy = 2.37847173959480950e-03 * days_per_year
    bodies[URANUS].vz = -2.96589568540237556e-05 * days_per_year
    bodies[URANUS].mass = 4.36624404335156298e-05 * solar_mass
    bodies[NEPTUNE].x = Taint.add(1.53796971148509165e+01)
    bodies[NEPTUNE].y = Taint.add(-2.59193146099879641e+01)
    bodies[NEPTUNE].z = Taint.add(1.79258772950371181e-01)
    bodies[NEPTUNE].vx = 2.68067772490389322e-03 * days_per_year
    bodies[NEPTUNE].vy = 1.62824170038242295e-03 * days_per_year
    bodies[NEPTUNE].vz = -9.51592254519715870e-05 * days_per_year
    bodies[NEPTUNE].mass = 5.15138902046611451e-05 * solar_mass


def tearDown():
    for i in range(5):
        bodies[i].x = 0.0
        bodies[i].y = 0.0
        bodies[i].z = 0.0
        bodies[i].vx = 0.0
        bodies[i].vy = 0.0
        bodies[i].vz = 0.0
        bodies[i].mass = 0.0


def advance():
    for i in range(5):
        body = bodies[i]
        for j in range(i + 1, 5):
            dx = body.x - bodies[j].x
            R = dx * dx
            dy = body.y - bodies[j].y
            R += dy * dy
            dz = body.z - bodies[j].z
            R += dz * dz
            R = sqrt(R)
            mag = 0.01 / (R * R * R)
            body.vx = body.vx - dx * bodies[j].mass * mag
            body.vy = body.vy - dy * bodies[j].mass * mag
            body.vz = body.vz - dz * bodies[j].mass * mag
            bodies[j].vx = bodies[j].vx + dx * body.mass * mag
            bodies[j].vy = bodies[j].vy + dy * body.mass * mag
            bodies[j].vz = bodies[j].vz + dz * body.mass * mag

    for i in range(5):
        bodies[i].x = bodies[i].x + 0.01 * bodies[i].vx
        bodies[i].y = bodies[i].y + 0.01 * bodies[i].vy
        bodies[i].z = bodies[i].z + 0.01 * bodies[i].vz


def energy():
    e = 0.0
    for i in range(5):
        e += 0.5 * bodies[i].mass * (bodies[i].vx * bodies[i].vx +
                                     bodies[i].vy * bodies[i].vy +
                                     bodies[i].vz * bodies[i].vz)
        for j in range(i + 1, 5):
            dx = bodies[i].x - bodies[j].x
            dy = bodies[i].y - bodies[j].y
            dz = bodies[i].z - bodies[j].z
            distance = sqrt(dx * dx + dy * dy + dz * dz)
            e -= (bodies[i].mass * bodies[j].mass) / distance

    return e


def offset_momentum():
    px = 0.0
    py = 0.0
    pz = 0.0
    for i in range(5):
        px += bodies[i].vx * bodies[i].mass
        py += bodies[i].vy * bodies[i].mass
        pz += bodies[i].vz * bodies[i].mass

    bodies[0].vx = -px / solar_mass
    bodies[0].vy = -py / solar_mass
    bodies[0].vz = -pz / solar_mass


def benchmark():
    init()
    offset_momentum()

    for k in range(250000):
        advance()

    result = energy()

    result = Taint.remove(result)

    for i in range(5):
        Taint.assertTainted(bodies[i].x)
        Taint.assertTainted(bodies[i].y)
        Taint.assertTainted(bodies[i].z)

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
