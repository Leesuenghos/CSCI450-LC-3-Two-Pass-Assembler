/** @file operation-list.h
 * @brief linked list of parsed assembly operations
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * Implementation of a list of parsed operations.  The operation list
 * will be created along with the symbol table in pass 1.  It will
 * allow us to avoid reopening and retokenizing the input file, as well
 * as preserve useful work from the first pass.  The resulting operation
 * list from the first pass will mostly just need the symbol operands to
 * calculate their offsets, and then determine the size of each operation
 * to finish the assembly process.
 *
 * Implemented as a basic linked list of opl_entry structures.  There will be one
 * opl_entry per each line in the input assembly file that contains an opcode/psedo opcode.
 */
#include "opcode.h"
#include "operand.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef OPERATION_LIST_H
#define OPERATION_LIST_H

/// opl_entry holds operation list entries that are maintained
/// as a linked list.
typedef struct opl_entry
{
  // The whole original line of this entry before translation
  char* line;

  // the actual line number in the input assembly file that this line was read from
  unsigned linenum;

  // The line label or NULL if line did not have a label
  char* label;

  // all valid lines must have an opcode or pseudo opcode, this is only
  // field that is required to be non empty / not NULL
  opcode* opc;

  // Operands, there can be from 0 to 3 operands depending on opcode
  int num_opr;
  operand* opr[3];

  // The address assigned to this operation in 1st pass
  uint16_t address;

  // The size of this line in machine words, usually 1 except for some
  // pseudoops that allocate a block or a string of multiple words
  uint16_t size;

  // The final compiled machine instruction, the target of the work of the
  // assembler in pass 2 for each line.  For pseudo ops where size is more than 1,
  // we would have multiple machine instructions, but only first one is kept here
  uint16_t inst;

  // The next operation entry in the operation linked list
  struct opl_entry* next;
} opl_entry;

/// The operation_list keeps track of the linked list of opl_entry
/// items while constructed from pass 1, and supports iterating over the list.
typedef struct operation_list
{
  /// current number of entries being held in the operation list
  unsigned num_entries;

  // The size of the program in 16-bit words that is held in this
  // list.
  unsigned size;

  // front of the operation_list
  opl_entry* front;

  // back of the operation_list
  opl_entry* back;

  // current iteration location when iterating the operation_list
  opl_entry* current;
} operation_list;

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

operation_list* opl_construct();
void opl_destruct(operation_list* opl);
opl_entry* opl_append(operation_list* opl, const char* line, unsigned linenum, const char* label, opcode* opc, uint16_t address);
void opl_append_operand(opl_entry* entry, operand* opr);
opl_entry* opl_begin(operation_list* opl);
opl_entry* opl_next(operation_list* opl);
void opl_display(operation_list* opl);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // OPERATION_LIST_H
