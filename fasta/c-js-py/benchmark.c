// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fasta-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * by Paul Hsieh
 */

#include <polyglot.h>
#include <taint.h>
#include <stdlib.h>
#include <stdio.h>

void writeFastaHeader(const char *id, const char *desc)
{
  // printf(">%s %s\n", id, desc);
}

void writeFasta(char *str)
{
  char ch;
  int i = 0;
  while ((ch = str[i++]) != '\0')
  {
    switch (ch)
    {
    case 'G':
    case 'T':
    case 'g':
    case 't':
      __truffletaint_assert_char(ch);
      break;

    case '\n':
      return;

    default:
      __truffletaint_assertnot_char(ch);
      break;
    }
  }

  // puts(str);
}

double gen_random(double max)
{
  static long last = 42;
  return max * (last = (last * 3877 + 29573) % 139968) / 139968;
}

struct aminoacids
{
  char c;
  double p;
};

__attribute__((always_inline)) static inline polyglot_typeid polyglot_aminoacids_typeid()
{
  static struct aminoacids __polyglot_typeid_aminoacids[0];
  return __polyglot_as_typeid(__polyglot_typeid_aminoacids);
}
__attribute__((always_inline)) static inline struct aminoacids *polyglot_as_aminoacids_array(void *p)
{
  void *ret = polyglot_as_typed(p, polyglot_array_typeid(polyglot_aminoacids_typeid(), 0));
  return (struct aminoacids *)ret;
}
__attribute__((always_inline)) static inline void *polyglot_from_aminoacids_array(struct aminoacids *arr, uint64_t len) { return polyglot_from_typed(arr, polyglot_array_typeid(polyglot_aminoacids_typeid(), len)); }
__attribute__((always_inline)) static inline struct aminoacids *polyglot_as_aminoacids(void *p)
{
  void *ret = polyglot_as_typed(p, polyglot_aminoacids_typeid());
  return (struct aminoacids *)ret;
}
__attribute__((always_inline)) static inline void *polyglot_from_aminoacids(struct aminoacids *s) { return polyglot_from_typed(s, polyglot_aminoacids_typeid()); }

struct aminoacids *(*createAminoacid)(char c, double p);

const char *JS_CODE_CREATE_AMINOACIDS = "function createAminoacid(_c, _p) {\n"
                                        "    return { c: _c, p: _p };"
                                        "}\n"
                                        "(createAminoacid)\n";

/* Weighted selection from alphabet */

void makeCumulative(struct aminoacids **genelist, int count)
{
  double cp = 0.0;
  int i;

  for (i = 0; i < count; i++)
  {
    cp += genelist[i]->p;
    genelist[i]->p = cp;
  }
}

char (*selectRandom)(const struct aminoacids *, int, double (*)(double),
                     double (*)(const struct aminoacids **, int),
                     char (*)(const struct aminoacids **, int));

double get_p(const struct aminoacids **genelist, int i)
{
  return genelist[0]->p;
}

char get_c(const struct aminoacids **genelist, int i) { return genelist[0]->c; }

const char *PY_SELECT_RANDOM =
    "from math import floor\n"
    "\n"
    "def selectRandom(genelist, count, gen_random, get_p, get_c):\n"
    "    r = gen_random(1)\n"
    "    \n"
    "    if r < get_p(genelist, 0):\n"
    "        return get_c(genelist, 0)\n"
    "    \n"
    "    lo = 0\n"
    "    hi = count - 1\n"
    "    while hi > (lo + 1):\n"
    "        i = (hi + lo) / 2\n"
    "        i = floor(i)\n"
    "        if r < get_p(genelist, i):\n"
    "            hi = i\n"
    "        else:\n"
    "            lo = i\n"
    "    return get_c(genelist, hi)\n"
    "\n"
    "selectRandom\n";

/* Generate and write FASTA format */

void makeRandomFasta(const char *id, const char *desc,
                     const struct aminoacids **genelist, int count, int n)
{
  int todo = n;
  int i, m;

  writeFastaHeader(id, desc);

  for (; todo > 0; todo -= (60))
  {
    char pick[(60) + 1];
    if (todo < (60))
      m = todo;
    else
      m = (60);
    for (i = 0; i < m; i++)
      pick[i] = selectRandom(genelist, count, &gen_random, &get_c, &get_p);
    pick[m] = '\0';
    writeFasta(pick);
  }
}

const char *PY_REPEAT_LOOP =
    "def makeRepeatFastaLoop(s, ss, n, kn, writeFasta, get_char, set_char):\n"
    "    LINE_LENGTH = 60\n"
    "    todo = n\n"
    "    k = 0\n"
    "    m = 0\n"
    "    while todo > 0:\n"
    "        if todo < LINE_LENGTH:\n"
    "            m = todo\n"
    "        else:\n"
    "            m = LINE_LENGTH\n"
    "        while m >= (kn - k):\n"
    "            writeFasta(s, k)\n"
    "            m -= (kn - k)\n"
    "            k = 0\n"
    "        set_char(ss, k + m, \'\\0\')\n"
    "        writeFasta(ss, k)\n"
    "        tmp = get_char(s, m + k)\n"
    "        set_char(ss, k + m, tmp)\n"
    "        k += m\n"
    "        todo -= LINE_LENGTH\n"
    "\n"
    "makeRepeatFastaLoop\n";

char get_char(char *str, int i) { return str[i]; }

void set_char(char *str, int i, char ch) { str[i] = ch; }

void writeFastaC(char *str, int offset) { writeFasta(str + offset); }

void (*makeRepeatFastaLoop)(const char *, char *, int, int,
                            void (*)(char *, int), char (*)(char *, int),
                            void (*)(char *, int, char));

void makeRepeatFasta(const char *id, const char *desc, const char *s, int n)
{
  char *ss;
  int todo = n, k = 0, kn = strlen(s);

  ss = (char *)malloc(kn + 1);
  memcpy(ss, s, kn + 1);

  writeFastaHeader(id, desc);

  makeRepeatFastaLoop(s, ss, n, kn, &writeFastaC, &get_char, &set_char);

  memset(ss, 0, kn + 1);
  free(ss);
}

/* Main -- define alphabets, make 3 fragments */

struct aminoacids **iub;

struct aminoacids **homosapiens;

char *alu;

void setupBaseData()
{
  homosapiens = calloc(4, sizeof(struct aminoacids *));
  homosapiens[0] = createAminoacid('a', 0.3029549426680);
  homosapiens[1] = createAminoacid('c', 0.1979883004921);
  homosapiens[2] =
      createAminoacid(__truffletaint_add_char('g'), 0.1975473066391);
  homosapiens[3] =
      createAminoacid(__truffletaint_add_char('t'), 0.3015094502008);

  for (int i = 0; i < 4; i++)
  {
    homosapiens[i] = polyglot_as_aminoacids(homosapiens[i]);
  }

  iub = calloc(15, sizeof(struct aminoacids *));
  iub[0] = createAminoacid('a', 0.27);
  iub[1] = createAminoacid('c', 0.12);
  iub[2] = createAminoacid(__truffletaint_add_char('g'), 0.12);
  iub[3] = createAminoacid(__truffletaint_add_char('t'), 0.27);
  iub[4] = createAminoacid('B', 0.02);
  iub[5] = createAminoacid('D', 0.02);
  iub[6] = createAminoacid('H', 0.02);
  iub[7] = createAminoacid('K', 0.02);
  iub[8] = createAminoacid('M', 0.02);
  iub[9] = createAminoacid('N', 0.02);
  iub[10] = createAminoacid('R', 0.02);
  iub[11] = createAminoacid('S', 0.02);
  iub[12] = createAminoacid('V', 0.02);
  iub[13] = createAminoacid('W', 0.02);
  iub[14] = createAminoacid('Y', 0.02);

  for (int i = 0; i < 15; i++)
  {
    iub[i] = polyglot_as_aminoacids(iub[i]);
  }

  const char *alu_init = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
                         "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
                         "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
                         "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
                         "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
                         "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
                         "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";
  alu = calloc(((42 * 6) + 35), sizeof(char));
  for (size_t i = 0; i < ((42 * 6) + 35); i++)
  {
    const char ch = alu_init[i];
    switch (ch)
    {
    case 'G':
    case 'T':
      alu[i] = __truffletaint_add_char(ch);
      break;

    default:
      alu[i] = ch;
      break;
    }
  }
}

void destroyBaseData()
{
  memset(homosapiens, 0, 4 * sizeof(struct aminoacids *));
  free(homosapiens);
  homosapiens = ((void *)0);

  memset(iub, 0, 15 * sizeof(struct aminoacids *));
  free(iub);
  iub = ((void *)0);

  memset(alu, 0, ((42 * 6) + 35) * sizeof(char));
  free(alu);
  alu = ((void *)0);
}

int benchmark()
{
  int n = 1000000;

  setupBaseData();

  makeCumulative(iub, 15);
  makeCumulative(homosapiens, 4);

  makeRepeatFasta("ONE", "Homo sapiens alu", alu, n * 2);
  makeRandomFasta("TWO", "IUB ambiguity codes", iub, 15, n * 3);
  makeRandomFasta("THREE", "Homo sapiens frequency", homosapiens,
                  4, n * 5);

  destroyBaseData();

  return 0;
}

void setup(void *arg)
{
  createAminoacid = polyglot_eval("js", JS_CODE_CREATE_AMINOACIDS);
  selectRandom = polyglot_eval("python", PY_SELECT_RANDOM);
  makeRepeatFastaLoop = polyglot_eval("python", PY_REPEAT_LOOP);
}

int getExpectedResult() { return 0; }

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
         "fasta"
         "\n");
  struct timeval start, end;
  for (unsigned int i = 0; i < iterations; i++)
  {
    gettimeofday(&start, ((void *)0));
    int result = benchmark();
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
