// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

#define BENCHMARK_NAME "n-body_struct"
#define BENCHMARK_RESULT_TYPE 4
#include "../common.h"

#include <math.h>

const char *JS_ADVANCE =
    "(function advance(bodies) {\n"
    "    for (var i = 0; i < 5; ++i) {\n"
    "        var body = bodies[i];\n"
    "        for (var j = i + 1; j < 5; ++j) {\n"
    "            var dx = body.x - bodies[j].x;\n"
    "            var R = dx * dx;\n"
    "            var dy = body.y - bodies[j].y;\n"
    "            R += dy * dy;\n"
    "            var dz = body.z - bodies[j].z;\n"
    "            R += dz * dz;\n"
    "            R = Math.sqrt(R);\n"
    "            var mag = 0.01 / (R * R * R);\n"
    "            body.vx = body.vx - dx * bodies[j].mass * mag;\n"
    "            body.vy = body.vy - dy * bodies[j].mass * mag;\n"
    "            body.vz = body.vz - dz * bodies[j].mass * mag;\n"
    "            bodies[j].vx = bodies[j].vx + dx * body.mass * mag;\n"
    "            bodies[j].vy = bodies[j].vy + dy * body.mass * mag;\n"
    "            bodies[j].vz = bodies[j].vz + dz * body.mass * mag;\n"
    "        }\n"
    "    }\n"
    "    for (var i = 0; i < 5; ++i) {\n"
    "        bodies[i].x = bodies[i].x + 0.01 * bodies[i].vx;\n"
    "        bodies[i].y = bodies[i].y + 0.01 * bodies[i].vy;\n"
    "        bodies[i].z = bodies[i].z + 0.01 * bodies[i].vz;\n"
    "    }\n"
    "})\n";

const char *JS_ENERGY =
    "(function energy(bodies) {\n"
    "    var e = 0.0;\n"
    "    for (var i = 0; i < 5; ++i) {\n"
    "        e += 0.5 * bodies[i].mass * (bodies[i].vx * bodies[i].vx + "
    "bodies[i].vy * bodies[i].vy + bodies[i].vz * bodies[i].vz);\n"
    "        for (j = i + 1; j < 5; ++j) {\n"
    "            var dx = bodies[i].x - bodies[j].x;\n"
    "            var dy = bodies[i].y - bodies[j].y;\n"
    "            var dz = bodies[i].z - bodies[j].z;\n"
    "            var distance = Math.sqrt(dx * dx + dy * dy + dz * dz);\n"
    "            e -= (bodies[i].mass * bodies[j].mass) / distance;\n"
    "        }\n"
    "    }\n"
    "    return e;\n"
    "})\n";

struct Body
{
  double x;
  double y;
  double z;
  double vx;
  double vy;
  double vz;
  double mass;
};

void (*advance)(struct Body *);
double (*energy)(struct Body *);

double pi;
double solar_mass;
double days_per_year;

#define NBODIES 5
#define DT 0.01

#define SUN 0
#define JUPITER 1
#define SATURN 2
#define URANUS 3
#define NEPTUNE 4

struct Body *bodies;

void setup(void *arg)
{
  advance = polyglot_eval("js", JS_ADVANCE);
  energy = polyglot_eval("js", JS_ENERGY);
}

POLYGLOT_DECLARE_STRUCT(Body)

void init()
{
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  bodies = calloc(NBODIES, sizeof(struct Body));

  bodies[SUN].x = __truffletaint_add_double(0.0);
  bodies[SUN].y = __truffletaint_add_double(0.0);
  bodies[SUN].z = __truffletaint_add_double(0.0);
  bodies[SUN].vx = 0;
  bodies[SUN].vy = 0;
  bodies[SUN].vz = 0;
  bodies[SUN].mass = solar_mass;
  bodies[JUPITER].x = __truffletaint_add_double(4.84143144246472090e+00);
  bodies[JUPITER].y = __truffletaint_add_double(-1.16032004402742839e+00);
  bodies[JUPITER].z = __truffletaint_add_double(-1.03622044471123109e-01);
  bodies[JUPITER].vx = 1.66007664274403694e-03 * days_per_year;
  bodies[JUPITER].vy = 7.69901118419740425e-03 * days_per_year;
  bodies[JUPITER].vz = -6.90460016972063023e-05 * days_per_year;
  bodies[JUPITER].mass = 9.54791938424326609e-04 * solar_mass;
  bodies[SATURN].x = __truffletaint_add_double(8.34336671824457987e+00);
  bodies[SATURN].y = __truffletaint_add_double(4.12479856412430479e+00);
  bodies[SATURN].z = __truffletaint_add_double(-4.03523417114321381e-01);
  bodies[SATURN].vx = -2.76742510726862411e-03 * days_per_year;
  bodies[SATURN].vy = 4.99852801234917238e-03 * days_per_year;
  bodies[SATURN].vz = 2.30417297573763929e-05 * days_per_year;
  bodies[SATURN].mass = 2.85885980666130812e-04 * solar_mass;
  bodies[URANUS].x = __truffletaint_add_double(1.28943695621391310e+01);
  bodies[URANUS].y = __truffletaint_add_double(-1.51111514016986312e+01);
  bodies[URANUS].z = __truffletaint_add_double(-2.23307578892655734e-01);
  bodies[URANUS].vx = 2.96460137564761618e-03 * days_per_year;
  bodies[URANUS].vy = 2.37847173959480950e-03 * days_per_year;
  bodies[URANUS].vz = -2.96589568540237556e-05 * days_per_year;
  bodies[URANUS].mass = 4.36624404335156298e-05 * solar_mass;
  bodies[NEPTUNE].x = __truffletaint_add_double(1.53796971148509165e+01);
  bodies[NEPTUNE].y = __truffletaint_add_double(-2.59193146099879641e+01);
  bodies[NEPTUNE].z = __truffletaint_add_double(1.79258772950371181e-01);
  bodies[NEPTUNE].vx = 2.68067772490389322e-03 * days_per_year;
  bodies[NEPTUNE].vy = 1.62824170038242295e-03 * days_per_year;
  bodies[NEPTUNE].vz = -9.51592254519715870e-05 * days_per_year;
  bodies[NEPTUNE].mass = 5.15138902046611451e-05 * solar_mass;
}

void tearDown() { free(bodies); }

void offset_momentum()
{
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < NBODIES; ++i)
  {
    px += bodies[i].vx * bodies[i].mass;
    py += bodies[i].vy * bodies[i].mass;
    pz += bodies[i].vz * bodies[i].mass;
  }
  bodies[SUN].vx = -px / solar_mass;
  bodies[SUN].vy = -py / solar_mass;
  bodies[SUN].vz = -pz / solar_mass;
}

double benchmark()
{
  init();
  offset_momentum();

  void *polyglot_bodies = polyglot_from_Body_array(bodies, NBODIES);
  for (int k = 0; k < 2000000; k++)
    advance(polyglot_bodies);

  double result = energy(polyglot_bodies);
  result = __truffletaint_remove_double(result);

  for (int i = 0; i < NBODIES; i++)
  {
    __truffletaint_assert_double(bodies[i].x);
    __truffletaint_assert_double(bodies[i].y);
    __truffletaint_assert_double(bodies[i].z);
  }

  tearDown();

  return result;
}

double getExpectedResult() { return -0.16902628585296136; }
