// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/pidigits-node-3.html

/* The Computer Language Benchmarks Game
 * https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
 *
 * contributed by Basit Ayantunde
 */

#define BENCHMARK_NAME "pidigits"
#define BENCHMARK_RESULT_TYPE 1
#include "../common.h"

#include <malloc.h>

const char *JS_PIDIGITS =
    "const Taint = Polyglot.import(\"taint\");\n"
    "\n"
    "function pidigits (n, buf) {\n"
    "\n"
    "    // Int32\n"
    "    let i = 0,\n"
    "        k = 0,\n"
    "        d = 0,\n"
    "        k2 = 0,\n"
    "        d3 = 0,\n"
    "        d4 = 0;\n"
    "\n"
    "    let tmp1 = Taint.addTaint(0n), // mpz_init(tmp1)\n"
    "        tmp2 = Taint.addTaint(0n), // mpz_init(tmp2)\n"
    "        acc = Taint.addTaint"
    "(0n), // mpz_init_set_ui(acc, 0)\n"
    "        den = Taint.addTaint"
    "(1n), // mpz_init_set_ui(den, 1)\n"
    "        num = Taint.addTaint"
    "(1n); // mpz_init_set_ui(num, 1)\n"
    "\n"
    "    const chr_0 = \"0\".charCodeAt(0),\n"
    "          chr_t = \"\\t\".charCodeAt(0),\n"
    "          chr_n = \"\\n\".charCodeAt(0),\n"
    "          chr_c = \":\".charCodeAt(0);\n"
    "\n"
    "    let bufoffs = 0;\n"
    "\n"
    "    while (i < n) {\n"
    "        k++;\n"
    "\n"
    "        //#region inline nextTerm(k)\n"
    "        k2 = k * 2 + 1;\n"
    "        acc += num * 2n; // mpz_addmul_ui(acc, num, 2)\n"
    "        acc *= BigInt(k2); // mpz_mul_ui(acc, acc, k2)\n"
    "        den *= BigInt(k2); // mpz_mul_ui(den, den, k2)\n"
    "        num *= BigInt(k); // mpz_mul_ui(num, num, k)\n"
    "        //#endregion inline nextTerm(k)\n"
    "\n"
    "        if (num > acc /* mpz_cmp(num, acc) > 0 */ ) continue;\n"
    "\n"
    "        //#region inline extractDigit(3);\n"
    "        tmp1 = num * 3n; // mpz_mul_ui(tmp1, num, nth);\n"
    "        tmp2 = tmp1 + acc; // mpz_add(tmp2, tmp1, acc);\n"
    "        tmp1 = tmp2 / den; // mpz_tdiv_q(tmp1, tmp2, den);\n"
    "        d3 = Number(tmp1) >>> 0; // mpz_get_ui(tmp1)\n"
    "        //#region inline extractDigit(3);\n"
    "\n"
    "        d = d3;\n"
    "\n"
    "        //#region inline extractDigit(4);\n"
    "        tmp1 = num * 4n; // mpz_mul_ui(tmp1, num, nth);\n"
    "        tmp2 = tmp1 + acc; // mpz_add(tmp2, tmp1, acc);\n"
    "        tmp1 = tmp2 / den; // mpz_tdiv_q(tmp1, tmp2, den);\n"
    "        d4 = Number(tmp1) >>> 0; // mpz_get_ui(tmp1)\n"
    "        //#region inline extractDigit(4);\n"
    "\n"
    "        if (d !== d4) continue;\n"
    "\n"
    "        buf.writeInt8(d + chr_0, bufoffs++);\n"
    "\n"
    "        if (++i % 10 === 0) {\n"
    "            buf.writeInt8(chr_t, bufoffs++);\n"
    "            buf.writeInt8(chr_c, bufoffs++);\n"
    "\n"
    "            bufoffs = buf.writeNumberAsString(i, bufoffs);\n"
    "\n"
    "            buf.writeInt8(chr_n, bufoffs++);\n"
    "        }\n"
    "\n"
    "        //#region inline eliminateDigit(d)\n"
    "        acc -= den * BigInt(d); // mpz_submul_ui(acc, den, d)\n"
    "        acc *= 10n; // mpz_mul_ui(acc, acc, 10)\n"
    "        num *= 10n; // mpz_mul_ui(num, num, 10)\n"
    "        //#endregion inline eliminateDigit(d)\n"
    "    }\n"
    "\n"
    "    Taint.assertNotTainted(i);\n"
    "    Taint.assertNotTainted(k);\n"
    "    Taint.assertTainted(d);\n"
    "    Taint.assertTainted(d3);\n"
    "    Taint.assertTainted(d4);\n"
    "\n"
    "    Taint.assertTainted(tmp1);\n"
    "    Taint.assertTainted(tmp2);\n"
    "    Taint.assertTainted(acc);\n"
    "    Taint.assertTainted(den);\n"
    "    Taint.assertTainted(num);\n"
    "}\n"
    "\n"
    "(pidigits)\n";

class Buffer {
private:
  char *data;
  size_t length;

  int countDigits(int num) {
    int digits = 1;
    while (num >= 10) {
      digits++;
      num = num / 10;
    }
    return digits;
  }

public:
  Buffer() {
    this->length = 0;
    this->data = nullptr;
  }

  Buffer(size_t length_) {
    this->length = length_;
    this->data = (char *)calloc(length_, sizeof(char));
  }

  int writeNumberAsString(int, int);

  int writeInt8(int, int);

  void writeToNative(void *);

  ~Buffer() {
    free(this->data);
    this->length = 0;
    this->data = nullptr;
  }
};

int Buffer::writeNumberAsString(int num, int offset) {
  const int digits = countDigits(num);
  for (int curOffset = offset + digits - 1; curOffset >= offset; curOffset--) {
    data[curOffset] = '0' + num % 10;
    num = num / 10;
  }
  return offset + digits;
}

int Buffer::writeInt8(int ch, int offset) {
  if (ch != (ch & 0xff))
    throw "Invalid range for character, must be signed 8-bit number";

  data[offset] = (char)ch;
  return offset + 1;
}

void Buffer::writeToNative(void *ioObj) {
  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  for (int i = 0; i < this->length; ++i) {
    char ch = __truffletaint_remove_char(this->data[i]);
    BENCHMARK_IO_WRITE_CHAR(ioObj, ch);
  }

  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);
}

POLYGLOT_DECLARE_TYPE(Buffer)

void (*pidigits)(int, Buffer *);

void *ioObj;

extern "C" {

void setup(void *args) {
  ioObj = args;
  pidigits = (void (*)(int, Buffer *))polyglot_eval("js", JS_PIDIGITS);
}

// bufSize = (12 + countDigits(n) + 1) * (n / 10);
// for (let i = 10, length = 10 ** (Math.log10(n) >>> 0); i < length; i *= 10) {
//    bufsize -= i - 1;
// }
#define BUF_SIZE 4142

int benchmark() {
  Buffer *buf = new Buffer(BUF_SIZE);
  pidigits(2500, (Buffer *)polyglot_from_Buffer(buf));
  buf->writeToNative(ioObj);
  delete buf;

  return 0;
}

int getExpectedResult() { return 0; }
}
