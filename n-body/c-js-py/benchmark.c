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
const char *PY_CODE =
    "class Foo:\n\n"
    "    def get_x(self, nBody):\n"
    "        return self.x[nBody]\n\n"
    "    def set_x(self, nBody, value):\n"
    "        self.x[nBody] = value\n\n"
    "    def init_x(self, arr):\n"
    "        self.x = arr\n\n"
    ""
    "    def get_y(self, nBody):\n"
    "        return self.y[nBody]\n\n"
    "    def set_y(self, nBody, value):\n"
    "        self.y[nBody] = value\n\n"
    "    def init_y(self, arr):\n"
    "        self.y = arr\n\n"
    ""
    "    def get_z(self, nBody):\n"
    "        return self.z[nBody]\n\n"
    "    def set_z(self, nBody, value):\n"
    "        self.z[nBody] = value\n\n"
    "    def init_z(self, arr):\n"
    "        self.z = arr\n\n"
    ""
    "    def get_vx(self, nBody):\n"
    "        return self.vx[nBody]\n\n"
    "    def set_vx(self, nBody, value):\n"
    "        self.vx[nBody] = value\n\n"
    "    def init_vx(self, arr):\n"
    "        self.vx = arr\n\n"
    ""
    "    def get_vy(self, nBody):\n"
    "        return self.vy[nBody]\n\n"
    "    def set_vy(self, nBody, value):\n"
    "        self.vy[nBody] = value\n\n"
    "    def init_vy(self, arr):\n"
    "        self.vy = arr\n\n"
    ""
    "    def get_vz(self, nBody):\n"
    "        return self.vz[nBody]\n\n"
    "    def set_vz(self, nBody, value):\n"
    "        self.vz[nBody] = value\n\n"
    "    def init_vz(self, arr):\n"
    "        self.vz = arr\n\n"
    ""
    "    def get_mass(self, nBody):\n"
    "        return self.mass[nBody]\n\n"
    "    def set_mass(self, nBody, value):\n"
    "        self.mass[nBody] = value\n\n"
    "    def init_mass(self, arr):\n"
    "        self."
    "mass"
    " = arr\n\n"
    "\n"
    "\n\n"
    "Foo()\n";

const char *JS_CREATE_ARRAY =
    "function createArray() { return [0.0, 0.0, 0.0, 0.0, 0.0]; }\n"
    "(createArray)\n";

void *(*createArray)();

double (*get_x)(int);
void (*set_x)(int, double);
void (*init_x)(void *);
double (*get_y)(int);
void (*set_y)(int, double);
void (*init_y)(void *);
double (*get_z)(int);
void (*set_z)(int, double);
void (*init_z)(void *);
double (*get_vx)(int);
void (*set_vx)(int, double);
void (*init_vx)(void *);
double (*get_vy)(int);
void (*set_vy)(int, double);
void (*init_vy)(void *);
double (*get_vz)(int);
void (*set_vz)(int, double);
void (*init_vz)(void *);
double (*get_mass)(int);
void (*set_mass)(int, double);
void (*init_mass)(void *);

void setup(void *arg)
{
  void *module = polyglot_eval("python", PY_CODE);

  get_x = polyglot_get_member(module, "get_x");
  set_x = polyglot_get_member(module, "set_x");
  init_x = polyglot_get_member(module, "init_x");
  get_y = polyglot_get_member(module, "get_y");
  set_y = polyglot_get_member(module, "set_y");
  init_y = polyglot_get_member(module, "init_y");
  get_z = polyglot_get_member(module, "get_z");
  set_z = polyglot_get_member(module, "set_z");
  init_z = polyglot_get_member(module, "init_z");
  get_vx = polyglot_get_member(module, "get_vx");
  set_vx = polyglot_get_member(module, "set_vx");
  init_vx = polyglot_get_member(module, "init_vx");
  get_vy = polyglot_get_member(module, "get_vy");
  set_vy = polyglot_get_member(module, "set_vy");
  init_vy = polyglot_get_member(module, "init_vy");
  get_vz = polyglot_get_member(module, "get_vz");
  set_vz = polyglot_get_member(module, "set_vz");
  init_vz = polyglot_get_member(module, "init_vz");
  get_mass = polyglot_get_member(module, "get_mass");
  set_mass = polyglot_get_member(module, "set_mass");
  init_mass = polyglot_get_member(module, "init_mass");

  createArray = polyglot_eval("js", JS_CREATE_ARRAY);
}

void init()
{
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  init_x(createArray());
  init_y(createArray());
  init_z(createArray());
  init_vx(createArray());
  init_vy(createArray());
  init_vz(createArray());
  init_mass(createArray());

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
    set_x(i, 0.0);
    set_y(i, 0.0);
    set_z(i, 0.0);
    set_vx(i, 0.0);
    set_vy(i, 0.0);
    set_vz(i, 0.0);
    set_mass(i, 0.0);
  }
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
    __truffletaint_assert_double(get_vx(i));
    __truffletaint_assert_double(get_vy(i));
    __truffletaint_assert_double(get_vz(i));
    __truffletaint_assertnot_double(get_mass(i));
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