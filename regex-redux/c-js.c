// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/regexredux-gcc-2.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 **
 ** regex-dna program contributed by Mike Pall
 ** converted from regex-dna program by Jeremy Zerfas
 **
 */

#define BENCHMARK_NAME "regex-redux"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

void *(*cleanData)(void *);
const char *JS_CLEAN_DATA = "(function cleanData(data) {\n"
                            "    return data.replace(/^>.*\\n|\\n/mg, \"\");\n"
                            "})\n";

int (*matchData)(void *, int);
const char *JS_MATCH_DATA = "(function matchData(data, regexIndex) {\n"
                            "    const regExps = [\n"
                            "        /agggtaaa|tttaccct/ig,\n"
                            "        /[cgt]gggtaaa|tttaccc[acg]/ig,\n"
                            "        /a[act]ggtaaa|tttacc[agt]t/ig,\n"
                            "        /ag[act]gtaaa|tttac[agt]ct/ig,\n"
                            "        /agg[act]taaa|ttta[agt]cct/ig,\n"
                            "        /aggg[acg]aaa|ttt[cgt]ccct/ig,\n"
                            "        /agggt[cgt]aa|tt[acg]accct/ig,\n"
                            "        /agggta[cgt]a|t[acg]taccct/ig,\n"
                            "        /agggtaa[cgt]|[acg]ttaccct/ig\n"
                            "    ];\n"
                            "    const m = data.match(regExps[regexIndex]);\n"
                            "    if (m) {\n"
                            "        return m.length;\n"
                            "    } else {\n"
                            "        return 0;\n"
                            "    }"
                            "})\n";

void *(*replaceData)(void *);
const char *JS_REPLACE_DATA = "(function replaceData(data) {\n"
                              "    return data\n"
                              "        .replace(/tHa[Nt]/g, '<4>')\n"
                              "        .replace(/aND|caN|Ha[DS]|WaS/g, '<3>')\n"
                              "        .replace(/a[NSt]|BY/g, '<2>')\n"
                              "        .replace(/<[^>]*>/g, '|')\n"
                              "        .replace(/\\|[^|][^|]*\\|/g, '-');\n"
                              "})\n";

void *ioObj;

#define __USE_STRING_INLINES
#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct fbuf
{
  char *buf;
  size_t size, len;
} fbuf_t;

static void fb_init(fbuf_t *b)
{
  b->buf = NULL;
  b->len = b->size = 0;
}

static char *fb_need(fbuf_t *b, size_t need)
{
  need += b->len;
  if (need > b->size)
  {
    if (b->size == 0)
      b->size = need;
    else
      while (need > b->size)
        b->size += b->size;
    if (!(b->buf = realloc(b->buf, b->size)))
    {
      printf("unable to realloc\n");
      exit(1);
    }
  }
  return b->buf + b->len;
}

#define FB_MINREAD (3 << 16)

/* Read all into dst buffer. */
static size_t fb_readall(fbuf_t *dst)
{
  BENCHMARK_IO_OPEN_INPUT_FILE(ioObj);
  while (BENCHMARK_IO_CAN_READ_LINE(ioObj))
  {
    char *dp = fb_need(dst, FB_MINREAD);
    int n;
    void *tmp;
    BENCHMARK_IO_READ_LINE_CONST_BUFFER(ioObj, tmp, dp, dst->size - dst->len,
                                        n);
    dst->len += n;

    dp = fb_need(dst, FB_MINREAD);
    dp[0] = '\n';
    dst->len++;
  }
  BENCHMARK_IO_CLOSE_INPUT_FILE(ioObj);

  const int len = dst->len;
  char *buf = dst->buf;
  for (int i = 0; i < len; i++)
  {
    buf[i] = __truffletaint_add_char(buf[i]);
  }

  return len;
}

static const char *variants[] = {
    "agggtaaa|tttaccct", "[cgt]gggtaaa|tttaccc[acg]",
    "a[act]ggtaaa|tttacc[agt]t", "ag[act]gtaaa|tttac[agt]ct",
    "agg[act]taaa|ttta[agt]cct", "aggg[acg]aaa|ttt[cgt]ccct",
    "agggt[cgt]aa|tt[acg]accct", "agggta[cgt]a|t[acg]taccct",
    "agggtaa[cgt]|[acg]ttaccct", NULL};

int benchmark()
{
  fbuf_t seq;
  const char **pp;
  size_t ilen, clen, slen;
  int flip;
  fb_init(&seq);
  ilen = fb_readall(&seq);

  void *initialStr = polyglot_from_string_n(seq.buf, seq.len, "UTF-8");
  void *cleanedStr = cleanData(initialStr);
  clen = polyglot_get_string_size(cleanedStr);

  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  for (int i = 0; i < 9; i++)
  {
    BENCHMARK_IO_WRITE(ioObj, variants[i], strlen(variants[i]));
    BENCHMARK_IO_WRITE_CHAR(ioObj, ' ');
    size_t nMatches = matchData(cleanedStr, i);
    __truffletaint_assert_unsigned_long(nMatches);
    BENCHMARK_IO_WRITE_NUMBER(ioObj,
                              __truffletaint_remove_unsigned_long(nMatches));
    BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  }

  void *replacedStr = replaceData(cleanedStr);
  slen = polyglot_get_string_size(replacedStr);

  __truffletaint_assertnot_unsigned_long(ilen);
  __truffletaint_assert_unsigned_long(clen);
  __truffletaint_assert_unsigned_long(slen);

  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, ilen);
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, __truffletaint_remove_unsigned_long(clen));
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, __truffletaint_remove_unsigned_long(slen));
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');

  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);

  return 0;
}

int getExpectedResult() { return 0; }

void setup(void *arg)
{
  ioObj = arg;
  cleanData = polyglot_eval("js", JS_CLEAN_DATA);
  matchData = polyglot_eval("js", JS_MATCH_DATA);
  replaceData = polyglot_eval("js", JS_REPLACE_DATA);
}
