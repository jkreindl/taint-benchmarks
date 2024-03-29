// adapted from
// https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-gcc-1.html

// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// Contributed by Jeremy Zerfas

#define BENCHMARK_NAME "k-nucleotide"
#define BENCHMARK_RESULT_TYPE 1
#define NATIVE_INPUT_FILE "knucleotide-input-250000.fasta"
#include "../common.h"

void *ioObj;

// This controls the maximum length for each set of oligonucleotide frequencies
// and each oligonucleotide count output by this program.
#define MAXIMUM_OUTPUT_LENGTH 4096

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "khash.h"

// Define a custom hash function to use instead of khash's default hash
// function. This custom hash function uses a simpler bit shift and XOR which
// results in several percent faster performance compared to when khash's
// default hash function is used.
#define CUSTOM_HASH_FUNCTION(key) ((key) ^ (key) >> 7)

KHASH_INIT(oligonucleotide, uint64_t, uint32_t, 1, CUSTOM_HASH_FUNCTION,
           kh_int64_hash_equal)

// intptr_t should be the native integer type on most sane systems.
typedef intptr_t intnative_t;

typedef struct
{
  uint64_t key;
  uint32_t value;
} element;

// Macro to convert a nucleotide character to a code. Note that upper and lower
// case ASCII letters only differ in the fifth bit from the right and we only
// need the three least significant bits to differentiate the letters 'A', 'C',
// 'G', and 'T'. Spaces in this array/string will never be used as long as
// characters other than 'A', 'C', 'G', and 'T' aren't used.
#define code_For_Nucleotide(nucleotide) (" \0 \1\3  \2"[nucleotide & 0x7])

// And one more macro to convert the codes back to nucleotide characters.
#define nucleotide_For_Code(code) ("ACGT"[code & 0x3])

// Function to use when sorting elements with qsort() later. Elements with
// larger values will come first and in cases of identical values then elements
// with smaller keys will come first.
static int element_Compare(const element *const left_Element,
                           const element *const right_Element)
{

  // Sort based on element values.
  if (left_Element->value < right_Element->value)
    return 1;
  if (left_Element->value > right_Element->value)
    return -1;

  // If we got here then both items have the same value so then sort based on
  // key.
  return left_Element->key > right_Element->key ? 1 : -1;
}

// Generate frequencies for all oligonucleotides in polynucleotide that are of
// desired_Length_For_Oligonucleotides and then save it to output.
static void generate_Frequencies_For_Desired_Length_Oligonucleotides(
    const char *const polynucleotide, const intnative_t polynucleotide_Length,
    const intnative_t desired_Length_For_Oligonucleotides, char *const output)
{

  khash_t(oligonucleotide) *hash_Table = kh_init(oligonucleotide);

  uint64_t key = 0;
  const uint64_t mask =
      ((uint64_t)1 << 2 * desired_Length_For_Oligonucleotides) - 1;

  // For the first several nucleotides we only need to append them to key in
  // preparation for the insertion of complete oligonucleotides to hash_Table.
  for (intnative_t i = 0; i < desired_Length_For_Oligonucleotides - 1; i++)
    key = (key << 2 & mask) | polynucleotide[i];

  // Add all the complete oligonucleotides of
  // desired_Length_For_Oligonucleotides to hash_Table and update the count
  // for each oligonucleotide.
  for (intnative_t i = desired_Length_For_Oligonucleotides - 1;
       i < polynucleotide_Length; i++)
  {

    key = (key << 2 & mask) | polynucleotide[i];

    int element_Was_Unused;
    const khiter_t k =
        kh_put(oligonucleotide, hash_Table, key, &element_Was_Unused);

    // If the element_Was_Unused, then initialize the count to 1, otherwise
    // increment the count.
    if (element_Was_Unused)
      kh_value(hash_Table, k) = __truffletaint_add_int(1);
    else
      kh_value(hash_Table, k)++;
  }

  // Create an array of elements from hash_Table.
  intnative_t elements_Array_Size = kh_size(hash_Table), i = 0;
  element *elements_Array = malloc(elements_Array_Size * sizeof(element));
  uint32_t value;
  kh_foreach(hash_Table, key, value,
             elements_Array[i++] = ((element){key, value}));

  kh_destroy(oligonucleotide, hash_Table);

  // Sort elements_Array.
  qsort(elements_Array, elements_Array_Size, sizeof(element),
        (int (*)(const void *, const void *))element_Compare);

  // Print the frequencies for each oligonucleotide.
  for (intnative_t output_Position = 0, i = 0; i < elements_Array_Size; i++)
  {

    // Convert the key for the oligonucleotide to a string.
    char oligonucleotide[desired_Length_For_Oligonucleotides + 1];
    for (intnative_t j = desired_Length_For_Oligonucleotides - 1; j > -1; j--)
    {
      oligonucleotide[j] = nucleotide_For_Code(elements_Array[i].key);
      elements_Array[i].key >>= 2;
    }
    oligonucleotide[desired_Length_For_Oligonucleotides] = '\0';

#define FRACTION_SCALE 1000

    uint64_t count = 100L * elements_Array[i].value * FRACTION_SCALE;
    long total =
        polynucleotide_Length - desired_Length_For_Oligonucleotides + 1;
    long fraction = (count / total);

    __truffletaint_assert_long(fraction);
    fraction = __truffletaint_remove_long(fraction);

    // Output the frequency for oligonucleotide to output.
    // note: snprintf in glibc and musl exhibit different rounding behavior, we
    // format the fraction manually to get consistent behavior
    output_Position += snprintf(
        output + output_Position, MAXIMUM_OUTPUT_LENGTH - output_Position,
        "%s %ld.%03ld\n", oligonucleotide, fraction / FRACTION_SCALE,
        fraction % FRACTION_SCALE);
  }

  free(elements_Array);
}

// Generate a count for the number of times oligonucleotide appears in
// polynucleotide and then save it to output.
static void generate_Count_For_Oligonucleotide(
    const char *const polynucleotide, const intnative_t polynucleotide_Length,
    const char *const oligonucleotide, char *const output)
{
  const intnative_t oligonucleotide_Length = strlen(oligonucleotide);

  khash_t(oligonucleotide) *const hash_Table = kh_init(oligonucleotide);

  uint64_t key = 0;
  const uint64_t mask = ((uint64_t)1 << 2 * oligonucleotide_Length) - 1;

  // For the first several nucleotides we only need to append them to key in
  // preparation for the insertion of complete oligonucleotides to hash_Table.
  for (intnative_t i = 0; i < oligonucleotide_Length - 1; i++)
    key = (key << 2 & mask) | polynucleotide[i];

  // Add all the complete oligonucleotides of oligonucleotide_Length to
  // hash_Table and update the count for each oligonucleotide.
  for (intnative_t i = oligonucleotide_Length - 1; i < polynucleotide_Length;
       i++)
  {

    key = (key << 2 & mask) | polynucleotide[i];

    int element_Was_Unused;
    const khiter_t k =
        kh_put(oligonucleotide, hash_Table, key, &element_Was_Unused);

    // If the element_Was_Unused, then initialize the count to 1, otherwise
    // increment the count.
    if (element_Was_Unused)
      kh_value(hash_Table, k) = __truffletaint_add_int(1);
    else
      kh_value(hash_Table, k)++;
  }

  // Generate the key for oligonucleotide.
  key = 0;
  for (intnative_t i = 0; i < oligonucleotide_Length; i++)
    key = (key << 2) | code_For_Nucleotide(oligonucleotide[i]);

  // Output the count for oligonucleotide to output.
  khiter_t k = kh_get(oligonucleotide, hash_Table, key);
  uintmax_t count = k == kh_end(hash_Table) ? 0 : kh_value(hash_Table, k);
  __truffletaint_assert_unsigned_long(count);
  count = __truffletaint_remove_unsigned_long(count);
  snprintf(output, MAXIMUM_OUTPUT_LENGTH, "%ju\t%s", count, oligonucleotide);

  kh_destroy(oligonucleotide, hash_Table);
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
            code_For_Nucleotide(__truffletaint_add_char(buffer[i]));

    // Make sure we still have enough memory allocated for any potential
    // nucleotides in the next line.
    if (polynucleotide_Capacity - polynucleotide_Length < sizeof(buffer))
      polynucleotide = realloc(polynucleotide, polynucleotide_Capacity *= 2);
  }

  BENCHMARK_IO_CLOSE_INPUT_FILE(ioObj);

  // Free up any leftover memory.
  polynucleotide = realloc(polynucleotide, polynucleotide_Length);

  char output_Buffer[7][MAXIMUM_OUTPUT_LENGTH];

  generate_Count_For_Oligonucleotide(polynucleotide, polynucleotide_Length,
                                     "GGTATTTTAATTTATAGT", output_Buffer[6]);

  generate_Count_For_Oligonucleotide(polynucleotide, polynucleotide_Length,
                                     "GGTATTTTAATT", output_Buffer[5]);

  generate_Count_For_Oligonucleotide(polynucleotide, polynucleotide_Length,
                                     "GGTATT", output_Buffer[4]);

  generate_Count_For_Oligonucleotide(polynucleotide, polynucleotide_Length,
                                     "GGTA", output_Buffer[3]);

  generate_Count_For_Oligonucleotide(polynucleotide, polynucleotide_Length,
                                     "GGT", output_Buffer[2]);

  generate_Frequencies_For_Desired_Length_Oligonucleotides(
      polynucleotide, polynucleotide_Length, 2, output_Buffer[1]);

  generate_Frequencies_For_Desired_Length_Oligonucleotides(
      polynucleotide, polynucleotide_Length, 1, output_Buffer[0]);

  BENCHMARK_IO_OPEN_OUTPUT_FILE(ioObj);

  // Output the results to stdout.
  for (intnative_t i = 0; i < 7; i++)
  {
    BENCHMARK_IO_WRITE(ioObj, output_Buffer[i], strlen(output_Buffer[i]));
    BENCHMARK_IO_WRITE_CHAR(ioObj, '\n');
  }

  BENCHMARK_IO_CLOSE_OUTPUT_FILE(ioObj);

  free(polynucleotide);
}

int benchmark()
{
  knucleotide();
  return 0;
}

int getExpectedResult() { return 0; }

void setup(void *arg) { ioObj = arg; }
