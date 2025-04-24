/** @file assembler.c
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
#include "assembler.h"
#include "opcode.h"
#include "operand.h"
#include "operation-list.h"
#include "symbol-table.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief LC-3 Assembler
 *
 * Main function to assemble a file into LC-3 machine instructions.
 * This function is called by the main, and passed in command line
 * options/flags.  It controls creating a symbol table and tokenizer.
 * Pass 1 is run to fill in the symbol table and get the preliminary
 * list of operation lines that are extcted from the file.  Pass 2
 * uses the symbol table to calculate all symbol/label addresses and
 * construct the final machine instruction for each detected operation.
 * The function ends by writing the assembled program out to a binary
 * file.
 *
 * @param asmfile The name of the input file with LC-3 assembly
 *   operation lines to be parsed and assembled into a binary file.
 * @param binfile The name of the resulting binary/executable file that
 *   should be written after a successful 2 pass translation.
 * @param verbose If true we display results of symbol table creation in
 *   pass one, and linked list opcode/operand translation in pass two
 *   on standard output during the assembly process.
 */
void lc3asm(const char* asmfile, const char* binfile, bool verbose)
{
  // create symbol table and tokenizer needed in pass 1 and
  // pass 2
  tokenizer* tk = tk_construct(asmfile);
  symbol_table* st = st_construct(0);
  operation_list* opl;

  // perform pass 1 which fills in the symbol table and returns
  // the list of partially processed operation lines from the pass
  // to be used in pass two
  opl = pass_one(tk, st);
  if (verbose)
  {
    printf("Pass 1 Symbol Table Results\n");
    st_display(st);
  }

  // perform pass 2, which requires the symbol table and the operation list
  // created in pass 1
  pass_two(opl, st);
  if (verbose)
  {
    printf("\n\nPass 2 Assembly Results\n");
    opl_display(opl);
  }

  // determine output file name and write the assembled binary file
  if (!binfile)
  {
    binfile = bin_file_name(asmfile);
  }
  write_bin_file(binfile, opl, verbose);

  // cleanly close and exit
  tk_destruct(tk);
  st_destruct(st);
  opl_destruct(opl);
}

/** @brief assembly 1st pass
 *
 * Pass one of the LC-3 assembly process.  The purpose of this pass is to
 * extract and parse the lines from the assembly file using the tokenizer.
 * For each valid line with an LC-3 operation, we perform the following
 * - Extract opcode from the line, all valid operation lines have 1 opcode
 * - Check for an ORIG pseudoopcode and change the current address if directed
 * - Check for and extract line label and add the label to the symbol table
 * - Create an operation-list entry for this opcode line
 * - Parse the operands and add to the opertion-list
 * - Determine the machine instruction size for the current operation
 * - Update the address based on instruction size to determine location of next
 *   operation.
 *
 * @param tk A constructed and initialized tokenizer that will return token lists of
 *   each operation line from the file we are assembling.
 * @param st A constructed and empty symbol table that we insert label/address pairs
 *   into when we find a lable for an operation line.
 *
 * @returns operation_list* The first pass creats a list of partially processed
 *   operations and returns this list.  Each entry of the operation-list
 *   contains the processed opcode, operand, label, the address of this operation and
 *   the size of the machine instruction.
 */
operation_list* pass_one(tokenizer* tk, symbol_table* st)
{
  operation_list* opl = opl_construct();
  tokens* tks;
  opcode* opc;
  operand* opr;
  opl_entry* entry;

  char* label;
  uint16_t address = 0x0000;

  // tokenize each line with an opcode/operands operation we find
  while ((tks = tk_next_line(tk)) != NULL)
  {
    // extract opcode first
    opc = extract_opcode(tks);

    // check for ORIG pseudoopcode, which changes the address
    if (opc->opc == ORIG)
    {
      opr = extract_operand(tks, 1);
      address = opr->value;
    }

    // check for a line label symbol on this line and insert
    // it into the symbol table if needed
    int opr_index = 1;
    label = check_for_symbol(tks);
    if (label != NULL)
    {
      st_insert(st, label, address);
      // if label is present, operands begin at token 2 in the token list
      opr_index = 2;
    }

    // append new operation to operation list
    entry = opl_append(opl, tks->line, tks->linenum, label, opc, address);

    // extract the operands and add to the new entry
    for (int idx = opr_index; idx < tks->num_tokens; idx++)
    {
      opr = extract_operand(tks, idx);
      opl_append_operand(entry, opr);
    }

    // figure out this entry size to determine next address
    if (opc->opc == ORIG)
    {
      entry->size = 0;
    }
    else if (opc->opc == END)
    {
      entry->size = 0;
    }
    else if (opc->opc == BLKW)
    {
      entry->size = entry->opr[0]->value;
    }
    else if (opc->opc == STRINGZ)
    {
      entry->size = entry->opr[0]->value;
      // after we get size, set value to be first character for ease of
      // assembly
      entry->opr[0]->value = (uint16_t)entry->opr[0]->svalue[0];
    }
    // all others the size should be 1 memory word for the operation or pseudo op
    else
    {
      entry->size = 1;
    }

    // update address by the operation entry size to advance to next location for next
    // operation line we read
    address += entry->size;
    opl->size += entry->size;
  }

  return opl;
}

/** @brief assembly 2nd pass
 *
 * Perform the assembly second pass in order to construct the actual
 * machine instructions for each assembly operation.  For each entry in
 * the operation list returned from the first pass, we need to:
 * - Determine the value for any label which represents an offset, by
 *   looking up the symbol address and determining offset from current
 *   operation address to the label address.
 * - Construct the machine instruction using the opcode and operand
 *   values for the operation list entry.
 *
 * @param opl The linked list of operation lines partiatlly translated
 *   and constructued from the first pass.
 * @param st The symbol table constructed from the first pass, which contains
 *   the assigned addresses of all line labels encountered during that pass.
 *   These addresses are used when encountering SYMBOL operands to calculate
 *   relative offsets when assembling machine instructions.
 */
void pass_two(operation_list* opl, symbol_table* st)
{
  // iterate through the operation list entries starting at the beginning
  // of the operation list
  opl_entry* entry = opl_begin(opl);

  while (entry)
  {
    // if any operand is a SYMBOL, calculate its offset from the symbol address
    // in the symbol table
    operand* opr;
    for (int idx = 0; idx < entry->num_opr; idx++)
    {
      opr = entry->opr[idx];
      if (opr->opr == SYMBOL)
      {
        calculate_symbol_offset(opr, entry->address, st);
      }
    }

    // construct the machine instruction for this entry using the opcode and
    // list of operations.
    asm_inst(entry);

    // go to next entry
    entry = opl_next(opl);
  }
}

/** @brief Write binary file
 *
 * Write the final operation list assembled instructions out to the
 * resulting binary file.
 *
 * We have a primitive obj/exe format here we define.
 * All LC-3 bin files have the following expected format
 *
 * //number_of_sections
 * //section_num
 * section_address section_size
 * section_code
 * //section_num
 * section_address section_size
 * section_code
 *
 * NOTE: left in code for number_of_sections and section_num,
 * but decided to just use section_address section_size as
 * header for the binary, this allows for a simple linker
 * where we just concatenate sections together, and the loader
 * just keeps reading sections until reaches end of file
 *
 * All of the output are 16 bit words in the bin file.  The
 * first word specifies the number of sections total that are
 * in the binary file.  A section is basically a starting
 * address where the section should be loaded into the LC-3
 * machine memory, and then the assembled 16-bit instruction/data
 * words.  Each section is preceeded by its number, its starting
 * address, and the size in words of the assembled code. The
 * loader expects exactly the number of words specified are
 * present in the section_code, and that the next expected
 * section number is found after that if multiple sections
 * are linked together in the binary file.
 *
 * @param binfile The name of the binary file to be created and
 *   have the assembled instructions in the LC-3 bin format
 *   written into.
 * @param opl An operation list which contains all of the information,
 *   including the final assembled machine instructions, to be
 *   written into the resulting binary file.
 * @param verbose A boolen flag, if true we provide verbose status
 *   information when writing the resulting binary file output.
 */
void write_bin_file(const char* binfile, operation_list* opl, bool verbose)
{
  // open file for writing
  FILE* out = fopen(binfile, "wb");
  if (out == NULL)
  {
    fprintf(stderr, "<assembler::write_bin_file> error could not open file <%s>\n", binfile);
    exit(1);
  }

  // we currently only support a single section here, but
  // we might want to either create a linker and/or also allow
  // a program with multiple .ORIG/.END in same file?
  // output number_of_sections
  size_t total_writ = 0;
  size_t writ;
  size_t word_size = 2; // word size is 2 bytes for LC-3
  // uint16_t number_of_sections = 0x1;
  // writ = fwrite(&number_of_sections, word_size, 1, out);
  // total_writ += writ;

  // next the section_num, section_address and section_size are written
  // we assume that the starting section address is the address of the
  // first entry in the opl we have
  opl_entry* current;
  current = opl_begin(opl);
  uint16_t section_num = 0x1;
  uint16_t section_address = current->address;
  uint16_t section_size = opl->size;
  // writ = fwrite(&section_num, word_size, 1, out);
  // total_writ += writ;
  writ = fwrite(&section_address, word_size, 1, out);
  total_writ += writ;
  writ = fwrite(&section_size, word_size, 1, out);
  total_writ += writ;

  // now iterate through the list of assembled operations.
  // some pseudoops, such as .BLKW and .STRINGZ have multiple
  // words that are written
  while (current)
  {
    opctype opc = current->opc->opc;

    // output a block of 0's for BLKW pseudo op
    if (opc == BLKW)
    {
      uint16_t b = 0x0;
      uint16_t block_size = current->opr[0]->value;
      for (int idx = 0; idx < block_size; idx++)
      {
        writ = fwrite(&b, word_size, 1, out);
        total_writ += writ;
      }
    }
    // output string data for STRINGZ pseudo op
    else if (opc == STRINGZ)
    {
      uint16_t c;
      char* s = current->opr[0]->svalue;
      for (int idx = 0; idx < strlen(s); idx++)
      {
        c = s[idx];
        writ = fwrite(&c, word_size, 1, out);
        total_writ += writ;
      }
      // write a null '\0' at end of string
      c = 0x0;
      writ = fwrite(&c, word_size, 1, out);
      total_writ += writ;
    }
    // do nothing for ORIG begin and END end
    // though in future we could maybe start a new
    // section for each ORIG/END block defined ?
    else if (opc == ORIG || opc == END)
    {
      // pass, though could create sections here
    }
    // should be a LC3 operation or the FILL operation,
    // just output the single assembled instruction
    else
    {
      writ = fwrite(&current->inst, word_size, 1, out);
      total_writ += writ;
    }

    // process the next entry
    current = opl_next(opl);
  }

  if (verbose)
  {
    printf("Assembly complete\n");
    printf("    bin file: <%s>\n", binfile);
    printf("    words written: <0X%04X>\n", (int)total_writ);
    printf("    section: <0X%04X> address: <0x%04X> size: <0X%04X>\n", section_num, section_address, section_size);
  }
}

/** @brief binary file name
 *
 * Given the input assembly file name, determine the name for
 * the output file.  The user can specify an input and output
 * file name on the command line.  If no output file name is
 * given, this function removes any extension and adds .lc3
 * as the resulting binary file name in which to write the
 * assembled output into.
 *
 * @param asmfile The name of the input assembly language
 *   file that we are to construct the LC-3 binary file
 *   name for.  This function makes assumptions about the
 *   name, basically that the extenstion is .asm and that
 *   there is only 1 `.` present with that extension following.
 */
char* bin_file_name(const char* asmfile)
{
  char* binfile = strdup(asmfile);
  char* ext;

  // we find location of the expected file extension and remove
  // it
  ext = strstr(binfile, ".asm");
  if (ext == NULL)
  {
    fprintf(stderr, "<assembler::write_bin_file> unexpected file name, need .asm file <%s>\n", asmfile);
    exit(1);
  }
  ext[0] = '\0';

  // append the new output extension
  strcat(binfile, ".lc3");
  return binfile;
}

/** @brief match symbols
 *
 * Convenience method to determine if two c char array/string symbols
 * match or not.  For readability, so we don't have to check if
 * strcmp is 0 all the time.
 *
 * @param symbol1 A c char array/string to see if it matches with
 * @param symbol2 Another c char array/string we are seeing if match
 *
 * @returns bool true if the symbols are a match, false if not.
 */
bool match(const char* symbol1, const char* symbol2)
{
  return strcmp(symbol1, symbol2) == 0;
}

/** @brief check for symbol
 *
 * In the LC-3 assembley operations, a line of tokens has an optional
 * label that acts as a symbol for the symbol table.  If token 0 in
 * the list of tokens for the line is not a keyword opcode, then it must
 * be a label symbol.  This function returns the symbol, or NULL if
 * there is no label symbol on this line.
 *
 * @param tks An object of the tokens from an operation line being
 *   processed.
 *
 * @returns char* Returns the label symbol if present, or NULL if
 *   the line does not contain a label symbol.
 */
char* check_for_symbol(tokens* tks)
{
  // task 6 part 1 goes here
  return NULL;
}

/** @brief calculate symbol offset
 *
 * Symbols refer to line labels and should have
 * an entry in the symbol table from the first pass.
 * Lookup up the label address in the symbol table,
 * and calculate the correct offset from the current
 * operation address.  The result should be saved as
 * the operand value for use when assembling the machine
 * instruction.
 *
 * @param opr The operand of type SYMBOL that we need to
 *   determine the offset for.
 * @param opaddress The address of the current operation that
 *   we need to determine the offset from to the symbol address.
 * @param st The symbol table with label/address entries
 *   determined from the first pass of the assembler.
 *
 */
void calculate_symbol_offset(operand* opr, uint16_t opaddress, symbol_table* st)
{
  // task 6 part 2 goes here
}

/** @brief assemble LC-3 instruction
 *
 * Once all symbols have been resolved, addresses of each operation
 * determined, and all operands parsed and operand offsets calculated,
 * we are ready to determine and assemble the final LC-3 machine
 * instruction.
 *
 * An operation_list entry should have all of the information we need about the
 * opcode and parsed operands to assemble the instruction when this
 * function is called at the end of pass two.
 *
 * @param entry An operation_list entry with the information about the machine
 *   instruction to be assembled.
 */
void asm_inst(opl_entry* entry)
{
  // determine instruction so we can correctly interpet operands for assembly
  uint16_t i = 0x0000;
  switch (entry->opc->opc)
  {
  case ADD:
    i = asm_add(entry);
    break;
  case AND:
    i = asm_and(entry);
    break;
  case BR:
    i = asm_br(entry);
    break;
  case JMP: // JMP and RET instructions
    i = asm_jmp(entry);
    break;
  case JSR: // JSR and JSRR instructions
    i = asm_jsr(entry);
    break;
  case LD:
    i = asm_ld(entry);
    break;
  case LDI:
    i = asm_ldi(entry);
    break;
  case LDR:
    i = asm_ldr(entry);
    break;
  case LEA:
    i = asm_lea(entry);
    break;
  case NOT:
    i = asm_not(entry);
    break;
  case RTI:
    i = asm_rti(entry);
    break;
  case ST:
    i = asm_st(entry);
    break;
  case STI:
    i = asm_sti(entry);
    break;
  case STR:
    i = asm_str(entry);
    break;
  case TRAP:
    i = asm_trap(entry);
    break;
  // pseudo instructions
  case ORIG:
    // nothing to do, ORIG address initialization are processed in pass one
    break;
  case END:
    // nothing to do, though should we error check that all programs have an END?
    // TODO: by definition we should stop assembling on reaching END
    break;
  case BLKW:
    // nothing to do, operand indicates number of 0x0000 bytes to reserve for data
    break;
  case FILL:
    i = asm_fill(entry);
    break;
  case STRINGZ:
    i = asm_stringz(entry);
    break;
  default:
    fprintf(stderr, "Error: could not determine opcode type <%04X>\n", entry->opc->opc);
    exit(1);
  }
  entry->inst = i;
}

/** @brief assemble add operation
 *
 * ADD DR, SR1, SR2
 * ADD DR, SR, imm5
 * bit[5] = 0 if SR2, bit[5] = 1 if imm5
 *
 * @param entry The operation list entry that has an ADD opcode that needs to be
 *   assembled into a machine instruction.
 *
 * @returns uint16_t The assembled 16 bit instruction is returned from this
 *   function.
 */
uint16_t asm_add(opl_entry* entry)
{
  // error checking, we expect 3 operands to ADD, DR, SR1 and SR2/imm5
  if (entry->num_opr != 3)
  {
    fprintf(stderr, "<assembler::asm_add> Error malformed ADD instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // if both operands are registers, use version 0
  uint16_t i = 0x0000;
  uint16_t DR = entry->opr[0]->value;
  uint16_t SR1 = entry->opr[1]->value;
  if (entry->opr[2]->opr == REGISTER)
  {
    uint16_t SR2 = entry->opr[2]->value;
    i = (ADD << 12) | (DR << 9) | (SR1 << 6) | (SR2);
  }
  else
  {
    // only keep least significant 5 bits of operand 2 value
    // should we error check?  can only range from -0x10 to +0xF
    uint16_t imm5 = entry->opr[2]->value & 0x001F;
    // don't forget 1 at bit position 5 indicates immediate
    i = (ADD << 12) | (DR << 9) | (SR1 << 6) | (0x1 << 5) | (imm5);
  }
  return i;
}

/** @brief assemble and operation
 *
 * AND DR, SR1, SR2
 * AND DR, SR, imm5
 * bit[5] = 0 if SR2, bit[5] = 1 if imm5
 */
uint16_t asm_and(opl_entry* entry)
{
  // error checking, we expect 3 operands to AND, DR, SR1 and SR2/imm5
  if (entry->num_opr != 3)
  {
    fprintf(stderr, "<assembler::asm_and> Error malformed AND instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // if both operands are registers, use version 0
  uint16_t i = 0x0000;
  uint16_t DR = entry->opr[0]->value;
  uint16_t SR1 = entry->opr[1]->value;
  if (entry->opr[2]->opr == REGISTER)
  {
    uint16_t SR2 = entry->opr[2]->value;
    i = (AND << 12) | (DR << 9) | (SR1 << 6) | (SR2);
  }
  else
  {
    // only keep least significant 5 bits of operand 2 value
    // should we error check?  can only range from -0x10 to +0xF
    uint16_t imm5 = entry->opr[2]->value & 0x001F;
    // don't forget 1 at bit position 5 indicates immediate
    i = (AND << 12) | (DR << 9) | (SR1 << 6) | (0x1 << 5) | (imm5);
  }
  return i;
}

/** @brief assemble BR branch operation
 *
 * BRnzp PCoffset9
 */
uint16_t asm_br(opl_entry* entry)
{
  // error checking, we expect 1 operands to BR
  if (entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_br> Error malformed BR instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // the flags have already been set as the entries opcode flags, make sure only get
  // low 9 bits of the PCoffset9
  return (BR << 12) | (entry->opc->flags << 9) | (entry->opr[0]->value & 0x01FF);
}

/** @brief assemble .FILL pseudo operation
 *
 * .FILL 0x1234
 */
uint16_t asm_fill(opl_entry* entry)
{
  // error checking, we expect 1 operands to FILL
  if (entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_fill> Error malformed .FILL pseudo operation line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 8 bits of the trapvect8
  return (entry->opr[0]->value);
}

/** @brief assemble JMP/RET operation
 *
 * JMP R1
 * RET, implicit that base register is R7 for RET
 */
uint16_t asm_jmp(opl_entry* entry)
{
  // error checking, we expect 1 operand for JMP
  if (entry->opc->variant == 0 && entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_fill> Error malformed JMP operation line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }
  // error checking, we expect 0 operands for RET
  if (entry->opc->variant == 1 && entry->num_opr != 0)
  {
    fprintf(stderr, "<assembler::asm_fill> Error malformed RET operation line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // JMP variant
  if (entry->opc->variant == 0)
  {
    return (JMP << 12) | (entry->opr[0]->value << 6);
  }

  else if (entry->opc->variant == 1)
  {
    // defaults to base register R7 for return
    return (JMP << 12) | (7 << 6);
  }
  else
  {
    // should we put an error here instead?
    return 0x0;
  }
}

/** @brief assemble JSR/JSRR operation
 *
 * JSR PCoffset11
 * JSRR R4
 *
 * Both variants use opcode 0x4 0100.
 * Both variants expect 1 operand
 *
 */
uint16_t asm_jsr(opl_entry* entry)
{
  // error checking, we expect 1 operand for JSR/JSRR
  if (entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_jsr> Error malformed JSR/JSRR operation line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // JSRR variant, expect a register operand
  if (entry->opc->variant == 0)
  {
    return (JSR << 12) | (entry->opr[0]->value << 6);
  }
  // JSR should use PCoffset 11 value?, bit[11] is 1 for JSR
  else if (entry->opc->variant == 1)
  {
    return (JSR << 12) | (0x1 << 11) | (entry->opr[0]->value & 0x07FF);
  }
  else
  {
    // should we put an error here instead?
    return 0x0;
  }
}

/** @brief assemble LD operation
 *
 * LD DR, PCoffset9
 */
uint16_t asm_ld(opl_entry* entry)
{
  // error checking, we expect 2 operands to LD
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_ld> Error malformed LD instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 9 bits of the PCoffset9
  return (LD << 12) | (entry->opr[0]->value << 9) | (entry->opr[1]->value & 0x01FF);
}

/** @brief assemble LDI operation
 *
 * LDI DR, PCoffset9
 */
uint16_t asm_ldi(opl_entry* entry)
{
  // error checking, we expect 2 operands to LDI
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_ldi> Error malformed LDI instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 9 bits of the PCoffset9
  return (LDI << 12) | (entry->opr[0]->value << 9) | (entry->opr[1]->value & 0x01FF);
}

/** @brief assemble LDR operation
 *
 * LDI DR, BaseR, offset6
 */
uint16_t asm_ldr(opl_entry* entry)
{
  // error checking, we expect 3 operands to LDR
  if (entry->num_opr != 3)
  {
    fprintf(stderr, "<assembler::asm_ldr> Error malformed LDR instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  uint16_t DR = entry->opr[0]->value;
  uint16_t BaseR = entry->opr[1]->value;
  uint16_t offset6 = entry->opr[2]->value;

  // make sure only get low 9 bits of the PCoffset9
  return (LDR << 12) | (DR << 9) | (BaseR << 6) | (offset6 & 0x003F);
}

/** @brief assemble LEA operation
 *
 * LEA DR, PCoffset9
 */
uint16_t asm_lea(opl_entry* entry)
{
  // error checking, we expect 2 operands to LEA
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_lea> Error malformed LEA instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  uint16_t DR = entry->opr[0]->value;
  uint16_t PCoffset9 = entry->opr[1]->value;

  // make sure only get low 9 bits of the PCoffset9
  return (LEA << 12) | (DR << 9) | (PCoffset9 & 0x01FF);
}

/** @brief assemble NOT operation
 *
 * NOT DR, SR
 *
 * The LC-3 manual shows low 6 bits filled with 1 for NOT
 * so we will folow that, usually unused are filled with 0?
 */
uint16_t asm_not(opl_entry* entry)
{
  // error checking, we expect 2 operands to LEA
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_not> Error malformed NOT instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  uint16_t DR = entry->opr[0]->value;
  uint16_t SR = entry->opr[1]->value;

  // make sure only get low 9 bits of the PCoffset9
  return (NOT << 12) | (DR << 9) | (SR << 6) | (0x003F);
}

/** @brief assemble RTI operation
 *
 * RTI
 *
 * RTI and RET are only instructions with no operands,
 * besides the .END pseudo operation.
 */
uint16_t asm_rti(opl_entry* entry)
{
  // error checking, we expect 2 operands to RTI
  if (entry->num_opr != 0)
  {
    fprintf(stderr, "<assembler::asm_rti> Error malformed RTI instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }
  // make sure only get low 9 bits of the PCoffset9
  return (RTI << 12);
}

/** @brief assemble ST operation
 *
 * ST SR, PCoffset9
 */
uint16_t asm_st(opl_entry* entry)
{
  // error checking, we expect 2 operands to ST
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_st> Error malformed ST instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 9 bits of the PCoffset9
  return (ST << 12) | (entry->opr[0]->value << 9) | (entry->opr[1]->value & 0x01FF);
}

/** @brief assemble STI operation
 *
 * STI SR, PCoffset9
 */
uint16_t asm_sti(opl_entry* entry)
{
  // error checking, we expect 2 operands to STI
  if (entry->num_opr != 2)
  {
    fprintf(stderr, "<assembler::asm_sti> Error malformed STI instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 9 bits of the PCoffset9
  return (STI << 12) | (entry->opr[0]->value << 9) | (entry->opr[1]->value & 0x01FF);
}

/** @brief assemble STR operation
 *
 * STR SR, BaseR, offset6
 */
uint16_t asm_str(opl_entry* entry)
{
  // error checking, we expect 3 operands to STR
  if (entry->num_opr != 3)
  {
    fprintf(stderr, "<assembler::asm_str> Error malformed STR instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  uint16_t SR = entry->opr[0]->value;
  uint16_t BaseR = entry->opr[1]->value;
  uint16_t offset6 = entry->opr[2]->value;

  // make sure only get low 9 bits of the PCoffset9
  return (LDR << 12) | (SR << 9) | (BaseR << 6) | (offset6 & 0x003F);
}

/** @brief assemble .STRINGZ pseudo operation
 *
 * .STRINGZ "Hello World"
 */
uint16_t asm_stringz(opl_entry* entry)
{
  // error checking, we expect 1 operands to STRINGZ
  if (entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_fill> Error malformed .STRINGZ pseudo operation line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 8 bits of the trapvect8
  return (entry->opr[0]->value);
}

/** @brief assemble TRAP operation
 *
 * TRAP trapvect8
 */
uint16_t asm_trap(opl_entry* entry)
{
  // error checking, we expect 1 operands to TRAP
  if (entry->num_opr != 1)
  {
    fprintf(stderr, "<assembler::asm_trap> Error malformed TRAP instruction line: %05d: <%s>\n", entry->linenum, entry->line);
    exit(1);
  }

  // make sure only get low 8 bits of the trapvect8
  return (TRAP << 12) | (entry->opr[0]->value & 0x00FF);
}