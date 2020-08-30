// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/revcomp-gpp-3.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
// Contributed by Paul Kitchin

#include <taint.h>
#include <polyglot.h>
#include <cstdlib>
#include <cstdio>

const char *PY_CALL_COMPLEMENT = "def callComplement(complement, character):\n"
                                 "    return complement(character)\n"
                                 "\n\n"
                                 "callComplement\n";

void *ioObj = nullptr;
char (*callComplement)(char (*)(char), char) = nullptr;

char complement(char character)
{
   char complementChar;
   bool throwChar;
   switch (character & 0x1f)
   {
   case 1:
   {
      complementChar = __truffletaint_add_char('T');
      throwChar = true;
      break;
   };
   case 2:
   {
      complementChar = 'V';
      throwChar = false;
      break;
   };
   case 3:
   {
      complementChar = __truffletaint_add_char('G');
      throwChar = true;
      break;
   };
   case 4:
   {
      complementChar = 'H';
      throwChar = false;
      break;
   };
   case 7:
   {
      complementChar = __truffletaint_add_char('C');
      throwChar = true;
      break;
   };
   case 8:
   {
      complementChar = 'D';
      throwChar = false;
      break;
   };
   case 11:
   {
      complementChar = __truffletaint_add_char('M');
      throwChar = true;
      break;
   };
   case 13:
   {
      complementChar = 'K';
      throwChar = false;
      break;
   };
   case 14:
   {
      complementChar = __truffletaint_add_char('N');
      throwChar = true;
      break;
   };
   case 18:
   {
      complementChar = 'Y';
      throwChar = false;
      break;
   };
   case 19:
   {
      complementChar = __truffletaint_add_char('S');
      throwChar = true;
      break;
   };
   case 20:
   {
      complementChar = 'A';
      throwChar = false;
      break;
   };
   case 21:
   {
      complementChar = 'A';
      throwChar = false;
      break;
   };
   case 22:
   {
      complementChar = __truffletaint_add_char('B');
      throwChar = true;
      break;
   };
   case 23:
   {
      complementChar = 'W';
      throwChar = false;
      break;
   };
   case 25:
   {
      complementChar = __truffletaint_add_char('R');
      throwChar = true;
      break;
   };
   default:
   {
      complementChar = '\0';
      throwChar = false;
      break;
   };
   }

   if (throwChar)
   {
      throw complementChar;
   }
   else
   {
      return complementChar;
   }
}

char getComplementChar(char original)
{
   char ch;
   try
   {
      ch = callComplement(&complement, original);
      __truffletaint_assertnot_char(ch);
   }
   catch (char taintedChar)
   {
      ch = taintedChar;
      __truffletaint_assert_char(ch);
   }
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
      char temp = getComplementChar(*begin_char);
      *begin_char++ = getComplementChar(*end_char);
      *end_char-- = temp;
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
   callComplement = (char (*)(char (*)(char), char))polyglot_eval(
       "python", PY_CALL_COMPLEMENT);
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
