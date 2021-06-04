// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/nbody-clang-7.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// contributed by Shakhno DV, Shakhno AV

#define BENCHMARK_NAME "n-body"
#define BENCHMARK_RESULT_TYPE 4
#include "../common.h"

#include <math.h>

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

const char *JS_CREATE_ARRAY =
    "function createJsArray() { return [0.0, 0.0, 0.0, 0.0, 0.0]; }\n"
    "(createJsArray)\n";

void *(*createJsArray)();

double *createStorageArray() {
  void *jsArr = createJsArray();
  double *jsArrAsDoubleArr = polyglot_as_double_array(jsArr);
  return jsArrAsDoubleArr;
}

#define FOR_ALL_VARS(func)                                                     \
  func(x);                                                                     \
  func(y);                                                                     \
  func(z);                                                                     \
  func(vx);                                                                    \
  func(vy);                                                                    \
  func(vz);                                                                    \
  func(mass);

#define STORAGE_ARRAY_FIELD(arr_name) double *arr_name = NULL;

FOR_ALL_VARS(STORAGE_ARRAY_FIELD)

void setup(void *arg) { createJsArray = polyglot_eval("js", JS_CREATE_ARRAY); }

void init() {
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

#define SETUP_ARR(arr_name) arr_name = createStorageArray();

  FOR_ALL_VARS(SETUP_ARR)

#define INIT_BODY(body, _x, _y, _z, _vx, _vy, _vz, _mass)                      \
  x[body] = __truffletaint_add_double(_x);                                     \
  y[body] = __truffletaint_add_double(_y);                                     \
  z[body] = __truffletaint_add_double(_z);                                     \
  vx[body] = _vx * days_per_year;                                              \
  vy[body] = _vy * days_per_year;                                              \
  vz[body] = _vz * days_per_year;                                              \
  mass[body] = _mass * solar_mass;

  x[SUN] = y[SUN] = z[SUN] = __truffletaint_add_double(0.0);
  vx[SUN] = vy[SUN] = vz[SUN] = 0.0;
  mass[SUN] = solar_mass;
  INIT_BODY(JUPITER, 4.84143144246472090e+00, -1.16032004402742839e+00,
            -1.03622044471123109e-01, 1.66007664274403694e-03,
            7.69901118419740425e-03, -6.90460016972063023e-05,
            9.54791938424326609e-04);
  INIT_BODY(SATURN, 8.34336671824457987e+00, 4.12479856412430479e+00,
            -4.03523417114321381e-01, -2.76742510726862411e-03,
            4.99852801234917238e-03, 2.30417297573763929e-05,
            2.85885980666130812e-04);
  INIT_BODY(URANUS, 1.28943695621391310e+01, -1.51111514016986312e+01,
            -2.23307578892655734e-01, 2.96460137564761618e-03,
            2.37847173959480950e-03, -2.96589568540237556e-05,
            4.36624404335156298e-05);
  INIT_BODY(NEPTUNE, 1.53796971148509165e+01, -2.59193146099879641e+01,
            1.79258772950371181e-01, 2.68067772490389322e-03,
            1.62824170038242295e-03, -9.51592254519715870e-05,
            5.15138902046611451e-05);
}

void tearDown() {
  for (int i = 0; i < NBODIES; i++) {
    x[i] = y[i] = z[i] = vx[i] = vy[i] = vz[i] = mass[i] = 0.0;
  }

#define CLEAR_ARR(arr_name) arr_name = NULL;

  FOR_ALL_VARS(CLEAR_ARR)
}

void advance() {
  for (int i = 0; i < NBODIES; ++i) {
    double x1 = x[i];
    double y1 = y[i];
    double z1 = z[i];
    for (int j = i + 1; j < NBODIES; ++j) {
      double dx = x1 - x[j];
      double R = dx * dx;
      double dy = y1 - y[j];
      R += dy * dy;
      double dz = z1 - z[j];
      R += dz * dz;
      R = sqrt(R);
      double mag = DT / (R * R * R);
      vx[i] = vx[i] - dx * mass[j] * mag;
      vy[i] = vy[i] - dy * mass[j] * mag;
      vz[i] = vz[i] - dz * mass[j] * mag;
      vx[j] = vx[j] + dx * mass[i] * mag;
      vy[j] = vy[j] + dy * mass[i] * mag;
      vz[j] = vz[j] + dz * mass[i] * mag;
    }
  }

  for (int i = 0; i < NBODIES; ++i) {
    x[i] = x[i] + DT * vx[i];
    y[i] = y[i] + DT * vy[i];
    z[i] = z[i] + DT * vz[i];
  }
}

double energy() {
  double e = 0.0;
  for (int i = 0; i < NBODIES; ++i) {
    e += 0.5 * mass[i] * (vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i]);
    for (int j = i + 1; j < NBODIES; ++j) {
      double dx = x[i] - x[j];
      double dy = y[i] - y[j];
      double dz = z[i] - z[j];
      double distance = sqrt(dx * dx + dy * dy + dz * dz);
      e -= (mass[i] * mass[j]) / distance;
    }
  }
  return e;
}

void offset_momentum() {
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < NBODIES; ++i) {
    px += vx[i] * mass[i];
    py += vy[i] * mass[i];
    pz += vz[i] * mass[i];
  }
  vx[0] = -px / solar_mass;
  vy[0] = -py / solar_mass;
  vz[0] = -pz / solar_mass;
}

double benchmark() {
  init();
  offset_momentum();

  for (int k = 0; k < 2000000; k++)
    advance();

  double result = energy();
  result = __truffletaint_remove_double(result);

  for (int i = 0; i < NBODIES; i++) {
    __truffletaint_assert_double(x[i]);
    __truffletaint_assert_double(y[i]);
    __truffletaint_assert_double(z[i]);
  }

  tearDown();

  return result;
}

double getExpectedResult() { return -0.16902628585296136; }
