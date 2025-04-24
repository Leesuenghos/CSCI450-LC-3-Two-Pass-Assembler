/** @file operation-list.c
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
#define __STDC_WANT_LIB_EXT2__ 1
#include "operation-list.h"
#include "operand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief construct operation_list
 *
 * Construct a new operation list.  We dynamically allocate
 * and return a operation_list structure and initialize
 * it.  Initially the list is empty and all pointers of the
 * linked list point to NULL.
 *
 * @returns operation_list* Returns pointer to newly allocated
 *   and initialized operation list.
 */
operation_list* opl_construct()
{

  // allocate the operation list and initialize basic parameters
  operation_list* opl = (operation_list*)malloc(sizeof(operation_list));

  opl->num_entries = 0;
  opl->size = 0;
  opl->front = opl->back = opl->current = NULL;

  return opl;
}

/** @brief destruct operation_list
 *
 * Destruct an operation list.  Deallocate all opl_entry in list
 * then deallocate the top-level object.
 *
 * @param opl A pointer to the dynamically allocated operation_list
 *   that should be destroyed.
 */
void opl_destruct(operation_list* opl)
{
  // first traverse the list to deallocate all entries
  opl_entry* current = opl_begin(opl);
  while (current)
  {
    // deallocate the string labels that were duplicated
    free(current->line);
    free(current->label);
    opc_destruct(current->opc);

    // deallocate the operands here
    for (int idx = 0; idx < current->num_opr; idx++)
    {
      opr_destruct(current->opr[idx]);
    }

    // deallocate this entry
    free(current);

    // go to next entry
    current = opl_next(opl);
  }

  // once all entries are deallocated, free the operation_list itself
  free(opl);
}

/** @brief append new operation entry
 *
 * Append a new entry to the list.  We will be passed in the information that
 * pass 1 can determine when adding an entry.
 *
 * @param opl A pointer to the operation list to append new operation on.
 * @param line The original whole line containing this operation we are
 *   are appending.
 * @param linenum The line number in the input file we are translating that this
 *   operation was found and processed from.
 * @param label If the line has a label it is passed in, if not this is NULL.
 * @param opc The opcode of this entry, this is required to be present on every
 *   operation line for an LC-3 assembly file.
 * @param address The address of this operation determined by the first pass.
 *
 * @returns opl_entry* A pointer to the new entry is returned, or NULL if
 *   an error occurs on the insertion.
 */
opl_entry* opl_append(operation_list* opl, const char* line, unsigned linenum, const char* label, opcode* opc, uint16_t address)
{
  opl_entry* entry;

  // allocate a new entry
  entry = (opl_entry*)malloc(sizeof(opl_entry));

  // initialize the entry with the passed in information
  entry->line = strdup(line);
  entry->linenum = linenum;
  if (label)
    entry->label = strdup(label);
  else
    entry->label = NULL;
  entry->opc = opc;
  entry->address = address;
  entry->num_opr = 0;
  entry->size = 0x0;
  entry->inst = 0x0;
  entry->next = NULL;

  // if list is empty, front and back will now both be the new entry
  if (opl->front == NULL)
  {
    opl->front = opl->back = entry;
  }
  else // append to back of list
  {
    opl->back->next = entry;
    opl->back = entry;
  }
  opl->num_entries++;

  return entry;
}

/** @brief append new operand to the opl_entry
 *
 * Append a new operand to the given opl_entry.  There are at most 3 operands
 * for each operation line in an LC-3 assembly file.  We just add the
 * operand to the end of the array of operands for this entry.
 *
 * @param entry The operation list entry that we are appending a new operand
 *   on to.
 * @param opr The operand structure instance we are adding to the operation
 *   list entry.
 */
void opl_append_operand(opl_entry* entry, operand* opr)
{
  entry->opr[entry->num_opr] = opr;
  entry->num_opr++;
}

/** @brief begin iteration
 *
 * Set the iteration of list back to the beginning of the list.
 *
 * @param opl A pointer to the operation list to reset iteration for.
 *
 * @returns opl_entry* Returns pointer to the first operation list entry.
 */
opl_entry* opl_begin(operation_list* opl)
{
  opl->current = opl->front;
  return opl->current;
}

/** @brief next iteration
 *
 * Return current entry when iterating the list and move pointer up to
 * next entry.
 *
 * @param opl A pointer to the operation list to iterate next item for.
 *
 * @returns opl_entry* Returns a pointer to the next operation list
 *   entry in the current iteration.
 */
opl_entry* opl_next(operation_list* opl)
{
  opl->current = opl->current->next;
  return opl->current;
}

/** @brief display operation_list
 *
 * Display contents of a operation_list on stdout. Useful after pass 2
 * to see results of the assembly before writing final obj file.
 *
 * @param opl A pointer to the operation_list to display on stdout.
 */
void opl_display(operation_list* opl)
{
  printf("%-20s%-10s%-40s %-4s: %-4s %16s\n", "LABEL", "OPCODE", "OPERANDS", "ADDR", "INST", "BINARY");
  printf("--------------------------------------------------------------------------------------------------\n");
  opl_entry* current = opl_begin(opl);
  while (current)
  {
    // determine string for label to display
    const char* label;
    if (current->label)
      label = current->label;
    else
      label = "";

    // create a string of the operands to display
    char oprbuf[100];
    oprbuf[0] = '\0';
    for (int idx = 0; idx < current->num_opr; idx++)
    {
      strcat(oprbuf, opr_str(current->opr[idx]));
      if (idx < current->num_opr - 1)
      {
        strcat(oprbuf, ", ");
      }
    }

    printf("%-20s%-10s%-40s %04X: %04X %016b\n", label, opc_str(current->opc), oprbuf, current->address, current->inst, current->inst);

    // BLKW and STRINGZ pseudo ops actually will assemble to multiple address words, handle their output specially here
    if (current->opc->opc == BLKW)
    {
      char* empty = ".";
      uint16_t i = 0x0000;
      uint16_t address = current->address + 1;
      for (int idx = 1; idx < current->size; idx++)
      {
        printf("%-20s%-10s%-40s %04X: %04X %016b\n", "", empty, empty, address, i, i);
        address++;
      }
    }
    if (current->opc->opc == STRINGZ)
    {
      char* empty = ".";
      uint16_t i = 0x0000;
      uint16_t address = current->address + 1;
      for (int idx = 1; idx < current->size; idx++)
      {
        i = (uint16_t)current->opr[0]->svalue[idx];
        printf("%-20s%-10s%-40s %04X: %04X %016b\n", "", empty, empty, address, i, i);
        address++;
      }
    }

    current = opl_next(opl);
  }
}
