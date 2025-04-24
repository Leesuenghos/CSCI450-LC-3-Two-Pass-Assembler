/** @file assembler.h
 * @brief LC-3 Assembler
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * Header include file for implementation of two-pass
 * assembler for the LC-3 assembly language ISA.  This module
 * has a lot of helper functions, but the main work are done
 * in the `pass_one()` and `pass_two()` functions to implement the
 * two pass translation process.  This module makes use of the
 * `symbol-table` module to build and maintain a dictionary of
 * labels and addresses encountered in pass 1, and a linked list of
 * `operation-list` entries that is constructed in pass 1, and processed
 * and completed to assemble the machine instructions and final binary
 * file in pass two.
 */
#include "operation-list.h"
#include "symbol-table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef LC3VMASSEMBLER_H
#define LC3VMASSEMBLER_H

/// flags are used in testing and in the operation-list so make them visible
enum flags
{
  FP = 1 << 0,
  FZ = 1 << 1,
  FN = 1 << 2
};

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

void lc3asm(const char* asmfile, const char* binfile, bool verbose);
operation_list* pass_one(tokenizer* tk, symbol_table* st);
void pass_two(operation_list* opl, symbol_table* st);
void write_bin_file(const char* binfile, operation_list* opl, bool verbose);
char* bin_file_name(const char* asmfile);

bool match(const char* symbol1, const char* symbol2);
char* check_for_symbol(tokens* tks);
void calculate_symbol_offset(operand* opr, uint16_t opaddress, symbol_table* st);
void asm_inst(opl_entry* entry);
uint16_t asm_add(opl_entry* entry);
uint16_t asm_and(opl_entry* entry);
uint16_t asm_br(opl_entry* entry);
uint16_t asm_fill(opl_entry* entry);
uint16_t asm_jmp(opl_entry* entry);
uint16_t asm_jsr(opl_entry* entry);
uint16_t asm_ld(opl_entry* entry);
uint16_t asm_ldi(opl_entry* entry);
uint16_t asm_ldr(opl_entry* entry);
uint16_t asm_lea(opl_entry* entry);
uint16_t asm_not(opl_entry* entry);
uint16_t asm_rti(opl_entry* entry);
uint16_t asm_st(opl_entry* entry);
uint16_t asm_sti(opl_entry* entry);
uint16_t asm_str(opl_entry* entry);
uint16_t asm_stringz(opl_entry* entry);
uint16_t asm_trap(opl_entry* entry);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // LC3VMASSEMBLER_H
