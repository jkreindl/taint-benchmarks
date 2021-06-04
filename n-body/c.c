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

double *x;
double *y;
double *z;
double *vx;
double *vy;
double *vz;
double *mass;

#define GETTER_AND_SETTER(arr_name)                                            \
                                                                               \
  double get_##arr_name(int nBody) { return arr_name[nBody]; }                 \
                                                                               \
  void set_##arr_name(int nBody, double value) { arr_name[nBody] = value; }

GETTER_AND_SETTER(x)
GETTER_AND_SETTER(y)
GETTER_AND_SETTER(z)
GETTER_AND_SETTER(vx)
GETTER_AND_SETTER(vy)
GETTER_AND_SETTER(vz)
GETTER_AND_SETTER(mass)

#define SUN 0
#define JUPITER 1
#define SATURN 2
#define URANUS 3
#define NEPTUNE 4

void init() {
  pi = 3.141592653589793;
  solar_mass = (4 * pi * pi);
  days_per_year = 365.24;

  x = calloc(NBODIES, sizeof(double));
  y = calloc(NBODIES, sizeof(double));
  z = calloc(NBODIES, sizeof(double));
  vx = calloc(NBODIES, sizeof(double));
  vy = calloc(NBODIES, sizeof(double));
  vz = calloc(NBODIES, sizeof(double));
  mass = calloc(NBODIES, sizeof(double));

  set_x(SUN, __truffletaint_add_double(0.0));
  set_y(SUN, __truffletaint_add_double(0.0));
  set_z(SUN, __truffletaint_add_double(0.0));
  set_vx(SUN, 0);
  set_vy(SUN, 0);
  set_vz(SUN, 0);
  set_mass(SUN, solar_mass);
  set_x(JUPITER, __truffletaint_add_double(4.84143144246472090e+00));
  set_y(JUPITER, __truffletaint_add_double(-1.16032004402742839e+00));
  set_z(JUPITER, __truffletaint_add_double(-1.03622044471123109e-01));
  set_vx(JUPITER, 1.66007664274403694e-03 * days_per_year);
  set_vy(JUPITER, 7.69901118419740425e-03 * days_per_year);
  set_vz(JUPITER, -6.90460016972063023e-05 * days_per_year);
  set_mass(JUPITER, 9.54791938424326609e-04 * solar_mass);
  set_x(SATURN, __truffletaint_add_double(8.34336671824457987e+00));
  set_y(SATURN, __truffletaint_add_double(4.12479856412430479e+00));
  set_z(SATURN, __truffletaint_add_double(-4.03523417114321381e-01));
  set_vx(SATURN, -2.76742510726862411e-03 * days_per_year);
  set_vy(SATURN, 4.99852801234917238e-03 * days_per_year);
  set_vz(SATURN, 2.30417297573763929e-05 * days_per_year);
  set_mass(SATURN, 2.85885980666130812e-04 * solar_mass);
  set_x(URANUS, __truffletaint_add_double(1.28943695621391310e+01));
  set_y(URANUS, __truffletaint_add_double(-1.51111514016986312e+01));
  set_z(URANUS, __truffletaint_add_double(-2.23307578892655734e-01));
  set_vx(URANUS, 2.96460137564761618e-03 * days_per_year);
  set_vy(URANUS, 2.37847173959480950e-03 * days_per_year);
  set_vz(URANUS, -2.96589568540237556e-05 * days_per_year);
  set_mass(URANUS, 4.36624404335156298e-05 * solar_mass);
  set_x(NEPTUNE, __truffletaint_add_double(1.53796971148509165e+01));
  set_y(NEPTUNE, __truffletaint_add_double(-2.59193146099879641e+01));
  set_z(NEPTUNE, __truffletaint_add_double(1.79258772950371181e-01));
  set_vx(NEPTUNE, 2.68067772490389322e-03 * days_per_year);
  set_vy(NEPTUNE, 1.62824170038242295e-03 * days_per_year);
  set_vz(NEPTUNE, -9.51592254519715870e-05 * days_per_year);
  set_mass(NEPTUNE, 5.15138902046611451e-05 * solar_mass);
}

void tearDown() {
  for (int i = 0; i < NBODIES; i++) {
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

void advance() {
  for (int i = 0; i < NBODIES; ++i) {
    double x1 = get_x(i);
    double y1 = get_y(i);
    double z1 = get_z(i);
    for (int j = i + 1; j < NBODIES; ++j) {
      double dx = x1 - get_x(j);
      double R = dx * dx;
      double dy = y1 - get_y(j);
      R += dy * dy;
      double dz = z1 - get_z(j);
      R += dz * dz;
      R = sqrt(R);
      double mag = DT / (R * R * R);
      set_vx(i, get_vx(i) - dx * get_mass(j) * mag);
      set_vy(i, get_vy(i) - dy * get_mass(j) * mag);
      set_vz(i, get_vz(i) - dz * get_mass(j) * mag);
      set_vx(j, get_vx(j) + dx * get_mass(i) * mag);
      set_vy(j, get_vy(j) + dy * get_mass(i) * mag);
      set_vz(j, get_vz(j) + dz * get_mass(i) * mag);
    }
  }

  for (int i = 0; i < NBODIES; ++i) {
    set_x(i, get_x(i) + DT * get_vx(i));
    set_y(i, get_y(i) + DT * get_vy(i));
    set_z(i, get_z(i) + DT * get_vz(i));
  }
}

double energy() {
  double e = 0.0;
  for (int i = 0; i < NBODIES; ++i) {
    e +=
        0.5 * get_mass(i) *
        (get_vx(i) * get_vx(i) + get_vy(i) * get_vy(i) + get_vz(i) * get_vz(i));
    for (int j = i + 1; j < NBODIES; ++j) {
      double dx = get_x(i) - get_x(j);
      double dy = get_y(i) - get_y(j);
      double dz = get_z(i) - get_z(j);
      double distance = sqrt(dx * dx + dy * dy + dz * dz);
      e -= (get_mass(i) * get_mass(j)) / distance;
    }
  }
  return e;
}

void offset_momentum() {
  double px = 0.0, py = 0.0, pz = 0.0;
  for (int i = 0; i < NBODIES; ++i) {
    px += get_vx(i) * get_mass(i);
    py += get_vy(i) * get_mass(i);
    pz += get_vz(i) * get_mass(i);
  }
  set_vx(0, -px / solar_mass);
  set_vy(0, -py / solar_mass);
  set_vz(0, -pz / solar_mass);
}

double benchmark() {
  init();
  offset_momentum();

  for (int k = 0; k < 2000000; k++)
    advance();

  double result = energy();
  result = __truffletaint_remove_double(result);

  for (int i = 0; i < NBODIES; i++) {
    __truffletaint_assert_double(get_x(i));
    __truffletaint_assert_double(get_y(i));
    __truffletaint_assert_double(get_z(i));
  }

  tearDown();

  return result;
}

double getExpectedResult() { return -0.16902628585296136; }

void setup(void *arg) {}
