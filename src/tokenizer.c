/** @file tokenizer.c
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
#define __STDC_WANT_LIB_EXT2__ 1
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief construct tokenizer
 *
 * Construct a new tokenizer.  We dynamically allocate
 * and return a tokenizer structure and initialize
 * it.  We attempt to open the file.  This function
 * prints an error message and exits if the file fails to
 * open.
 *
 * @param asmfile The input assembly file to tokenize and parse.
 *
 * @returns tokenizer* Returns pointer to newly allocated
 *   and initialized tokenizer with the file opened and ready
 *   to read and tokenize lines.
 */
tokenizer* tk_construct(const char* asmfile)
{

  // allocate the tokenizer  and initialize basic parameters
  tokenizer* tk = (tokenizer*)malloc(sizeof(tokenizer));

  tk->asmfile = strdup(asmfile);
  tk->linenum = 1;

  // attempt to open up the file input stream
  tk->in = fopen(asmfile, "r");
  if (tk->in == NULL)
  {
    fprintf(stderr, "<tokeniser::tk_construct> File Not Found: <%s>\n", asmfile);
    exit(1);
  }

  return tk;
}

/** @brief destruct tokenizer
 *
 * Destruct a tokenizer.  Close the open file and deallocate memory,
 * and finally destruct the tokenizer object itself.
 *
 * @param tk A pointer to the dynamically allocated tokenizer
 *   that should be destroyed.
 */
void tk_destruct(tokenizer* tk)
{
  // first deallocate the filename we duplicated
  free(tk->asmfile);

  // Now ensure that the file is closed
  fclose(tk->in);

  // once all entries are deallocated, free the tokenizer itself
  free(tk);
}

/** @brief destruct tokens list
 *
 * Destruct a line tokens list.  We dynamically allocate one for each
 * line in tk_next_line(), as well as duplicating all token strings.
 * So it is up to the caller of tk_next_line() to call this once
 * done with processing the line of tokens.
 *
 * @param tks A pointer to a dynamically allocated list of tokens
 *    from a line.
 */
void tokens_destruct(tokens* tks)
{
  // the line and all token in the list are duplicates, so free them
  free(tks->line);
  for (int index = 0; index < tks->num_tokens; index++)
  {
    free(tks->token[index]);
  }

  // finally free up the tokens object
  free(tks);
}

/** @brief tokenize next line
 *
 * Find next line with opcode/operation on it.  Then split up into
 * individual tokens and return as a tokens object.
 *
 * @param tk A pointer to the tokenizer that has the current state of
 *   our tokenization so far in the file.
 *
 * @returns tokens* A pointer to a new set of tokens on the next valid
 *   operation line to be processed.  Returns NULL if no more lines/tokens
 *   are left in stream.
 */
tokens* tk_next_line(tokenizer* tk)
{
  char buffer[4096];
  tokens* tks = NULL;
  char* token;
  bool done = false;

  // if we are at end of file then the input is done
  if (feof(tk->in))
  {
    return NULL;
  }

  // we have to get lines until we find a line that is not blank or a
  // comment.
  while (!done)
  {
    // get the next line of input
    fgets(buffer, sizeof(buffer), tk->in);

    // attempt to tokenize line elements, separate by whitespace and commas
    token = strtokquote(buffer, " \n\t,");

    // if line was empty we get a NULL token, or if first character is ;
    // then line only has a comments.  In both cases continue to next line
    if ((token == NULL) || token[0] == ';')
    {
      tk->linenum++;
      continue;
    }

    // otherwise we found a line with an operation to parse
    done = true;
  }

  // We are tokenizing a line, create a tokens object to return and fill it with tokens
  tks = (tokens*)malloc(sizeof(tokens));
  tks->line = strdup(buffer);
  tks->linenum = tk->linenum;
  tks->token[0] = strdup(token);
  tks->num_tokens = 1;

  // extract remaining tokens from buffer, passing NULL to
  // strtok continues tokenization where left off in buffer
  while ((token = strtokquote(NULL, " \n\t,")) != NULL)
  {
    // throw away rest of line when we encounter comment
    if ((token == NULL) || (token[0] == ';'))
      break;

    tks->token[tks->num_tokens] = strdup(token);
    tks->num_tokens++;
  }

  tk->linenum++;
  return tks;
}

/** @brief a tokenizer that keeps "string literal" as a single token
 *
 * Based on discussion:
 *  https://stackoverflow.com/questions/26187037/in-c-split-char-on-spaces-with-strtok-function-except-if-between-quotes
 * Replace strtok with a wrapper/version that treats an opening " as the
 * beginning of a block, and will skip delimiters until the closing " to
 * close the block.  The parameters and way this works should be the same
 * as the `strtok()` library function except for keeping quoted stings
 * as a single token.
 *
 * @param input The input buffer, an array of characters, that is to be tokenized.
 *    The location of the last token found is kept, so if called subsequently with
 *    input as NULL, tokenization continues on in the input from the previous
 *    last token found.
 * @param delimit A character array of delimiters.  All characters in this array
 *   are treated as differentiating legal tokens.  Sequences of delimiters will be
 *   skipped over.  The only exception is that delimiters in a string block will
 *   not be skipped.
 *
 * @returns char* Returns the next token found, or NULL when no more tokens can be found
 *   in the original buffer.
 */
char* strtokquote(char* input, char* delimit)
{
  // notice that token is statically allocated, it persists on subsequent calls
  // to strtokquote
  static char* token = NULL;
  char* start = NULL;

  // initial call, set token to of beginning string
  if (input != NULL)
  {
    token = input;
  }

  // skip over any delimiters at beginning
  while ((strchr(delimit, *token) != NULL) && (*token != '\0'))
  {
    token++;
  }

  // when we find that token is at \0 we are done tokenizing
  if (*token == '\0')
  {
    return NULL;
  }

  // we are not done, set start of next token to current non
  // delimiter and search for end
  start = token;

  // now search for next delimiter, or end of string if
  // we get into a string block
  if (*token == '"')
  {
    start = token; // Start from opening quote
    token++;       // move past opening quote
    while (*token != '"' && *token != '\0')
    {
      token++;
    }
    if (*token == '"')
    {
      token++; // move past closing quote
    }
  }
  else
  {
    start = token;
    while (!strchr(delimit, *token) && *token != '\0')
    {
      token++;
    }
  }

  // Found delimiter, make it end of token that begins at start and
  // move to next character in token buffer in preparation for next call
  if (*token != '\0')
  {
    *token = '\0';
    token++;
  }

  return start;
}