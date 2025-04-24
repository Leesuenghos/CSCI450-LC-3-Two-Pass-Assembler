/** @file symbol-table.h
 * @brief symbol table using hash map
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2025
 * @note   ide:  gcc 13.3.0 / GNU Make 4.3 / VSCode 1.99
 *
 * Implementation of a basic map or dictionary type to support
 * creating a symbol table efficiently for the lc3 assembler
 *
 * Stolen from K&R C programming language, a simple implementation
 * of an open hashing scheme.  In open hashing, we use an array
 * of pointers.  Collisions to the same hash key are resolved by
 * building a linked list of symbol table entries that has to the
 * same key/index in the hash table.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/// st_entry holds symbol table entries in the open hashing
/// table.  collisions are resolved as linked lists on the
/// collided index
typedef struct st_entry
{
  char* symbol;
  uint16_t address;
  struct st_entry* next;
} st_entry;

/// We can create and manage multiple symbol_tables if needed.
/// This is really an OO design in plain C, the symbol_table
/// structure contains all needed information about a symbol table, and
/// needs to be passed in as first parameter for all functions that
/// manipulate a symbol_table
typedef struct symbol_table
{
  /// The size of the hash table
  unsigned table_size;

  /// current number of entries being held in the table
  unsigned num_entries;

  /// An array of size table_size.  Each entry is initially
  /// NULL, and if symbols collide to same hash index,
  /// collisions are managed as linked lists of the coliding
  /// symbols.
  st_entry** entries;
} symbol_table;

// If we are creating tests, make all declarations extern C so can
// work with catch2 C++ framework
#ifdef TEST
extern "C" {
#endif

symbol_table* st_construct(unsigned table_size);
void st_destruct(symbol_table* st);
st_entry* st_insert(symbol_table* st, const char* symbol, uint16_t address);
st_entry* st_lookup(symbol_table* st, const char* symbol);
void st_display(symbol_table* st);
unsigned st_hash(symbol_table* st, const char* symbol);

#ifdef TEST
} // end extern C for C++ test runner
#endif

#endif // SYMBOL_TABLE_H
