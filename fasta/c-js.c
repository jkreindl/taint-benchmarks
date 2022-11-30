// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/fasta-clang-1.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * by Paul Hsieh
 */

#define BENCHMARK_NAME "fasta"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

#include <string.h>

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

#define IM 139968
#define IA 3877
#define IC 29573

double gen_random(double max)
{
  static long last = 42;
  return max * (last = (last * IA + IC) % IM) / IM;
}

struct aminoacids
{
  char c;
  double p;
};

/* Weighted selection from alphabet */

void makeCumulative(struct aminoacids *genelist, int count)
{
  double cp = 0.0;
  int i;

  for (i = 0; i < count; i++)
  {
    cp += genelist[i].p;
    genelist[i].p = cp;
  }
}

char (*selectRandom)(const struct aminoacids *, int, double (*)(double),
                     double (*)(const struct aminoacids *, int),
                     char (*)(const struct aminoacids *, int));

double get_p(const struct aminoacids *genelist, int i) { return genelist[0].p; }

char get_c(const struct aminoacids *genelist, int i) { return genelist[0].c; }

const char *JS_SELECT_RANDOM =
    "("
    "function selectRandom(genelist, count, gen_random, get_p, get_c) {\n"
    "    var r = gen_random(1);\n"
    "    var i, lo, hi;\n"
    "\n"
    "    if (r < get_p(genelist, 0))\n"
    "        return get_c(genelist, 0);\n"
    "\n"
    "    lo = 0;\n"
    "    hi = count - 1;\n"
    "\n"
    "    while (hi > (lo + 1)) {\n"
    "        i = (hi + lo) / 2;\n"
    "        i = Math.floor(i);\n"
    "        if (r < get_p(genelist, i))\n"
    "            hi = i;\n"
    "        else\n"
    "            lo = i;\n"
    "    }\n"
    "    return get_c(genelist, hi);\n"
    "}\n"
    ")";

/* Generate and write FASTA format */

#define LINE_LENGTH (60)

void makeRandomFasta(const char *id, const char *desc,
                     const struct aminoacids *genelist, int count, int n)
{
  int todo = n;
  int i, m;

  writeFastaHeader(id, desc);

  for (; todo > 0; todo -= LINE_LENGTH)
  {
    char pick[LINE_LENGTH + 1];
    if (todo < LINE_LENGTH)
      m = todo;
    else
      m = LINE_LENGTH;
    for (i = 0; i < m; i++)
      pick[i] = selectRandom(genelist, count, &gen_random, &get_p, &get_c);
    pick[m] = '\0';
    writeFasta(pick);
  }
}

const char *JS_REPEAT_LOOP = "("
                             "function makeRepeatFastaLoop(s, ss, n, kn, "
                             "writeFasta, get_char, set_char) {\n"
                             "    const LINE_LENGTH = 60;\n"
                             "    var todo = n;\n"
                             "    var m;\n"
                             "    var k = 0;\n"
                             "    for (; todo > 0; todo -= LINE_LENGTH) {\n"
                             "        if (todo < LINE_LENGTH)\n"
                             "            m = todo;\n"
                             "        else\n"
                             "            m = LINE_LENGTH;\n"
                             "\n"
                             "        while (m >= kn - k) {\n"
                             "            writeFasta(s, k);\n"
                             "            m -= kn - k;\n"
                             "            k = 0;\n"
                             "        }\n"
                             "\n"
                             "        set_char(ss, k + m, \'\\0\');\n"
                             "        writeFasta(ss, k);\n"
                             "        var tmp = get_char(s, m + k);\n"
                             "        set_char(ss, k + m, tmp);\n"
                             "        k += m;\n"
                             "    }\n"
                             "}\n"
                             ")";

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

struct aminoacids *iub;
#define IUB_LEN 15

struct aminoacids *homosapiens;
#define HOMOSAPIENS_LEN 4

char *alu;
#define ALU_LENGTH ((42 * 6) + 35)

void setupBaseData()
{
  homosapiens = calloc(HOMOSAPIENS_LEN, sizeof(struct aminoacids));
  homosapiens[0].c = 'a';
  homosapiens[0].p = 0.3029549426680;
  homosapiens[1].c = 'c';
  homosapiens[1].p = 0.1979883004921;
  homosapiens[2].c = __truffletaint_add_char('g');
  homosapiens[2].p = 0.1975473066391;
  homosapiens[3].c = __truffletaint_add_char('t');
  homosapiens[3].p = 0.3015094502008;

  iub = calloc(IUB_LEN, sizeof(struct aminoacids));
  iub[0].c = 'a';
  iub[0].p = 0.27;
  iub[1].c = 'c';
  iub[1].p = 0.12;
  iub[2].c = __truffletaint_add_char('g');
  iub[2].p = 0.12;
  iub[3].c = __truffletaint_add_char('t');
  iub[3].p = 0.27;
  iub[4].c = 'B';
  iub[4].p = 0.02;
  iub[5].c = 'D';
  iub[5].p = 0.02;
  iub[6].c = 'H';
  iub[6].p = 0.02;
  iub[7].c = 'K';
  iub[7].p = 0.02;
  iub[8].c = 'M';
  iub[8].p = 0.02;
  iub[9].c = 'N';
  iub[9].p = 0.02;
  iub[10].c = 'R';
  iub[10].p = 0.02;
  iub[11].c = 'S';
  iub[11].p = 0.02;
  iub[12].c = 'V';
  iub[12].p = 0.02;
  iub[13].c = 'W';
  iub[13].p = 0.02;
  iub[14].c = 'Y';
  iub[14].p = 0.02;

  const char *alu_init = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
                         "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
                         "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
                         "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
                         "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
                         "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
                         "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";
  alu = calloc(ALU_LENGTH, sizeof(char));
  for (size_t i = 0; i < ALU_LENGTH; i++)
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
  memset(homosapiens, 0, HOMOSAPIENS_LEN * sizeof(struct aminoacids));
  free(homosapiens);
  homosapiens = NULL;

  memset(iub, 0, IUB_LEN * sizeof(struct aminoacids));
  free(iub);
  iub = NULL;

  memset(alu, 0, ALU_LENGTH * sizeof(char));
  free(alu);
  alu = NULL;
}

int benchmark()
{
  int n = 1000000;

  setupBaseData();

  makeCumulative(iub, IUB_LEN);
  makeCumulative(homosapiens, HOMOSAPIENS_LEN);

  makeRepeatFasta("ONE", "Homo sapiens alu", alu, n * 2);
  makeRandomFasta("TWO", "IUB ambiguity codes", iub, IUB_LEN, n * 3);
  makeRandomFasta("THREE", "Homo sapiens frequency", homosapiens,
                  HOMOSAPIENS_LEN, n * 5);

  destroyBaseData();

  return 0;
}

void setup(void *arg)
{
  selectRandom = polyglot_eval("js", JS_SELECT_RANDOM);
  makeRepeatFastaLoop = polyglot_eval("js", JS_REPEAT_LOOP);
}

int getExpectedResult() { return 0; }
