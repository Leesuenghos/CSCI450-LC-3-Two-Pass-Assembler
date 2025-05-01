/** @file operand.c
 * @brief represent LC-3 operands for translation
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * LC-3 operands consist of either REGISTER (R0, R1, etc.), a numeric literal
 * (#1, x3528), a string literal for the STRINGZ pseudo-opcode,
 * or a symbol which is a label to an address. We will attempt to
 * support decimal literals that start with # or hex that start with
 * x or X (also allow 0x/0X). String literals will always
 * start and end with quote ".
 *
 * The main purpose of this module is to take a token that should be
 * an operand, and translate/decode it value.
 */
#define __STDC_WANT_LIB_EXT2__ 1
#include "operand.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief construct operand
 *
 * Construct a new operand.  We dynamically allocate
 * and return a operand structure and initialize
 * it.
 *
 * @returns operand* Returns pointer to newly allocated
 *   and initialized operand object.
 */
operand* opr_construct()
{

  // allocate the operand and initialize it to be empty
  operand* opr = (operand*)malloc(sizeof(operand));
  opr->opr = UNKOWN;
  opr->value = 0;
  opr->svalue = NULL;
  opr->token = NULL;
  return opr;
}

/** @brief destruct operand
 *
 * Destruct a operand.  Free any memory allocated by the operand and
 * then free the operand object itself.
 *
 * @param opr A pointer to the dynamically allocated operand
 *   that should be destroyed.
 */
void opr_destruct(operand* opr)
{
  if (opr->token)
    free(opr->token);
  if (opr->svalue)
    free(opr->svalue);
  // free up the opc object
  free(opr);
}

/** @brief extract operand
 *
 * In the LC-3 assembley there will be from 0 to 3 operands on all
 * valid lines with an opcode representing a machine operation.
 * This method is told which token in the list of tokens is the
 * operand that it should extract.  The work of this method is
 * to determine the operand type, parse the operand value, and then
 * return an instance of an operand for processing in the assembler.
 *
 * @param tks An list of tokens from an operation line being
 *   processed.
 * @param tk_pos The position of the token that is the operand to
 *   extract and process.
 *
 * @returns operand Returns a created operand object with the processed
 *   operand value and other information about the operand.
 */
operand* extract_operand(tokens* tks, int tk_pos)
{
  // create an operand instance to hold the extracted operand information
  operand* opr = opr_construct();
  opr->token = strdup(tks->token[tk_pos]);

  if (is_register(opr->token))
  {
    extract_register(opr);
  }
  else if (is_string(opr->token))
  {
    extract_string(opr);
  }
  else if (is_hex_literal(opr->token))
  {
    extract_hex_literal(opr);
  }
  else if (is_decimal_literal(opr->token))
  {
    extract_decimal_literal(opr);
  }
  // if we don't detect the operand type, only remaining possibility is to
  // treat it as a symbol/label
  else
  {
    extract_symbol(opr);
    // fprintf(stderr, "<operand::extract_operand> Could not extract operand token <%s>\n", opr->token);
    // exit(1);
  }

  return opr;
}

/** @brief extract register operand
 *
 * We have determined that the operand token represents a register,
 * so extract the register value into the operand object.
 *
 * @param operand A pointer to the already created operand that
 *   has a register to extract and configure.
 */
void extract_register(operand* opr)
{
  opr->opr = REGISTER;
  // TODO: should we be more defensive, what if this fails?  Will usually
  // end up with register value of 0;
  opr->value = atoi(&(opr->token[1]));
}

/** @brief extract string operand
 *
 * We have determined that the operand token is a string literal.
 * Extract the string from the token and put into the svalue.
 * We want everything between the opening " and closing ".
 *
 * @param operand A pointer to the already created operand that
 *   has a string literal to extract and configure.
 */
void extract_string(operand* opr)
{
  opr->opr = STRING;

  // we are assuming that the first character at index 0 is a "
  // and the last character at strlen-1 is a ", get everything
  // in between, what if this isn't true?
  // allocate memory to hold new copy of only the string without the
  // quotes, so size-2, but we need to add in a terminator \0, so
  // size-1
  int size = strlen(opr->token);
  opr->svalue = (char*)malloc(size - 1);
  strncpy(opr->svalue, &(opr->token[1]), size - 2);

  // the strncpy doesn't add in a string terminator, so add it
  opr->svalue[size - 2] = '\0';

  // usually the terminator \0 is not counted in the string size, but
  // we need to take it into account when determining the size of this
  // operand, so our size is size - 2 (removed the ""), + 1 (for \0 at end)
  opr->value = size - 1;
}

/** @brief extract hex literal
 *
 * We have determined that the operand token is a hexadecimal
 * literal value.  Convert the hex value from string to
 * its numeric value in the operand.
 *
 * @param operand A pointer to the already created operand that
 *   has a string literal to extract and configure.
 */
void extract_hex_literal(operand* opr)
{
  opr->opr = NUMERIC;

  // first character is x or first two are 0x
  int idx = 1;
  if ((opr->token[idx] == 'X') || (opr->token[idx] == 'x'))
  {
    idx = 2;
  }
  opr->value = (uint16_t)strtol(&(opr->token[idx]), NULL, 16);
}

/** @brief extract decimal literal
 *
 * We have determined that the operand token is a decimal
 * literal value.  Convert the hex value from string to
 * its numeric value in the operand.
 *
 * @param operand A pointer to the already created operand that
 *   has a string literal to extract and configure.
 */
void extract_decimal_literal(operand* opr)
{
  opr->opr = NUMERIC;

  // first character might be # to indicate a decimal literal
  int idx = 0;
  if (opr->token[idx] == '#')
  {
    idx = 1;
  }
  opr->value = (uint16_t)atoi(&(opr->token[idx]));
}

/** @brief extract symbol
 *
 * The operand does not appear to be a numeric literal or
 * a string literal.  So it must be a symbol/label.
 *
 * @param operand A pointer to the already created operand that
 *   has a symbol/label to extract and configure.
 */
void extract_symbol(operand* opr)
{
  opr->opr = SYMBOL;
  opr->svalue = strdup(opr->token);
}

const char* registr_str[] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"};

/** @brief operand to string
 *
 * Return a (const c array/string) for an operand,
 * used to display tokenizing/assembling results.
 *
 * @param opr The operand to return a string representation of.
 */
const char* opr_str(operand* opr)
{
  switch (opr->opr)
  {
  case REGISTER:
    return registr_str[opr->value];
  case NUMERIC:
    return opr->token;
  case STRING:
    return opr->token;
  case SYMBOL:
    return opr->svalue;
  default:
    return "";
  }
}

/** @brief test if token is a register operand
 *
 * Any token that starts with a "R" will be treated as a
 * register operand.
 */
bool is_register(const char* token)
{
  // task 4 part 1 goes here
  return token[0] == 'R';
}

/** @brief test if token is a string literal
 *
 * Any token that starts with a quote character "
 * will be assumed to be a string literal.
 */
bool is_string(const char* token)
{
  // task 4 part 2 goes here
  return token[0] == '"';
}

/** @brief is hex digit
 *
 * Can't use isdigit() for hex numbers, so add in a-f A-F
 * as valid hex digits along with the decimal digits.
 *
 * @param c The character to test if it is 0-9 A-F
 *
 * @returns bool true if character c is a valid hex digit,
 *   false if it is not.
 */
bool is_hex_digit(char c)
{
  // task 4 part 3 goes here
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/** @brief test if token is a hext literal
 *
 * Any token that has pattern
 * xD
 * XD
 * 0xD
 * 0XD
 * Where is a hex digit, will be treated as a hexadecimal
 * literal value.
 */
bool is_hex_literal(const char* token)
{
  // task 4 part 4 goes here
  if ((token[0] == 'x' || token[0] == 'X') && is_hex_digit(token[1]))
  {
    return true;
  }
  if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X') && is_hex_digit(token[2]))
  {
    return true;
  }
  return false;
}

/** @brief test if token is a decimal literal
 *
 * Any token that starts with # or with a digit
 * (0x has already been detected) will be treated as a literal
 * decimal value.
 */
bool is_decimal_literal(const char* token)
{
  // task 4 part 5 goes here
  if (token[0] == '#')
  {
    if (token[1] == '-' && isdigit(token[2]))
      return true;
    if (isdigit(token[1]))
      return true;
  }
  else
  {
    // allow plain decimal like "5" for pseudo-opcodes
    if (isdigit(token[0]))
      return true;
  }
  return false;
}