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

double pi;
double solar_mass;
double days_per_year;
const char *JS_CREATE_ARRAY =
    "function createJsArray() { return [0.0, 0.0, 0.0, 0.0, 0.0]; }\n"
    "(createJsArray)\n";

void *(*createJsArray)();

double *createStorageArray()
{
  void *jsArr = createJsArray();
  double *jsArrAsDoubleArr = polyglot_as_double_array(jsArr);
  return jsArrAsDoubleArr;
}
double *x = ((void *)0);
double *y = ((void *)0);
double *z = ((void *)0);
double *vx = ((void *)0);
double *vy = ((void *)0);
double *vz = ((void *)0);
double *mass = ((void *)0);
;

void setup(void *arg) { createJsArray = polyglot_eval("js", JS_CREATE_ARRAY); }

void init()
{
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  x = createStorageArray();
  y = createStorageArray();
  z = createStorageArray();
  vx = createStorageArray();
  vy = createStorageArray();
  vz = createStorageArray();
  mass = createStorageArray();

  x[0] = y[0] = z[0] = __truffletaint_add_double(0.0);
  vx[0] = vy[0] = vz[0] = 0.0;
  mass[0] = solar_mass;
  x[1] = __truffletaint_add_double(4.84143144246472090e+00);
  y[1] = __truffletaint_add_double(-1.16032004402742839e+00);
  z[1] = __truffletaint_add_double(-1.03622044471123109e-01);
  vx[1] = 1.66007664274403694e-03 * days_per_year;
  vy[1] = 7.69901118419740425e-03 * days_per_year;
  vz[1] = -6.90460016972063023e-05 * days_per_year;
  mass[1] = 9.54791938424326609e-04 * solar_mass;

  x[2] = __truffletaint_add_double(8.34336671824457987e+00);
  y[2] = __truffletaint_add_double(4.12479856412430479e+00);
  z[2] = __truffletaint_add_double(-4.03523417114321381e-01);
  vx[2] = -2.76742510726862411e-03 * days_per_year;
  vy[2] = 4.99852801234917238e-03 * days_per_year;
  vz[2] = 2.30417297573763929e-05 * days_per_year;
  mass[2] = 2.85885980666130812e-04 * solar_mass;

  x[3] = __truffletaint_add_double(1.28943695621391310e+01);
  y[3] = __truffletaint_add_double(-1.51111514016986312e+01);
  z[3] = __truffletaint_add_double(-2.23307578892655734e-01);
  vx[3] = 2.96460137564761618e-03 * days_per_year;
  vy[3] = 2.37847173959480950e-03 * days_per_year;
  vz[3] = -2.96589568540237556e-05 * days_per_year;
  mass[3] = 4.36624404335156298e-05 * solar_mass;

  x[4] = __truffletaint_add_double(1.53796971148509165e+01);
  y[4] = __truffletaint_add_double(-2.59193146099879641e+01);
  z[4] = __truffletaint_add_double(1.79258772950371181e-01);
  vx[4] = 2.68067772490389322e-03 * days_per_year;
  vy[4] = 1.62824170038242295e-03 * days_per_year;
  vz[4] = -9.51592254519715870e-05 * days_per_year;
  mass[4] = 5.15138902046611451e-05 * solar_mass;
}

void tearDown()
{
  for (int i = 0; i < 5; i++)
  {
    x[i] = y[i] = z[i] = vx[i] = vy[i] = vz[i] = mass[i] = 0.0;
  }

  x = ((void *)0);
  y = ((void *)0);
  z = ((void *)0);
  vx = ((void *)0);
  vy = ((void *)0);
  vz = ((void *)0);
  mass = ((void *)0);
  ;
}

void advance()
{
  for (int i = 0; i < 5; ++i)
  {
    double x1 = x[i];
    double y1 = y[i];
    double z1 = z[i];
    for (int j = i + 1; j < 5; ++j)
    {
      double dx = x1 - x[j];
      double R = dx * dx;
      double dy = y1 - y[j];
      R += dy * dy;
      double dz = z1 - z[j];
      R += dz * dz;
      R = sqrt(R);
      double mag = 0.01 / (R * R * R);
      vx[i] = vx[i] - dx * mass[j] * mag;
      vy[i] = vy[i] - dy * mass[j] * mag;
      vz[i] = vz[i] - dz * mass[j] * mag;
      vx[j] = vx[j] + dx * mass[i] * mag;
      vy[j] = vy[j] + dy * mass[i] * mag;
      vz[j] = vz[j] + dz * mass[i] * mag;
    }
  }

  for (int i = 0; i < 5; ++i)
  {
    x[i] = x[i] + 0.01 * vx[i];
    y[i] = y[i] + 0.01 * vy[i];
    z[i] = z[i] + 0.01 * vz[i];
  }
}

double energy()
{
  double e = 0.0;
  for (int i = 0; i < 5; ++i)
  {
    e += 0.5 * mass[i] * (vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i]);
    for (int j = i + 1; j < 5; ++j)
    {
      double dx = x[i] - x[j];
      double dy = y[i] - y[j];
      double dz = z[i] - z[j];
      double distance = sqrt(dx * dx + dy * dy + dz * dz);
      e -= (mass[i] * mass[j]) / distance;
    }
  }
  return e;
}

void offset_momentum()
{
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < 5; ++i)
  {
    px += vx[i] * mass[i];
    py += vy[i] * mass[i];
    pz += vz[i] * mass[i];
  }
  vx[0] = -px / solar_mass;
  vy[0] = -py / solar_mass;
  vz[0] = -pz / solar_mass;
}

double benchmark()
{
  init();
  offset_momentum();

  for (int k = 0; k < 250000; k++)
    advance();

  double result = energy();
  result = __truffletaint_remove_double(result);

  for (int i = 0; i < 5; i++)
  {
    __truffletaint_assert_double(x[i]);
    __truffletaint_assert_double(y[i]);
    __truffletaint_assert_double(z[i]);
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
         "n-body"
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
