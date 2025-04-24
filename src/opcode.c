/** @file opcode.c
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
#define __STDC_WANT_LIB_EXT2__ 1
#include "opcode.h"
#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief construct opcode
 *
 * Construct a new opcode.  We dynamically allocate
 * and return a opcode structure and initialize
 * it.
 *
 * @returns opcode* Returns pointer to newly allocated
 *   and initialized opcode object.
 */
opcode* opc_construct()
{

  // allocate the opcode and initialize it to be empty
  opcode* opc = (opcode*)malloc(sizeof(opcode));
  opc->opc = RESERVED;
  opc->flags = 0x0;
  opc->variant = 0;
  opc->token = NULL;
  return opc;
}

/** @brief destruct opcode
 *
 * Destruct a opcode.  Deallocate any memory allocated for the
 * opcode then free the opcode object itself.
 *
 * @param opc A pointer to the dynamically allocated opcode
 *   that should be destroyed.
 */
void opc_destruct(opcode* opc)
{
  // free up the opc object
  free(opc);
}

/** @brief extract opcode
 *
 * In the LC-3 assembley all valid lines have to have 1 opcode or
 * pseudo-opcode.  This will be in token[0] of the tokens if there
 * is no label, or tokens[1] otherwise.
 *
 * This method finds which token is the opcode/pseudoopcode keyword and
 * then converts it into an enum opcode type.
 *
 * @param tks An object of the tokens from an operation line being
 *   processed.
 *
 * @returns opcode* Returns the extracted opcode information as an opcode
 *   object for processing by the assembler.
 */
opcode* extract_opcode(tokens* tks)
{
  char* opcode_keyword;
  if (is_keyword(tks->token[0]))
  {
    opcode_keyword = tks->token[0];
  }
  else if (is_keyword(tks->token[1]))
  {
    opcode_keyword = tks->token[1];
  }
  // opcode should be in either first or second token,
  // if not we just throw an error and stop
  else
  {
    fprintf(stderr, "<opcode::extract_opcode> Error: did not find opcode token on line <%s> <%s>\n", tks->token[0], tks->token[1]);
    exit(1);
  }

  // create an opcode instance to hold the extracted opcode information
  opcode* opc = opc_construct();

  // is there a better way to do this?  for now brute
  // force it
  if (match(opcode_keyword, "BR"))
  {
    opc->opc = BR;
    opc->flags = 0x00;
  }
  else if (match(opcode_keyword, "BRn"))
  {
    opc->opc = BR;
    opc->flags |= FN;
  }
  else if (match(opcode_keyword, "BRz"))
  {
    opc->opc = BR;
    opc->flags |= FZ;
  }
  else if (match(opcode_keyword, "BRp"))
  {
    opc->opc = BR;
    opc->flags |= FP;
  }
  else if (match(opcode_keyword, "BRnz"))
  {
    opc->opc = BR;
    opc->flags |= (FN | FZ);
  }
  else if (match(opcode_keyword, "BRnp"))
  {
    opc->opc = BR;
    opc->flags |= (FN | FP);
  }
  else if (match(opcode_keyword, "BRzp"))
  {
    opc->opc = BR;
    opc->flags |= (FZ | FP);
  }
  else if (match(opcode_keyword, "BRnzp"))
  {
    opc->opc = BR;
    opc->flags |= (FN | FZ | FP);
  }
  else if (match(opcode_keyword, "ADD"))
  {
    opc->opc = ADD;
  }
  else if (match(opcode_keyword, "LD"))
  {
    opc->opc = LD;
  }
  else if (match(opcode_keyword, "ST"))
  {
    opc->opc = ST;
  }
  else if (match(opcode_keyword, "JSR"))
  {
    opc->opc = JSR;
    opc->variant = 1;
  }
  else if (match(opcode_keyword, "JSRR"))
  {
    opc->opc = JSR;
    opc->variant = 0;
  }
  else if (match(opcode_keyword, "AND"))
  {
    opc->opc = AND;
  }
  else if (match(opcode_keyword, "LDR"))
  {
    opc->opc = LDR;
  }
  else if (match(opcode_keyword, "STR"))
  {
    opc->opc = STR;
  }
  else if (match(opcode_keyword, "RTI"))
  {
    opc->opc = RTI;
  }
  else if (match(opcode_keyword, "NOT"))
  {
    opc->opc = NOT;
  }
  else if (match(opcode_keyword, "LDI"))
  {
    opc->opc = LDI;
  }
  else if (match(opcode_keyword, "STI"))
  {
    opc->opc = STI;
  }
  else if (match(opcode_keyword, "JMP"))
  {
    opc->opc = JMP;
    opc->variant = 0;
  }
  else if (match(opcode_keyword, "RET"))
  {
    opc->opc = JMP;
    opc->variant = 1;
  }
  else if (match(opcode_keyword, "LEA"))
  {
    opc->opc = LEA;
  }
  else if (match(opcode_keyword, "TRAP"))
  {
    opc->opc = TRAP;
  }
  else if (match(opcode_keyword, ".ORIG"))
  {
    opc->opc = ORIG;
  }
  else if (match(opcode_keyword, ".END"))
  {
    opc->opc = END;
  }
  else if (match(opcode_keyword, ".BLKW"))
  {
    opc->opc = BLKW;
  }
  else if (match(opcode_keyword, ".FILL"))
  {
    opc->opc = FILL;
  }
  else if (match(opcode_keyword, ".STRINGZ"))
  {
    opc->opc = STRINGZ;
  }
  // actually the first test using is_keyword should catch, but check again
  else
  {
    fprintf(stderr, "<opcode::extract_opcode> Error: did not find token on line, token <%s>\n", opcode_keyword);
    exit(1);
  }

  return opc;
}

/** @brief opcode to string
 *
 * Return a (const c array/string) for an opcode,
 * used to display tokenizing/assembling results.
 *
 * @param opc The opcode to return a string representation of.
 *
 * @returns const char* Returns a c character array representation
 *   of the opcode that was extracted.
 */
const char* opc_str(opcode* opc)
{
  uint16_t f = opc->flags;
  switch (opc->opc)
  {
  case BR:
    // need to look at flags and add/return them, probably much better way to do this than brute force...
    if ((f & FN) && (f & FZ) && (f & FP))
      return "BRnzp";
    else if ((f & FN) && (f & FZ))
      return "BRnz";
    else if ((f & FN) && (f & FP))
      return "BRnp";
    else if ((f & FZ) && (f & FP))
      return "BRzp";
    else if (f & FN)
      return "BRn";
    else if (f & FZ)
      return "BRz";
    else if (f & FP)
      return "BRp";
    else
      return "BR";
  case ADD:
    return "ADD";
  case ST:
    return "ST";
  case LD:
    return "LD";
  case JSR:
    if (opc->variant == 1)
      return "JSR";
    else
      return "JSRR";
  case AND:
    return "AND";
  case LDR:
    return "LDR";
  case STR:
    return "STR";
  case RTI:
    return "RTI";
  case NOT:
    return "NOT";
  case LDI:
    return "LDI";
  case STI:
    return "STI";
  case JMP:
    if (opc->variant == 0)
      return "JMP";
    else
      return "RET";
  case RESERVED:
    return "RESERVED";
  case LEA:
    return "LEA";
  case TRAP:
    return "TRAP";
  case ORIG:
    return ".ORIG";
  case END:
    return ".END";
  case BLKW:
    return ".BLKW";
  case FILL:
    return ".FILL";
  case STRINGZ:
    return ".STRINGZ";
  default:
    return "UNK";
  }
}

/// All LC-3 opcode keywords that we recognize in the assembler
#define NUM_KEYWORDS 29
char* keywords[] = {"BRn", "BRz", "BRp", "BRnz", "BRnp", "BRzp", "BRnzp", "ADD", "LD", "ST", "JSR", "JSRR", "AND", "LDR", "STR", "RTI",
  "NOT", "LDI", "STI", "JMP", "RET", "RESERVED", "LEA", "TRAP", ".ORIG", ".END", ".BLKW", ".FILL", ".STRINGZ"};

/** @brief is keyword
 *
 * Check if a token is a keyword or not.  Keywords indicate
 * an opcode or a pseudo opcode for the assembler.
 *
 * @param token The token to test if it is an opcode/pseudo
 *   opcode keyword.
 */
bool is_keyword(const char* token)
{
  // task 3 implementation goes here
  return false;
}
