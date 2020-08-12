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

double *x;
double *y;
double *z;
double *vx;
double *vy;
double *vz;
double *mass;

double get_x(int nBody) { return x[nBody]; }
void set_x(int nBody, double value) { x[nBody] = value; }
double get_y(int nBody) { return y[nBody]; }
void set_y(int nBody, double value) { y[nBody] = value; }
double get_z(int nBody) { return z[nBody]; }
void set_z(int nBody, double value) { z[nBody] = value; }
double get_vx(int nBody) { return vx[nBody]; }
void set_vx(int nBody, double value) { vx[nBody] = value; }
double get_vy(int nBody) { return vy[nBody]; }
void set_vy(int nBody, double value) { vy[nBody] = value; }
double get_vz(int nBody) { return vz[nBody]; }
void set_vz(int nBody, double value) { vz[nBody] = value; }
double get_mass(int nBody) { return mass[nBody]; }
void set_mass(int nBody, double value) { mass[nBody] = value; }

void init()
{
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  x = calloc(5, sizeof(double));
  y = calloc(5, sizeof(double));
  z = calloc(5, sizeof(double));
  vx = calloc(5, sizeof(double));
  vy = calloc(5, sizeof(double));
  vz = calloc(5, sizeof(double));
  mass = calloc(5, sizeof(double));

  set_x(0, __truffletaint_add_double(0.0));
  set_y(0, __truffletaint_add_double(0.0));
  set_z(0, __truffletaint_add_double(0.0));
  set_vx(0, 0);
  set_vy(0, 0);
  set_vz(0, 0);
  set_mass(0, solar_mass);
  set_x(1, __truffletaint_add_double(4.84143144246472090e+00));
  set_y(1, __truffletaint_add_double(-1.16032004402742839e+00));
  set_z(1, __truffletaint_add_double(-1.03622044471123109e-01));
  set_vx(1, 1.66007664274403694e-03 * days_per_year);
  set_vy(1, 7.69901118419740425e-03 * days_per_year);
  set_vz(1, -6.90460016972063023e-05 * days_per_year);
  set_mass(1, 9.54791938424326609e-04 * solar_mass);
  set_x(2, __truffletaint_add_double(8.34336671824457987e+00));
  set_y(2, __truffletaint_add_double(4.12479856412430479e+00));
  set_z(2, __truffletaint_add_double(-4.03523417114321381e-01));
  set_vx(2, -2.76742510726862411e-03 * days_per_year);
  set_vy(2, 4.99852801234917238e-03 * days_per_year);
  set_vz(2, 2.30417297573763929e-05 * days_per_year);
  set_mass(2, 2.85885980666130812e-04 * solar_mass);
  set_x(3, __truffletaint_add_double(1.28943695621391310e+01));
  set_y(3, __truffletaint_add_double(-1.51111514016986312e+01));
  set_z(3, __truffletaint_add_double(-2.23307578892655734e-01));
  set_vx(3, 2.96460137564761618e-03 * days_per_year);
  set_vy(3, 2.37847173959480950e-03 * days_per_year);
  set_vz(3, -2.96589568540237556e-05 * days_per_year);
  set_mass(3, 4.36624404335156298e-05 * solar_mass);
  set_x(4, __truffletaint_add_double(1.53796971148509165e+01));
  set_y(4, __truffletaint_add_double(-2.59193146099879641e+01));
  set_z(4, __truffletaint_add_double(1.79258772950371181e-01));
  set_vx(4, 2.68067772490389322e-03 * days_per_year);
  set_vy(4, 1.62824170038242295e-03 * days_per_year);
  set_vz(4, -9.51592254519715870e-05 * days_per_year);
  set_mass(4, 5.15138902046611451e-05 * solar_mass);
}

void tearDown()
{
  for (int i = 0; i < 5; i++)
  {
    x[i] = y[i] = z[i] = vx[i] = vy[i] = vz[i] = mass[i] = 0.0;
  }

  free(x);
  free(y);
  free(z);
  free(vx);
  free(vy);
  free(vz);
  free(mass);
}

void advance()
{
  for (int i = 0; i < 5; ++i)
  {
    double x1 = get_x(i);
    double y1 = get_y(i);
    double z1 = get_z(i);
    for (int j = i + 1; j < 5; ++j)
    {
      double dx = x1 - get_x(j);
      double R = dx * dx;
      double dy = y1 - get_y(j);
      R += dy * dy;
      double dz = z1 - get_z(j);
      R += dz * dz;
      R = sqrt(R);
      double mag = 0.01 / (R * R * R);
      set_vx(i, get_vx(i) - dx * get_mass(j) * mag);
      set_vy(i, get_vy(i) - dy * get_mass(j) * mag);
      set_vz(i, get_vz(i) - dz * get_mass(j) * mag);
      set_vx(j, get_vx(j) + dx * get_mass(i) * mag);
      set_vy(j, get_vy(j) + dy * get_mass(i) * mag);
      set_vz(j, get_vz(j) + dz * get_mass(i) * mag);
    }
  }

  for (int i = 0; i < 5; ++i)
  {
    set_x(i, get_x(i) + 0.01 * get_vx(i));
    set_y(i, get_y(i) + 0.01 * get_vy(i));
    set_z(i, get_z(i) + 0.01 * get_vz(i));
  }
}

double energy()
{
  double e = 0.0;
  for (int i = 0; i < 5; ++i)
  {
    e +=
        0.5 * get_mass(i) *
        (get_vx(i) * get_vx(i) + get_vy(i) * get_vy(i) + get_vz(i) * get_vz(i));
    for (int j = i + 1; j < 5; ++j)
    {
      double dx = get_x(i) - get_x(j);
      double dy = get_y(i) - get_y(j);
      double dz = get_z(i) - get_z(j);
      double distance = sqrt(dx * dx + dy * dy + dz * dz);
      e -= (get_mass(i) * get_mass(j)) / distance;
    }
  }
  return e;
}

void offset_momentum()
{
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < 5; ++i)
  {
    px += get_vx(i) * get_mass(i);
    py += get_vy(i) * get_mass(i);
    pz += get_vz(i) * get_mass(i);
  }
  set_vx(0, -px / solar_mass);
  set_vy(0, -py / solar_mass);
  set_vz(0, -pz / solar_mass);
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
    __truffletaint_assert_double(get_x(i));
    __truffletaint_assert_double(get_y(i));
    __truffletaint_assert_double(get_z(i));
  }

  tearDown();

  return result;
}

double getExpectedResult() { return -0.1690859889909308; }

void setup(void *arg) {}

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
