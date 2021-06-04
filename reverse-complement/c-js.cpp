// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/revcomp-gpp-3.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
// Contributed by Paul Kitchin

#define BENCHMARK_NAME "reverse-complement"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"
#include <cstring>

extern "C" {

const char *JS_CALL_COMPLEMENT =
    "function callComplement(complement, character) {\n"
    "    return complement(character);\n"
    "}\n"
    "(callComplement)\n";

#define CHUNK_SIZE 65526
#define MAX_LINE_LENGTH 60

void *ioObj = nullptr;
char (*callComplement)(char (*)(char), char) = nullptr;

char complement(char character) {
#define SET_CCH(ch, isTainted)                                                 \
  {                                                                            \
    complementChar = ch;                                                       \
    throwChar = isTainted;                                                     \
    break;                                                                     \
  }
#define RETURN_COMPLEMENT_CHAR(ch) SET_CCH(ch, false)
#define THROW_COMPLEMENT_CHAR(ch) SET_CCH(__truffletaint_add_char(ch), true)
  char complementChar;
  bool throwChar;
  switch (character & 0x1f) {
  case 1:
    THROW_COMPLEMENT_CHAR('T');
  case 2:
    RETURN_COMPLEMENT_CHAR('V');
  case 3:
    THROW_COMPLEMENT_CHAR('G');
  case 4:
    RETURN_COMPLEMENT_CHAR('H');
  case 7:
    THROW_COMPLEMENT_CHAR('C');
  case 8:
    RETURN_COMPLEMENT_CHAR('D');
  case 11:
    THROW_COMPLEMENT_CHAR('M');
  case 13:
    RETURN_COMPLEMENT_CHAR('K');
  case 14:
    THROW_COMPLEMENT_CHAR('N');
  case 18:
    RETURN_COMPLEMENT_CHAR('Y');
  case 19:
    THROW_COMPLEMENT_CHAR('S');
  case 20:
    RETURN_COMPLEMENT_CHAR('A');
  case 21:
    RETURN_COMPLEMENT_CHAR('A');
  case 22:
    THROW_COMPLEMENT_CHAR('B');
  case 23:
    RETURN_COMPLEMENT_CHAR('W');
  case 25:
    THROW_COMPLEMENT_CHAR('R');
  default:
    RETURN_COMPLEMENT_CHAR('\0');
  }
#undef THROW_COMPLEMENT_CHAR
#undef RETURN_COMPLEMENT_CHAR
#undef SET_CCH
  if (throwChar) {
    throw complementChar;
  } else {
    return complementChar;
  }
}

char getComplementChar(char original) {
  char ch;
  try {
    ch = callComplement(&complement, original);
    __truffletaint_assertnot_char(ch);
  } catch (char taintedChar) {
    ch = taintedChar;
    __truffletaint_assert_char(ch);
  }
  return ch;
}

struct chunk {
  chunk() : previous(0), next(0), length(0) {}
  chunk(chunk *previous) : previous(previous), next(0), length(0) {
    previous->next = this;
  }
  chunk *previous;
  chunk *next;
  unsigned short length;
  char data[CHUNK_SIZE];
};

chunk *compute_reverse_complement(chunk *begin, chunk *end) {
  chunk *start = begin;
  char *begin_char = begin->data;
  char *end_char = end->data + end->length - 1;
  while (begin != end || begin_char < end_char) {
    char temp = getComplementChar(*begin_char);
    *begin_char++ = getComplementChar(*end_char);
    *end_char-- = temp;
    if (*begin_char == '\n') {
      ++begin_char;
    }
    if (*end_char == '\n') {
      --end_char;
    }
    if (begin_char == begin->data + begin->length) {
      begin = begin->next;
      begin_char = begin->data;
      if (*begin_char == '\n') {
        ++begin_char;
      }
    }
    if (end_char == end->data - 1) {
      end = end->previous;
      end_char = end->data + end->length - 1;
      if (*end_char == '\n') {
        --end_char;
      }
    }
  }

  return start;
}

void checkReverseComplement(chunk *start) {
  for (chunk *current = start; current; current = current->next) {
    for (int i = 0; i < current->length; i++) {
      char ch = current->data[i];
      switch (ch) {
      case 'T':
      case 'G':
      case 'C':
      case 'M':
      case 'N':
      case 'S':
      case 'B':
      case 'R':
        __truffletaint_assert_char(ch);
        break;
      default:
        __truffletaint_assertnot_char(ch);
        break;
      }
    }
  }
}

void reverseComplement() {

  BENCHMARK_IO_OPEN_INPUT_FILE(ioObj);
  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  void *temp = nullptr;
  uint64_t tempLen = 0L;
  char *buf = nullptr;
  uint64_t bufSize = 0L;
  uint64_t bytesWritten = 0L;

  while (BENCHMARK_IO_CAN_READ_LINE(ioObj)) {

    BENCHMARK_IO_READ_LINE(ioObj, temp, tempLen, buf, bufSize, bytesWritten)
    BENCHMARK_IO_WRITE(ioObj, buf, bytesWritten);
    BENCHMARK_IO_WRITE(ioObj, "\n", 1);

    chunk *start = new chunk();
    chunk *end = start;
    while (BENCHMARK_IO_CAN_READ_LINE(ioObj) &&
           BENCHMARK_IO_PEEK(ioObj) != '>') {
      for (int line = 0; line < 1074 && BENCHMARK_IO_CAN_READ_LINE(ioObj) &&
                         BENCHMARK_IO_PEEK(ioObj) != '>';
           ++line) {

        BENCHMARK_IO_READ_LINE(ioObj, temp, tempLen, buf, bufSize, bytesWritten)

        if (bytesWritten > MAX_LINE_LENGTH) {
          throw "Unexpected line length";
        }

        memcpy(end->data + end->length, buf, bytesWritten);
        end->length += bytesWritten + 1;
        *(end->data + end->length - 1) = '\n';
      }

      if (BENCHMARK_IO_CAN_READ_LINE(ioObj) &&
          BENCHMARK_IO_PEEK(ioObj) != '>') {
        end = new chunk(end);
      }
    }
    --end->length;

    start = compute_reverse_complement(start, end);

    checkReverseComplement(start);

    while (start) {
      BENCHMARK_IO_WRITE(ioObj, start->data, start->length);
      chunk *last = start;
      start = start->next;
      delete last;
    }
    start = nullptr;
    BENCHMARK_IO_WRITE(ioObj, "\n", 1);
  }

  if (buf) {
    free(buf);
  }

  BENCHMARK_IO_CLOSE_INPUT_FILE(ioObj);
  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);
}

void setup(void *args) {
  ioObj = args;
  callComplement =
      (char (*)(char (*)(char), char))polyglot_eval("js", JS_CALL_COMPLEMENT);
}

int benchmark() {
  reverseComplement();
  return 0;
}

int getExpectedResult() { return 0; }
}
