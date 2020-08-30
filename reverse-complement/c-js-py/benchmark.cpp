// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/revcomp-gpp-3.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
// Contributed by Paul Kitchin

#include <taint.h>
#include <polyglot.h>
#include <cstdlib>
#include <cstdio>

void *ioObj = nullptr;
char (*throwComplement)(char);
char (*catchComplement)(char, char (*)(char));

char getComplementChar(char original)
{
   char ch = catchComplement(original, throwComplement);
   return ch;
}

struct chunk
{
   chunk() : previous(0), next(0), length(0) {}
   chunk(chunk *previous) : previous(previous), next(0), length(0)
   {
      previous->next = this;
   }
   chunk *previous;
   chunk *next;
   unsigned short length;
   char data[65526];
};

chunk *compute_reverse_complement(chunk *begin, chunk *end)
{
   chunk *start = begin;
   char *begin_char = begin->data;
   char *end_char = end->data + end->length - 1;
   while (begin != end || begin_char < end_char)
   {
      char beginCharComplement = getComplementChar(*begin_char);
      char endCharComplement = getComplementChar(*end_char);
      *begin_char++ = endCharComplement;
      *end_char-- = beginCharComplement;
      if (*begin_char == '\n')
      {
         ++begin_char;
      }
      if (*end_char == '\n')
      {
         --end_char;
      }
      if (begin_char == begin->data + begin->length)
      {
         begin = begin->next;
         begin_char = begin->data;
         if (*begin_char == '\n')
         {
            ++begin_char;
         }
      }
      if (end_char == end->data - 1)
      {
         end = end->previous;
         end_char = end->data + end->length - 1;
         if (*end_char == '\n')
         {
            --end_char;
         }
      }
   }

   return start;
}

void checkReverseComplement(chunk *start)
{
   for (chunk *current = start; current; current = current->next)
   {
      for (int i = 0; i < current->length; i++)
      {
         char ch = current->data[i];
         switch (ch)
         {
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

void reverseComplement()
{

   polyglot_invoke(ioObj, "openInputFile");
   polyglot_invoke(ioObj, "openOutputFile");

   void *temp = nullptr;
   uint64_t tempLen = 0L;
   char *buf = nullptr;
   uint64_t bufSize = 0L;
   uint64_t bytesWritten = 0L;

   while (polyglot_as_boolean(polyglot_invoke(ioObj, "canReadLine")))
   {

      temp = polyglot_invoke(ioObj, "readLine");
      tempLen = polyglot_get_string_size(temp);
      if (bufSize < ((tempLen + 1) * 4))
      {
         bufSize = ((tempLen + 1) * 4);
         if (buf)
         {
            free(buf);
         }
         buf = (char *)calloc(bufSize, sizeof(char));
      }
      bytesWritten = polyglot_as_string(temp, buf, bufSize, "UTF-8");
      temp = (void *)0L;
      tempLen = 0L;
      polyglot_invoke(ioObj, "write", polyglot_from_string_n(buf, bytesWritten, "UTF-8"));
      polyglot_invoke(ioObj, "write", polyglot_from_string_n("\n", 1, "UTF-8"));

      chunk *start = new chunk();
      chunk *end = start;
      while (polyglot_as_boolean(polyglot_invoke(ioObj, "canReadLine")) &&
             polyglot_as_i32(polyglot_invoke(ioObj, "peekNextChar")) != '>')
      {
         for (int line = 0; line < 1074 && polyglot_as_boolean(polyglot_invoke(ioObj, "canReadLine")) &&
                            polyglot_as_i32(polyglot_invoke(ioObj, "peekNextChar")) != '>';
              ++line)
         {

            temp = polyglot_invoke(ioObj, "readLine");
            tempLen = polyglot_get_string_size(temp);
            if (bufSize < ((tempLen + 1) * 4))
            {
               bufSize = ((tempLen + 1) * 4);
               if (buf)
               {
                  free(buf);
               }
               buf = (char *)calloc(bufSize, sizeof(char));
            }
            bytesWritten = polyglot_as_string(temp, buf, bufSize, "UTF-8");
            temp = (void *)0L;
            tempLen = 0L;

            if (bytesWritten > 60)
            {
               throw "Unexpected line length";
            }

            memcpy(end->data + end->length, buf, bytesWritten);
            end->length += bytesWritten + 1;
            *(end->data + end->length - 1) = '\n';
         }

         if (polyglot_as_boolean(polyglot_invoke(ioObj, "canReadLine")) &&
             polyglot_as_i32(polyglot_invoke(ioObj, "peekNextChar")) != '>')
         {
            end = new chunk(end);
         }
      }
      --end->length;

      start = compute_reverse_complement(start, end);

      checkReverseComplement(start);

      while (start)
      {
         polyglot_invoke(ioObj, "write", polyglot_from_string_n(start->data, start->length, "UTF-8"));
         chunk *last = start;
         start = start->next;
         delete last;
      }
      start = nullptr;
      polyglot_invoke(ioObj, "write", polyglot_from_string_n("\n", 1, "UTF-8"));
   }

   if (buf)
   {
      free(buf);
   }

   polyglot_invoke(ioObj, "closeInputFile");
   polyglot_invoke(ioObj, "closeOutputFile");
}

void setup(void *args)
{
   ioObj = args;
   throwComplement =
       (char (*)(char))polyglot_eval("python", "from polyglot import import_value\n"
                                               "Taint = import_value(\"taint\")\n"
                                               "\n"
                                               "\n"
                                               "class TaintedValueError(Exception):\n"
                                               "    def __init__(self, ch):\n"
                                               "        self.ch = ch\n"
                                               "\n"
                                               "    def get_value(self):\n"
                                               "        return self.ch\n"
                                               "\n"
                                               "\n"
                                               "def complement(character):\n"
                                               "    if character == ord('A') or character == ord('a'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('T'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('C') or character == ord('c'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('G'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('G') or character == ord('g'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('C'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('T') or character == ord('t'):\n"
                                               "        return ord('A')\n"
                                               "    elif character == ord('U') or character == ord('u'):\n"
                                               "        return ord('A')\n"
                                               "    elif character == ord('M') or character == ord('m'):\n"
                                               "        return ord('K')\n"
                                               "    elif character == ord('R') or character == ord('r'):\n"
                                               "        return ord('Y')\n"
                                               "    elif character == ord('W') or character == ord('w'):\n"
                                               "        return ord('W')\n"
                                               "    elif character == ord('S') or character == ord('s'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('S'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('Y') or character == ord('y'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('R'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('K') or character == ord('k'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('M'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('V') or character == ord('v'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('B'))"
                                               ")"
                                               "\n"
                                               "    elif character == ord('H') or character == ord('h'):\n"
                                               "        return ord('D')\n"
                                               "    elif character == ord('D') or character == ord('d'):\n"
                                               "        return ord('H')\n"
                                               "    elif character == ord('B') or character == ord('b'):\n"
                                               "        return ord('V')\n"
                                               "    elif character == ord('N') or character == ord('n'):\n"
                                               "        raise "
                                               "Taint.add("
                                               "TaintedValueError(ord('N'))"
                                               ")"
                                               "\n"
                                               "    else:\n"
                                               "        return None\n"
                                               "\ncomplement\n");
   catchComplement =
       (char (*)(char, char (*)(char)))polyglot_eval("js", "const Taint = Polyglot.import(\"taint\");\n"
                                                           "\n"
                                                           "function catchComplement(originalChar, complement) {\n"
                                                           "    var complementChar;\n"
                                                           "    try {\n"
                                                           "        complementChar = complement(originalChar);\n"
                                                           "        "
                                                           "Taint.assertNotTainted("
                                                           "complementChar"
                                                           ")"
                                                           ";\n"
                                                           "    } catch (taintedValueError) {\n"
                                                           "        "
                                                           "Taint.assertTainted("
                                                           "taintedValueError"
                                                           ")"
                                                           ";\n"
                                                           "        complementChar = taintedValueError.get_value()\n"
                                                           "        complementChar = "
                                                           "Taint.add("
                                                           "complementChar"
                                                           ")"
                                                           ";\n"
                                                           "        "
                                                           "Taint.assertTainted("
                                                           "complementChar"
                                                           ")"
                                                           ";\n"
                                                           "    }\n"
                                                           "    return complementChar;\n"
                                                           "}\n"
                                                           "\n(catchComplement)\n");
}

extern "C"
{

   int benchmark()
   {
      reverseComplement();
      return 0;
   }

   void *createDefaultBenchmarkArg()
   {
      // usually, the BenchmarkIO is supplied by the benchmark harness, but one can
      // also supply it manually
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
             "reverse-complement"
             "\n");
      struct timeval start, end;
      for (unsigned int i = 0; i < iterations; i++)
      {
         gettimeofday(&start, __null);
         int result = benchmark();
         gettimeofday(&end, __null);
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
}
