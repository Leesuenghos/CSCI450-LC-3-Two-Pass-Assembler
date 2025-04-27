/** @file assg06-tests.cpp
 * @brief Unit tests for OS assg 05
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2024
 * @note   ide:  VS Code Editor / IDE ; g++ 8.2.0 / GNU Make 4.2.1
 *
 * Unit tests for assignment 06, building an LC-3 assembler.
 */
#include "catch.hpp"
#include <iostream>
#include <string>

using namespace std;

#define TEST
#include "assembler.h"
#include "opcode.h"
#include "operand.h"
#include "operation-list.h"
#include "symbol-table.h"
#include "tokenizer.h"

#define task1
#undef task2
#undef task3
#undef task4
#undef task5
#undef task6

/**
 * @brief Task 1: test symbol-table functions
 */
#ifdef task1
TEST_CASE("Task 1: test symbol-table functions", "[task1]")
{
  // create a symbol table with 11 hash slots
  symbol_table* st = st_construct(11);

  // test hash function
  const char* s1 = "LOOP1";
  CHECK(st_hash(st, s1) == 8);
  const char* s2 = "MYINT";
  CHECK(st_hash(st, s2) == 3);
  const char* s3 = "MESSAGE";
  CHECK(st_hash(st, s3) == 9);
  const char* s4 = "ERROR";
  CHECK(st_hash(st, s4) == 7);
  const char* s5 = "ERRO2";
  CHECK(st_hash(st, s5) == 8);
  const char* s6 = "V1";
  CHECK(st_hash(st, s6) == 9);
  const char* s7 = "V2";
  CHECK(st_hash(st, s7) == 10);
  const char* s8 = "V3";
  CHECK(st_hash(st, s8) == 0);

  // test match
  const char* s8dup = "V3";
  const char* s1dup = "LOOP1";
  CHECK(match(s1dup, s1));
  CHECK(match(s8, s8dup));

  CHECK_FALSE(match(s4, s5));
  CHECK_FALSE(match(s6, s7));

  // test strdup
  char* s2dup = strdup(s2);
  CHECK(match(s2, s2dup));

  // test basic insert and lookup
  st_insert(st, s1, 0x3000);
  CHECK(st->num_entries == 1);

  st_entry* entry;
  entry = st_lookup(st, s1);
  CHECK(entry != NULL);
  CHECK(entry->address == 0x3000);

  // if not entered yet, should fail lookup
  entry = st_lookup(st, s2);
  CHECK(entry == NULL);

  // insert a duplicate should cause error
  // st_insert(st, s1, 0x3001); // we are exiting currently, can't test

  // insert rest, including some colided hashes
  st_insert(st, s2, 0x1234);
  st_insert(st, s3, 0xDEAD);
  st_insert(st, s4, 0xBEEF);
  st_insert(st, s5, 0xFFFF);
  st_insert(st, s6, 0x0000);
  st_insert(st, s7, 0xABCD);
  st_insert(st, s8, 0x9876);

  CHECK(st->num_entries == 8);

  // test lookup implementation more thoroughly.
  entry = st_lookup(st, s2);
  CHECK(match(entry->symbol, s2));
  CHECK(entry->address == 0x1234);

  entry = st_lookup(st, s3);
  CHECK(match(entry->symbol, s3));
  CHECK(entry->address == 0xDEAD);

  entry = st_lookup(st, s4);
  CHECK(match(entry->symbol, s4));
  CHECK(entry->address == 0xBEEF);

  entry = st_lookup(st, s5);
  CHECK(match(entry->symbol, s5));
  CHECK(entry->address == 0xFFFF);

  entry = st_lookup(st, s6);
  CHECK(match(entry->symbol, s6));
  CHECK(entry->address == 0x0000);

  entry = st_lookup(st, s7);
  CHECK(match(entry->symbol, s7));
  CHECK(entry->address == 0xABCD);

  entry = st_lookup(st, s8);
  CHECK(match(entry->symbol, s8));
  CHECK(entry->address == 0x9876);

  // test some more failed lookups
  const char* s9 = "LOOP2";
  entry = st_lookup(st, s9);
  CHECK(entry == NULL);

  const char* s10 = "ERROR2";
  entry = st_lookup(st, s10);
  CHECK(entry == NULL);

  st_insert(st, s9, 0x4567);
  entry = st_lookup(st, s9);
  CHECK(st->num_entries == 9);
  CHECK(entry->address == 0x4567);

  // will cause current symbol table to display in tests, so may
  // not want to keep this uncommented
  cout << "Test Task 1 Final Symbol Table" << endl;
  st_display(st);

  // destruct the symbol table so can at least check if we segv
  st_destruct(st);
}
#endif // task1

/**
 * @brief Task 2: test tokenizer functions
 */
#ifdef task2
TEST_CASE("Task 2: test tokenizer functions", "[task2]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  tokens* tks;

  // first operation line of this file has 2 tokens and first is .ORIG
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 2);
  CHECK(match(tks->token[0], ".ORIG"));
  CHECK(match(tks->token[1], "0x3050"));
  tokens_destruct(tks);

  // second operation line of this file has 3 tokens
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 3);
  CHECK(match(tks->token[0], "LD"));
  CHECK(match(tks->token[1], "R1"));
  CHECK(match(tks->token[2], "SIX"));
  tokens_destruct(tks);

  // third operation line of this file also has 3 tokens
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 3);
  CHECK(match(tks->token[0], "LD"));
  CHECK(match(tks->token[1], "R2"));
  CHECK(match(tks->token[2], "NUMBER"));
  tokens_destruct(tks);

  // fourth operation line of this file  has 4 tokens
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 4);
  CHECK(match(tks->token[0], "AND"));
  CHECK(match(tks->token[1], "R3"));
  CHECK(match(tks->token[2], "R3"));
  CHECK(match(tks->token[3], "#0"));
  tokens_destruct(tks);

  // fifth operation line has to skip some blank and
  // comment lines, and has 5 tokens
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 5);
  CHECK(match(tks->token[0], "AGAIN"));
  CHECK(match(tks->token[1], "ADD"));
  CHECK(match(tks->token[2], "R3"));
  CHECK(match(tks->token[3], "R3"));
  CHECK(match(tks->token[4], "R2"));
  tokens_destruct(tks);

  // skipping over lines 6-10 to test strtokquote implementation
  tks = tk_next_line(tk); // line 6
  tokens_destruct(tks);
  tks = tk_next_line(tk); // line 7
  tokens_destruct(tks);
  tks = tk_next_line(tk); // line 8
  tokens_destruct(tks);
  tks = tk_next_line(tk); // line 9
  tokens_destruct(tks);
  tks = tk_next_line(tk); // line 10
  tokens_destruct(tks);

  // line 11 has a  STRINGZ pseudoop with a string literal token
  tks = tk_next_line(tk);
  CHECK(tks->num_tokens == 3);
  CHECK(match(tks->token[0], "MSG"));
  CHECK(match(tks->token[1], ".STRINGZ"));
  CHECK(match(tks->token[2], "\"Error Message\""));
  tokens_destruct(tks);
}
#endif // task2

/**
 * @brief Task 3: test opcode functions
 */
#ifdef task3
TEST_CASE("Task 3: test opcode functions", "[task3]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  tokens* tks;
  opcode* opc;

  // 1) first line is .ORIG pseudoopcode
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == ORIG);
  tokens_destruct(tks);

  // 2) LD
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == LD);
  tokens_destruct(tks);

  // 3) LD
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == LD);
  tokens_destruct(tks);

  // 4) AND
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == AND);
  tokens_destruct(tks);

  // 5) ADD
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == ADD);
  tokens_destruct(tks);

  // 6) ADD
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == ADD);
  tokens_destruct(tks);

  // 7) BRp
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == BR);
  CHECK(opc->flags == 0x1); // NP flag
  tokens_destruct(tks);

  // 8) TRAP
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == TRAP);
  tokens_destruct(tks);

  // 9) .BLKW
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == BLKW);
  tokens_destruct(tks);

  // 10) .FILL
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == FILL);
  tokens_destruct(tks);

  // 11) .STRINGZ
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == STRINGZ);
  tokens_destruct(tks);

  // 12) .END
  tks = tk_next_line(tk);
  opc = extract_opcode(tks);
  CHECK(opc->opc == END);
  tokens_destruct(tks);

  // TODO: should add in tests of the jsr/jsrr and jmp/ret variant parsing
  // TODO: and really should also test more BR flag parsing cases
}
#endif // task3

/**
 * @brief Task 4: test opcode functions
 */
#ifdef task4
TEST_CASE("Task 4: test operand `is_*()` functions", "[task4]")
{
  // test is_register()
  CHECK(is_register("R0"));
  CHECK(is_register("R5"));
  CHECK(is_register("R7"));
  CHECK_FALSE(is_register("r7"));
  CHECK_FALSE(is_register("\"Error string\""));
  CHECK_FALSE(is_register("\"Output Message\""));
  CHECK_FALSE(is_register("0xaa"));
  CHECK_FALSE(is_register("0X1234"));
  CHECK_FALSE(is_register("Xaa"));
  CHECK_FALSE(is_register("xBEEF"));
  CHECK_FALSE(is_register("#-1"));
  CHECK_FALSE(is_register("#0999"));
  CHECK_FALSE(is_register("#255"));

  // test is_string()
  CHECK_FALSE(is_string("R0"));
  CHECK_FALSE(is_string("R5"));
  CHECK_FALSE(is_string("R7"));
  CHECK_FALSE(is_string("r7"));
  CHECK(is_string("\"Error string\""));
  CHECK(is_string("\"Output Message\""));
  CHECK_FALSE(is_string("0xaa"));
  CHECK_FALSE(is_string("0X1234"));
  CHECK_FALSE(is_string("Xaa"));
  CHECK_FALSE(is_string("xBEEF"));
  CHECK_FALSE(is_string("#-1"));
  CHECK_FALSE(is_string("#0999"));
  CHECK_FALSE(is_string("#255"));

  // test is_hex_digit
  CHECK(is_hex_digit('0'));
  CHECK(is_hex_digit('5'));
  CHECK(is_hex_digit('9'));
  CHECK(is_hex_digit('A'));
  CHECK(is_hex_digit('a'));
  CHECK(is_hex_digit('B'));
  CHECK(is_hex_digit('b'));
  CHECK(is_hex_digit('C'));
  CHECK(is_hex_digit('c'));
  CHECK(is_hex_digit('D'));
  CHECK(is_hex_digit('d'));
  CHECK(is_hex_digit('E'));
  CHECK(is_hex_digit('e'));
  CHECK(is_hex_digit('F'));
  CHECK(is_hex_digit('f'));
  CHECK_FALSE(is_hex_digit('G'));
  CHECK_FALSE(is_hex_digit('X'));
  CHECK_FALSE(is_hex_digit('x'));
  CHECK_FALSE(is_hex_digit('#'));

  // test is_hex_literal()
  CHECK_FALSE(is_hex_literal("R0"));
  CHECK_FALSE(is_hex_literal("R5"));
  CHECK_FALSE(is_hex_literal("R7"));
  CHECK_FALSE(is_hex_literal("r7"));
  CHECK_FALSE(is_hex_literal("\"Error string\""));
  CHECK_FALSE(is_hex_literal("\"Output Message\""));
  CHECK(is_hex_literal("0xaa"));
  CHECK(is_hex_literal("0X1234"));
  CHECK(is_hex_literal("Xaa"));
  CHECK(is_hex_literal("xBEEF"));
  CHECK_FALSE(is_hex_literal("#-1"));
  CHECK_FALSE(is_hex_literal("#0999"));
  CHECK_FALSE(is_hex_literal("#255"));

  // test is_decimal_literal()
  CHECK_FALSE(is_decimal_literal("R0"));
  CHECK_FALSE(is_decimal_literal("R5"));
  CHECK_FALSE(is_decimal_literal("R7"));
  CHECK_FALSE(is_decimal_literal("r7"));
  CHECK_FALSE(is_decimal_literal("\"Error string\""));
  CHECK_FALSE(is_decimal_literal("\"Output Message\""));
  // CHECK_FALSE(is_decimal_literal("0xaa"));  // these will be tested as hex literals before testing for decimal
  // CHECK_FALSE(is_decimal_literal("0X1234"));
  CHECK_FALSE(is_decimal_literal("Xaa"));
  CHECK_FALSE(is_decimal_literal("xBEEF"));
  CHECK(is_decimal_literal("#-1"));
  CHECK(is_decimal_literal("#0999"));
  CHECK(is_decimal_literal("#255"));
}

TEST_CASE("Task 4: test operand_extract functions", "[task4]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  tokens* tks;
  operand* opr;

  // 1) first line is .ORIG 0x3050 pseudoopcode
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 0x3050);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 2) LD R1, SIX
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 1);
  opr_destruct(opr);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == SYMBOL);
  CHECK(match(opr->svalue, "SIX"));
  opr_destruct(opr);
  tokens_destruct(tks);

  // 3) LD R2, NUMBER
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 2);
  opr_destruct(opr);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == SYMBOL);
  CHECK(match(opr->svalue, "NUMBER"));
  opr_destruct(opr);
  tokens_destruct(tks);

  // 4) AND R3, R3, #0
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 3);
  opr_destruct(opr);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 3);
  opr_destruct(opr);
  opr = extract_operand(tks, 3);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 0);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 5) ADD R3, R3, R2
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 3);
  opr_destruct(opr);
  opr = extract_operand(tks, 3);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 3);
  opr_destruct(opr);
  opr = extract_operand(tks, 4);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 2);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 6) ADD R1, R1, #-1
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 1);
  opr_destruct(opr);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == REGISTER);
  CHECK(opr->value == 1);
  opr_destruct(opr);
  opr = extract_operand(tks, 3);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 0xFFFF); // -1 is unsigned 0xFFFF
  opr_destruct(opr);
  tokens_destruct(tks);

  // 7) BRp AGAIN
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == SYMBOL);
  CHECK(match(opr->svalue, "AGAIN"));
  opr_destruct(opr);
  tokens_destruct(tks);

  // 8) TRAP 0x25
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 1);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 0x25);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 9) NUMBER .BLKW 5
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 5);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 10) SIX .FILL 0x0006
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == NUMERIC);
  CHECK(opr->value == 0x0006);
  opr_destruct(opr);
  tokens_destruct(tks);

  // 11) MSG .STRINGZ "Error Message"
  tks = tk_next_line(tk);
  opr = extract_operand(tks, 2);
  CHECK(opr->opr == STRING);
  CHECK(match(opr->svalue, "Error Message")); // NOTE: extract_string removes the "" delimeters
  opr_destruct(opr);
  tokens_destruct(tks);
}
#endif // task4

/**
 * @brief Task 5: test operation_list functions
 */
#ifdef task5
TEST_CASE("Task 5: test operaion_list functions", "[task5]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  symbol_table* st = st_construct(0);

  operation_list* opl;

  opl = pass_one(tk, st);
  opl_entry* entry = opl_begin(opl);

  // .ORIG 0x3050 file line 5
  CHECK(entry->linenum == 5);
  CHECK(entry->opc->opc == ORIG);
  CHECK(entry->num_opr == 1);
  CHECK(entry->address == 0x3050);
  CHECK(entry->size == 0);
  entry = opl_next(opl);

  // LD R1, SIX file line 6
  CHECK(entry->linenum == 6);
  CHECK(entry->opc->opc == LD);
  CHECK(entry->num_opr == 2);
  CHECK(entry->address == 0x3050);
  CHECK(entry->size == 1);
  entry = opl_next(opl);

  // LD R2, NUMBER file line 7
  CHECK(entry->linenum == 7);
  CHECK(entry->opc->opc == LD);
  CHECK(entry->num_opr == 2);
  CHECK(entry->address == 0x3051);
  CHECK(entry->size == 1);
  entry = opl_next(opl);

  // AND R3, R3, #0 file line 8
  CHECK(entry->linenum == 8);
  CHECK(entry->opc->opc == AND);
  CHECK(entry->num_opr == 3);
  CHECK(entry->address == 0x3052);
  CHECK(entry->size == 1);
  entry = opl_next(opl);

  // AGAIN ADD R3, R3, R2 file line 13
  CHECK(entry->linenum == 13);
  // CHECK(match(entry->label, "AGAIN"));
  CHECK(entry->opc->opc == ADD);
  CHECK(entry->num_opr == 3);
  CHECK(entry->address == 0x3053);
  CHECK(entry->size == 1);
  entry = opl_next(opl);
}
#endif // task 5

/**
 * @brief Task 6: test assembler helper functions
 */
#ifdef task6
TEST_CASE("Task 6: test assembler `check_for_symbol()` function", "[task6]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  tokens* tks;

  // 1) first line is .ORIG 0x3050 pseudoopcode
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 2) LD R1, SIX
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 3) LD R2, NUMBER
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 4) AND R3, R3, #0
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 5) AGAIN ADD R3, R3, R2
  tks = tk_next_line(tk);
  CHECK(match(check_for_symbol(tks), "AGAIN"));
  tokens_destruct(tks);

  // 6) ADD R1, R1, #-1
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 7) BRp AGAIN
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 8) TRAP 0x25
  tks = tk_next_line(tk);
  CHECK(check_for_symbol(tks) == NULL);
  tokens_destruct(tks);

  // 9) NUMBER .BLKW 5
  tks = tk_next_line(tk);
  CHECK(match(check_for_symbol(tks), "NUMBER"));
  tokens_destruct(tks);

  // 10) SIX .FILL 0x0006
  tks = tk_next_line(tk);
  CHECK(match(check_for_symbol(tks), "SIX"));
  tokens_destruct(tks);

  // 11) MSG .STRINGZ "Error Message"
  tks = tk_next_line(tk);
  CHECK(match(check_for_symbol(tks), "MSG"));
  tokens_destruct(tks);
}

TEST_CASE("Task 6: test assembler `calculate_symbol_offset()` function", "[task6]")
{
  const char* asmfile = "progs/multiply-by-six.asm";
  tokenizer* tk = tk_construct(asmfile);
  symbol_table* st = st_construct(0);

  operation_list* opl;
  operand* opr;

  opl = pass_one(tk, st);
  opl_entry* entry = opl_begin(opl);

  // .ORIG 0x3050 line 1 skip, no symbol
  entry = opl_next(opl);

  // LD R1, SIX line 2, offset is +11 from PC+1
  opr = entry->opr[1];
  calculate_symbol_offset(opr, entry->address, st);
  CHECK(opr->value == 11);
  entry = opl_next(opl);

  // LD R2, NUMBER line 3, offset is +5 from PC+1
  opr = entry->opr[1];
  calculate_symbol_offset(opr, entry->address, st);
  CHECK(opr->value == 5);
  entry = opl_next(opl);

  // AND R3, R3, #0 line 4 skip, no symbol
  entry = opl_next(opl);
  // AGAIN ADD R3, R3, R2 line 5 skip, no symbol
  entry = opl_next(opl);
  // ADD R1, R1, #-1 line 6 skip, no symbol
  entry = opl_next(opl);

  // BRp AGAIN line 7, offset is -3 from PC+1
  opr = entry->opr[0];
  calculate_symbol_offset(opr, entry->address, st);
  CHECK(opr->value == 65533); // -3 unsigned is 65533
  entry = opl_next(opl);
}
#endif // task 6