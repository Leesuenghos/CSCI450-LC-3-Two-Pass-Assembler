/** @file tokenizer.h
 * @brief tokenize assembley operation lines
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * A line oriented tokenizer for tokenizing line based assembly operations.
 *
 * This tokenizer will hide/remove all comments and blank lines.  This tokenizer
 * includes our own implementation of the `strtok()` function, modified to return
 * string literals between quote " as single tokens.  The tokenizer returns a
 * list of unprocessed tokens as the ultimate result from tokenizing a line.
 * The assembler is responsible for interpreting the tokens found on each line,
 * and other tasks.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef TOKENIZER_H
#define TOKENIZER_H

/// A simple struct that the tokenizer can fill in with
/// the tokens it extracts from each line.
/// NOTE: We hardcode an upper limit of 5 tokens here in the
/// token array, which is fine for LC-3 assembly, but is a
/// limiting assumption
typedef struct tokens
{
  char* line;
  unsigned linenum;
  int num_tokens;
  char* token[5];
} tokens;

/// The tokenizer opens an assembly input file, and iterates through
/// each line of the file, tokenizing and returning the tokens found
/// from each line.
typedef struct tokenizer
{
  // The assembly file name we have open and are tokenizing
  char* asmfile;

  // The file input stream we have open and are reading from
  FILE* in;

  /// The line number we are currently on in the line-oriented tokenization
  unsigned linenum;

  // The current/last line we just read from file
  // char* line;
} tokenizer;

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

tokenizer* tk_construct(const char* asmfile);
void tk_destruct(tokenizer* tk);
void tokens_destruct(tokens* tks);
tokens* tk_next_line(tokenizer* tk);
char* strtokquote(char* input, char* delimit);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // TOKENIZER_H
