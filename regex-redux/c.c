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
#define NATIVE_INPUT_FILE "regexredux-input-50000.fasta"
#include "../common.h"

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

/* Substitute pattern p with replacement r, copying from src to dst buffer. */
static size_t fb_subst(fbuf_t *dst, fbuf_t *src, const char *p, const char *r)
{
  pcre *re;
  pcre_extra *re_ex;
  const char *re_e;
  char *dp;
  int re_eo, m[3], pos, rlen, clen;
  if (!(re = pcre_compile(p, 0, &re_e, &re_eo, NULL)))
  {
    printf("unable to compile regex for substition\n");
    exit(1);
  }
  re_ex = pcre_study(re, 0, &re_e);
  for (dst->len = 0, rlen = strlen(r), pos = 0;
       pcre_exec(re, re_ex, src->buf, src->len, pos, 0, m, 3) >= 0;
       pos = m[1])
  {
    clen = m[0] - pos;
    dp = fb_need(dst, clen + rlen);
    dst->len += clen + rlen;
    memcpy(dp, src->buf + pos, clen);
    memcpy(dp + clen, r, rlen);
  }
  clen = src->len - pos;
  dp = fb_need(dst, clen);
  dst->len += clen;
  memcpy(dp, src->buf + pos, clen);
  return dst->len;
}

/* Count all matches with pattern p in src buffer. */
static int fb_countmatches(fbuf_t *src, const char *p)
{
  pcre *re;
  pcre_extra *re_ex;
  const char *re_e;
  int re_eo, m[3], pos, count;
  if (!(re = pcre_compile(p, 0, &re_e, &re_eo, NULL)))
  {
    printf("unable to compile regex for counting\n");
    exit(1);
  }
  re_ex = pcre_study(re, 0, &re_e);
  for (count = 0, pos = 0;
       pcre_exec(re, re_ex, src->buf, src->len, pos, 0, m, 3) >= 0; pos = m[1])
    count++;
  return count;
}

static const char *variants[] = {
    "agggtaaa|tttaccct", "[cgt]gggtaaa|tttaccc[acg]",
    "a[act]ggtaaa|tttacc[agt]t", "ag[act]gtaaa|tttac[agt]ct",
    "agg[act]taaa|ttta[agt]cct", "aggg[acg]aaa|ttt[cgt]ccct",
    "agggt[cgt]aa|tt[acg]accct", "agggta[cgt]a|t[acg]taccct",
    "agggtaa[cgt]|[acg]ttaccct", NULL};

static const char *subst[] = {
    "tHa[Nt]", "<4>", "aND|caN|Ha[DS]|WaS", "<3>", "a[NSt]|BY", "<2>",
    "<[^>]*>", "|", "\\|[^|][^|]*\\|", "-", NULL};

void assertSequenceTaint(fbuf_t *seq)
{
  char *data = seq->buf;
  const size_t len = seq->len;

  int i;
  while (i < len)
  {

    char ch = data[i++];
    switch (ch)
    {
    case '|':
    case '-':
    {
      __truffletaint_assertnot_char(ch);
      break;
    }

    case '<':
    {
      // should we get an out-of-bounds access here the program did not produce
      // the correct result anyways
      __truffletaint_assertnot_char(ch);
      __truffletaint_assertnot_char(data[i++]);
      __truffletaint_assertnot_char(data[i++]);
      break;
    }

    default:
      __truffletaint_assert_char(ch);
    }
  }
}

int benchmark()
{
  fbuf_t seq[2];
  const char **pp;
  size_t ilen, clen, slen;
  int flip;
  fb_init(&seq[0]);
  fb_init(&seq[1]);
  ilen = fb_readall(&seq[0]);

  clen = fb_subst(&seq[1], &seq[0], ">.*|\n", "");

  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  for (pp = variants; *pp; pp++)
  {
    BENCHMARK_IO_WRITE(ioObj, *pp, strlen(*pp));
    BENCHMARK_IO_WRITE_CHAR(ioObj, ' ');
    int nMatches = fb_countmatches(&seq[1], *pp);
    __truffletaint_assertnot_int(nMatches);
    BENCHMARK_IO_WRITE_NUMBER(ioObj, nMatches);
    BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  }

  for (slen = 0, flip = 1, pp = subst; *pp; pp += 2, flip = 1 - flip)
  {
    slen = fb_subst(&seq[1 - flip], &seq[flip], *pp, pp[1]);
  }

  __truffletaint_assertnot_unsigned_long(ilen);
  __truffletaint_assertnot_unsigned_long(clen);
  __truffletaint_assertnot_unsigned_long(slen);
  assertSequenceTaint(&seq[1 - flip]);

  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, ilen);
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, clen);
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  BENCHMARK_IO_WRITE_NUMBER(ioObj, slen);
  BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');

  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);

  return 0;
}

int getExpectedResult() { return 0; }

void setup(void *arg) { ioObj = arg; }
