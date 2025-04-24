/** @file operand.h
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
#include "tokenizer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef OPERAND_H
#define OPERAND_H

/// Operand enumerated type.  Operands can be either
/// a register (R0, R1), a numeric literal (#1, x3050)
/// a string for the .STRINGZ pseudo-opcode ("Error message"),
/// or a SYMBOL which should be in the symbol-table.
typedef enum oprtype
{
  REGISTER = 0,
  NUMERIC,
  STRING,
  SYMBOL,
  UNKOWN
} oprtype;

enum registr // misspelled because register is a c/c++ keyword
{
  R0 = 0,
  R1,
  R2,
  R3,
  R4,
  R5,
  R6,
  R7,
};

/// The operand holds the value of a parsed/processed
/// operand from an operation line of assembly.  The
/// value will be either the register value, numeric value,
/// or the offset to a symbol (resolved in pass2).
/// If the operand is a string, then the svalue field
/// holds the string value.
typedef struct operand
{
  // the basic operand type
  oprtype opr;

  // The value, holds numeric value for REGISTER, NUMERIC
  // and SYMBOL.
  uint16_t value;

  // The svalue holds a string value for STRINGZ pseudo-ops
  char* svalue;

  // keep the actual operand token if needed
  char* token;
} operand;

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

operand* opr_construct();
void opr_destruct(operand* opr);
operand* extract_operand(tokens* tks, int tk_pos);
void extract_register(operand* opr);
void extract_string(operand* opr);
void extract_hex_literal(operand* opr);
void extract_decimal_literal(operand* opr);
void extract_symbol(operand* opr);
const char* opr_str(operand* opr);
bool is_register(const char* token);
bool is_string(const char* token);
bool is_hex_digit(char c);
bool is_hex_literal(const char* token);
bool is_decimal_literal(const char* token);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // OPERAND_H
