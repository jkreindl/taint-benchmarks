// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-node-1.html
// and
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-gcc-1.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// Contributed by Jeremy Zerfas

#define BENCHMARK_NAME "k-nucleotide"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

const char *JS_LOGIC =
    "const Taint = Polyglot.import(\"taint\");\n"
    "\n"
    "function RefNum (num) {\n"
    "    num = Taint.add(num);\n"
    "    this.num = num;\n"
    "}\n"
    "\n"
    "function frequency (seq, length) {\n"
    "    let freq = new Map();\n"
    "    let n = seq.length - length + 1;\n"
    "    for (let i = 0; i < n; ++i) {\n"
    "        let key = seq.substr(i, length);\n"
    "        key = Taint.remove(key);\n"
    "        const cur = freq.get(key);\n"
    "        cur === undefined ? freq.set(key, new RefNum(1)) : ++cur.num;\n"
    "    }\n"
    "    return freq;\n"
    "}\n"
    "\n"
    "function sort (seq, length) {\n"
    "    Taint.assertTainted(seq);\n"
    "    const f = frequency(seq, length);\n"
    "    const keys = Array.from(f.keys());\n"
    "    const n = seq.length - length + 1;\n"
    "    let res = \"\";\n"
    "    keys.sort((a, b) => Taint.remove(f.get(b).num - f.get(a).num));\n"
    "    for (let key of keys) {\n"
    "        key = Taint.remove(key);\n"
    "        const count = f.get(key).num * 100 * 1000;\n"
    "        const fraction = Math.floor(count / n);\n"
    "        Taint.assertTainted(fraction);\n"
    "        res += key + ' ' + (Math.floor(fraction / 1000)) + \".\" + (fraction % 1000).toString().padStart(3, \"0\") + '\\n';\n"
    "    }\n"
    "    res += '\\n';\n"
    "    return Taint.remove(res);\n"
    "}\n"
    "\n"
    "function find (seq, s) {\n"
    "    Taint.assertTainted(seq);\n"
    "    var f = frequency(seq, s.length);\n"
    "    return (Taint.remove(f.get(s).num) || 0) + \"\\t\" + Taint.remove(s) + '\\n';\n"
    "}\n"
    "\n"
    "({\n"
    "    sort: sort,\n"
    "    find: find\n"
    "})\n";

void *ioObj;

// This controls the maximum length for each set of oligonucleotide frequencies
// and each oligonucleotide count output by this program.
#define MAXIMUM_OUTPUT_LENGTH 4096

// intptr_t should be the native integer type on most sane systems.
typedef intptr_t intnative_t;

void *(*sort)(void *, const intnative_t);

// Generate frequencies for all oligonucleotides in polynucleotide that are of
// desired_Length_For_Oligonucleotides and then save it to output.
static void *generate_Frequencies_For_Desired_Length_Oligonucleotides(
    void *polynucleotide,
    const intnative_t desired_Length_For_Oligonucleotides)
{
  void *res = sort(polynucleotide, desired_Length_For_Oligonucleotides);
  polyglot_invoke(ioObj, "write", res);
}

void *(*find)(void *, void *);

// Generate a count for the number of times oligonucleotide appears in
// polynucleotide and then save it to output.
static void
generate_Count_For_Oligonucleotide(void *polynucleotide,
                                   const intnative_t polynucleotide_Length,
                                   const char *const oligonucleotide)
{
  void *oligonucleotide_str = polyglot_from_string(oligonucleotide, "UTF-8");
  void *res = find(polynucleotide, oligonucleotide_str);
  polyglot_invoke(ioObj, "write", res);
}

#define INPUT_BUFFER_SIZE 4096

static inline int skipFirstTwoPolynucleotides(char *buffer)
{
  uint64_t bytesRead;
  do
  {
    void *tmp;
    BENCHMARK_IO_READ_LINE_CONST_BUFFER(ioObj, tmp, buffer, INPUT_BUFFER_SIZE,
                                        bytesRead);
  } while (bytesRead && memcmp(">THREE", buffer, sizeof(">THREE") - 1));
}

static inline int readNextLineOfPolynucleotideIntoBuffer(char *buffer)
{
  uint64_t bytesRead;
  void *tmp;
  BENCHMARK_IO_READ_LINE_CONST_BUFFER(ioObj, tmp, buffer, INPUT_BUFFER_SIZE,
                                      bytesRead);
  return bytesRead && buffer[0] != '>';
}

static inline char toUpperCase(char ch)
{
  if ('a' <= ch && ch <= 'z')
    ch = ch - 'a' + 'A';
  return ch;
}

void knucleotide()
{
  BENCHMARK_IO_OPEN_INPUT_FILE(ioObj);

  char buffer[INPUT_BUFFER_SIZE];

  // Find the start of the third polynucleotide.
  skipFirstTwoPolynucleotides(buffer);

  // Start with 1 MB of storage for reading in the polynucleotide and grow
  // geometrically.
  intnative_t polynucleotide_Capacity = 1048576;
  intnative_t polynucleotide_Length = 0;
  char *polynucleotide = malloc(polynucleotide_Capacity);

  // Start reading and encoding the third polynucleotide.
  while (readNextLineOfPolynucleotideIntoBuffer(buffer))
  {
    for (intnative_t i = 0; buffer[i] != '\0'; i++)
      if (buffer[i] != '\n')
        polynucleotide[polynucleotide_Length++] =
            toUpperCase(__truffletaint_add_char(buffer[i]));

    // Make sure we still have enough memory allocated for any potential
    // nucleotides in the next line.
    if (polynucleotide_Capacity - polynucleotide_Length < sizeof(buffer))
      polynucleotide = realloc(polynucleotide, polynucleotide_Capacity *= 2);
  }

  BENCHMARK_IO_CLOSE_INPUT_FILE(ioObj);

  // Free up any leftover memory.
  polynucleotide = realloc(polynucleotide, polynucleotide_Length);

  void *sequence =
      polyglot_from_string_n(polynucleotide, polynucleotide_Length, "UTF-8");

  free(polynucleotide);

  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  generate_Frequencies_For_Desired_Length_Oligonucleotides(sequence, 1);
  generate_Frequencies_For_Desired_Length_Oligonucleotides(sequence, 2);
  generate_Count_For_Oligonucleotide(sequence, polynucleotide_Length, "GGT");
  generate_Count_For_Oligonucleotide(sequence, polynucleotide_Length, "GGTA");
  generate_Count_For_Oligonucleotide(sequence, polynucleotide_Length, "GGTATT");
  generate_Count_For_Oligonucleotide(sequence, polynucleotide_Length,
                                     "GGTATTTTAATT");
  generate_Count_For_Oligonucleotide(sequence, polynucleotide_Length,
                                     "GGTATTTTAATTTATAGT");

  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);
}

int benchmark()
{
  knucleotide();
  return 0;
}

int getExpectedResult() { return 0; }

void setup(void *arg)
{
  ioObj = arg;
  void *jsModule = polyglot_eval("js", JS_LOGIC);
  find = polyglot_get_member(jsModule, "find");
  sort = polyglot_get_member(jsModule, "sort");
}
