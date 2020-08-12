// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

const char *PY_ADVANCE =
    "from math import sqrt\n"
    "\n"
    "def advance(bodies):\n"
    "    for i in range(5):\n"
    "        body = bodies[i]\n"
    "        for j in range(i + 1, 5):\n"
    "            dx = body.x - bodies[j].x\n"
    "            R = dx * dx\n"
    "            dy = body.y - bodies[j].y\n"
    "            R += dy * dy\n"
    "            dz = body.z - bodies[j].z\n"
    "            R += dz * dz\n"
    "            R = sqrt(R)\n"
    "            mag = 0.01 / (R * R * R)\n"
    "            body.vx = body.vx - dx * bodies[j].mass * mag\n"
    "            body.vy = body.vy - dy * bodies[j].mass * mag\n"
    "            body.vz = body.vz - dz * bodies[j].mass * mag\n"
    "            bodies[j].vx = bodies[j].vx + dx * body.mass * mag\n"
    "            bodies[j].vy = bodies[j].vy + dy * body.mass * mag\n"
    "            bodies[j].vz = bodies[j].vz + dz * body.mass * mag\n"
    "    \n"
    "    for i in range(5):\n"
    "        bodies[i].x = bodies[i].x + 0.01 * bodies[i].vx\n"
    "        bodies[i].y = bodies[i].y + 0.01 * bodies[i].vy\n"
    "        bodies[i].z = bodies[i].z + 0.01 * bodies[i].vz\n"
    "\n"
    "advance\n";

const char *PY_ENERGY =
    "from math import sqrt\n"
    "\n"
    "def energy(bodies):\n"
    "    e = 0.0\n"
    "    for i in range(5):\n"
    "        e += 0.5 * bodies[i].mass * (bodies[i].vx * bodies[i].vx + "
    "bodies[i].vy * bodies[i].vy + bodies[i].vz * bodies[i].vz)\n"
    "        for j in range(i + 1, 5):\n"
    "            dx = bodies[i].x - bodies[j].x\n"
    "            dy = bodies[i].y - bodies[j].y\n"
    "            dz = bodies[i].z - bodies[j].z\n"
    "            distance = sqrt(dx * dx + dy * dy + dz * dz)\n"
    "            e -= (bodies[i].mass * bodies[j].mass) / distance\n"
    "    return e\n"
    "\n"
    "energy\n";

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
struct Body *bodies;

void setup(void *arg)
{
  advance = polyglot_eval("python", PY_ADVANCE);
  energy = polyglot_eval("python", PY_ENERGY);
}

POLYGLOT_DECLARE_STRUCT(Body)

void init()
{
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  bodies = calloc(5, sizeof(struct Body));

  bodies[0].x = __truffletaint_add_double(0.0);
  bodies[0].y = __truffletaint_add_double(0.0);
  bodies[0].z = __truffletaint_add_double(0.0);
  bodies[0].vx = 0;
  bodies[0].vy = 0;
  bodies[0].vz = 0;
  bodies[0].mass = solar_mass;
  bodies[1].x = __truffletaint_add_double(4.84143144246472090e+00);
  bodies[1].y = __truffletaint_add_double(-1.16032004402742839e+00);
  bodies[1].z = __truffletaint_add_double(-1.03622044471123109e-01);
  bodies[1].vx = 1.66007664274403694e-03 * days_per_year;
  bodies[1].vy = 7.69901118419740425e-03 * days_per_year;
  bodies[1].vz = -6.90460016972063023e-05 * days_per_year;
  bodies[1].mass = 9.54791938424326609e-04 * solar_mass;
  bodies[2].x = __truffletaint_add_double(8.34336671824457987e+00);
  bodies[2].y = __truffletaint_add_double(4.12479856412430479e+00);
  bodies[2].z = __truffletaint_add_double(-4.03523417114321381e-01);
  bodies[2].vx = -2.76742510726862411e-03 * days_per_year;
  bodies[2].vy = 4.99852801234917238e-03 * days_per_year;
  bodies[2].vz = 2.30417297573763929e-05 * days_per_year;
  bodies[2].mass = 2.85885980666130812e-04 * solar_mass;
  bodies[3].x = __truffletaint_add_double(1.28943695621391310e+01);
  bodies[3].y = __truffletaint_add_double(-1.51111514016986312e+01);
  bodies[3].z = __truffletaint_add_double(-2.23307578892655734e-01);
  bodies[3].vx = 2.96460137564761618e-03 * days_per_year;
  bodies[3].vy = 2.37847173959480950e-03 * days_per_year;
  bodies[3].vz = -2.96589568540237556e-05 * days_per_year;
  bodies[3].mass = 4.36624404335156298e-05 * solar_mass;
  bodies[4].x = __truffletaint_add_double(1.53796971148509165e+01);
  bodies[4].y = __truffletaint_add_double(-2.59193146099879641e+01);
  bodies[4].z = __truffletaint_add_double(1.79258772950371181e-01);
  bodies[4].vx = 2.68067772490389322e-03 * days_per_year;
  bodies[4].vy = 1.62824170038242295e-03 * days_per_year;
  bodies[4].vz = -9.51592254519715870e-05 * days_per_year;
  bodies[4].mass = 5.15138902046611451e-05 * solar_mass;
}

void tearDown() { free(bodies); }

void offset_momentum()
{
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < 5; ++i)
  {
    px += bodies[i].vx * bodies[i].mass;
    py += bodies[i].vy * bodies[i].mass;
    pz += bodies[i].vz * bodies[i].mass;
  }
  bodies[0].vx = -px / solar_mass;
  bodies[0].vy = -py / solar_mass;
  bodies[0].vz = -pz / solar_mass;
}

double benchmark()
{
  init();
  offset_momentum();

  void *polyglot_bodies = polyglot_from_Body_array(bodies, 5);
  for (int k = 0; k < 250000; k++)
    advance(polyglot_bodies);

  double result = energy(polyglot_bodies);
  result = __truffletaint_remove_double(result);

  for (int i = 0; i < 5; i++)
  {
    __truffletaint_assert_double(bodies[i].x);
    __truffletaint_assert_double(bodies[i].y);
    __truffletaint_assert_double(bodies[i].z);
  }

  tearDown();

  return result;
}

double getExpectedResult() { return -0.1690859889909308; }

void *createDefaultBenchmarkArg()
{
  return polyglot_import("benchmarkIO");
}

#include <sys/time.h>
int main(int argc, char **argv)
{
  setup(createDefaultBenchmarkArg());

  unsigned int iterations = 0xFFFFFFFF;
  if (argc == 2)
  {
    int explicitIterations = atoi(argv[1]);
    if (explicitIterations < 0)
    {
      fprintf(stderr, "invalid iteration count: " + explicitIterations);
      exit(1);
    }
    else
    {
      iterations = explicitIterations;
    }
  }

  printf("starting benchmark: "
         "n-body_struct"
         "\n");
  struct timeval start, end;
  for (unsigned int i = 0; i < iterations; i++)
  {
    gettimeofday(&start, ((void *)0));
    double result = benchmark();
    gettimeofday(&end, ((void *)0));
    double s = (double)(end.tv_usec - start.tv_usec) / 1000000 +
               (double)(end.tv_sec - start.tv_sec);
    printf(
        "iteration %d took %f seconds and gave "
        "%f"
        "\n",
        i, s, result);
  }
  return 0;
}
