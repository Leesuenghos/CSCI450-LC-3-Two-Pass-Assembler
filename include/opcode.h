/** @file opcode.h
 * @brief represent LC-3 opcodes for translation
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * LC-3 opcode and pseudo-opcode parsing and assembly.  All definitions to identify,
 * represent and assemble LC-3 opcodes in an LC-3 assembly file are
 * in this module.  We define a basic enumerated type for all LC-3
 * opcodes and pseudo assembler operations.  This module also supports a
 * struct object that holds information about a parsed opcode, like
 * NZP flags for BR and different variants like JSR/JSRR, needed to assemble
 * opcode into a machine instruction.
 */
#include "tokenizer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef OPCODE_H
#define OPCODE_H

/// Opcode enumerated type.  There are only 15/16 valid opcodes
/// and 5 pseudo opcodes in LC-3.  The opcodes have the
/// correct machine instruction 4bit value for each
/// one in this enum.  The pseudo opcodes are numbered starting
/// at 0x20.  The enum value for the real opcodes directly
/// corresponds to the 4-bit machine instruction.  The value for
/// the pseudo-opcodes is arbitrary and not used in the translation.
typedef enum opctype
{
  BR = 0x0, // used for tokens BRn, BRz, BRp, BRnz, BRnp, BRzp, BRnzp
  ADD,
  LD,
  ST,
  JSR, // JSR and JSRR tokens
  AND,
  LDR,
  STR,
  RTI, // = 0x8 only instruction with no operands
  NOT,
  LDI,
  STI,
  JMP,      // JMP and RET are 1100 0xC
  RESERVED, // not used, doesn't map to a token
  LEA,
  TRAP, // 0xF
  ORIG = 0x20,
  END,
  BLKW,
  FILL,
  STRINGZ
} opctype;

/// The opcode holds information about the operation
/// opcode that each LC-3 assembly operation holds.
/// All information needed to translate the opcode into
/// a machine instruction is gathered in this struct.
typedef struct opcode
{
  // the basic opcode type
  opctype opc;

  // for BR opcodes, indicate which flags NZP were set
  // like in the LC3vm we use the low 3 bits to hold flags
  uint16_t flags;

  // There are two ADD, AND, JMP/RET and JSR/JSSR variants.  If the
  // opcode is ADD, AND, or JSR/JSRR we may use this field:
  // 0 means JSR with a PCOffset11
  // 1 means JSRR return using BaseR
  uint16_t variant;

  // keep the actual opcode string token if needed;
  char* token;
} opcode;

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

opcode* opc_construct();
void opc_destruct(opcode* opc);
opcode* extract_opcode(tokens* tks);
const char* opc_str(opcode* opc);
bool is_keyword(const char* token);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // OPCODE_H
