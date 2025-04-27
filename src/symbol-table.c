/** @file symbol-table.c
 * @brief map/dictionary type
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
#define __STDC_WANT_LIB_EXT2__ 1
#include "symbol-table.h"
#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// TABLE_SIZE will be used if constuct table and specify size 0
/// Use a prime number as default to help hash function
#define TABLE_SIZE 5011

/** @brief construct symbol_table
 *
 * Construct a new symbol table.  We dynamically allocate
 * and return a symbol_table structure and initialize
 * it.  We dynamically allocate the symbol_table entries
 * as an array of the indicated table_size for hashing.
 *
 * @param table_size The size of the hash table to create,
 *   defaults to a size of 5011 (prime number) if not specified
 *   (e.g. if given 0 as the table_size).
 *
 * @returns symbol_table* Returns pointer to newly allocated
 *   and initialized symbol table.
 */
symbol_table* st_construct(unsigned table_size)
{
  // use default if table_size not specified
  if (!table_size)
  {
    table_size = TABLE_SIZE;
  }

  // allocate the symbol table and initialize basic parameters
  symbol_table* st = (symbol_table*)malloc(sizeof(symbol_table));
  st->table_size = table_size;
  st->num_entries = 0;

  // allocate a hash table of pointers, and initialize all to NULL
  st->entries = (st_entry**)calloc(table_size, sizeof(st_entry*));
  for (int i = 0; i < table_size; i++)
  {
    st->entries[i] = NULL;
  }

  return st;
}

/** @brief destruct symbol_table
 *
 * Destruct a symbol table.  Deallocate the hash table of
 * entries, then deallocate the symbol table structure itself.
 *
 * @param st A pointer to the dynamically allocated symbol_table
 *   that should be deallocated and destroyed.
 */
void st_destruct(symbol_table* st)
{
  // first deallocate and return all symbol table entries by traversing
  // the collision linked lists
  for (int i = 0; i < st->table_size; i++)
  {
    st_entry* entry = st->entries[i];
    while (entry != NULL)
    {
      st_entry* current = entry;
      entry = entry->next;

      // we duplicate/allocate symbol for the table, so free it
      free(current->symbol);

      // and free the entry itself
      free(current);
    }
  }

  // once all entries are deallocated, can deallocate the entries array
  free(st->entries);

  // once all entries are deallocated and the entries array is free,
  // free the symbol_table itself
  free(st);
}

/** @brief insert new symbol/address pair
 *
 * Insert a new symbol with its address into the symbol_table.
 *
 * @param st A pointer to the symbol table to insert new symbol/address
 *   pair into.
 * @param symbol The character string of the symbol being inserted into the
 *   symbol table.
 * @param address The address uint16_t associated with the symbol.
 *
 * @returns st_entry* A pointer to the new entry is returned, or NULL if
 *   an error occurs on the insertion.
 */
st_entry* st_insert(symbol_table* st, const char* symbol, uint16_t address)
{
  st_entry* entry;
  unsigned hashval;

  // error check don't insert a duplicate address for a label
  entry = st_lookup(st, symbol);
  if (entry != NULL)
  {
    fprintf(stderr, "<symbol_table::insert> duplicate insertion attempted on symbol <%s> address <%d>\n", symbol, address);
    exit(1);
  }

  // otherwise safe to insert into table
  entry = (st_entry*)malloc(sizeof(st_entry));
  if (entry == NULL || (entry->symbol = strdup(symbol)) == NULL)
  {
    fprintf(stderr, "<symbol_table::insert> error allocating new st_entry or symbol\n");
    exit(1);
  }

  entry->address = address;
  hashval = st_hash(st, symbol);
  entry->next = st->entries[hashval];
  st->entries[hashval] = entry;
  st->num_entries++;

  return entry;
}

/** @brief lookup symbol/address pair
 *
 * Lookup a symbol in the symbol_table.  Returns NULL on failure if the
 * asked for symbol is not currently in the table, or
 * a pointer to the st_entry if found.
 *
 * @param st A pointer to the symbol table to lookup symbol/address
 *   pair for.
 * @param symbol The symbol, a c char array, of the symbol/address pair
 *   to be retrieved.
 *
 * @returns st_entry* A pointer to the found entry is returned, or NULL if
 *   this symbol is not in the symbol_table.
 */
st_entry* st_lookup(symbol_table* st, const char* symbol)
{
  // task 1 part 2 implementation goes here
  unsigned key = st_hash(st, symbol);
  st_entry* entry = st->entries[key];

  while (entry != NULL)
  {
    if (match(entry->symbol, symbol))
    {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

/** @brief display symbol_table
 *
 * Display contents of a symbol_table on stdout. Useful after pass 1 to generate
 * report of symbol table creation when assembling programs.
 *
 * @param st A pointer to the symbol table to display on stdout.
 */
void st_display(symbol_table* st)
{
  printf("Symbol             ADDRESS (indx)\n");
  printf("---------------------------------\n");

  char* padding = "....................";

  for (unsigned index = 0; index < st->table_size; index++)
  {
    st_entry* entry = st->entries[index];
    while (entry != NULL)
    {
      printf("%s%s0x%04X (%04d)\n", entry->symbol, padding + strlen(entry->symbol), entry->address, index);
      entry = entry->next;
    }
  }
}

/** @brief hash a symbol
 *
 * A basic hash function to hash a c character array/string.
 * Given the symbol, generate and returns its hash value index.
 *
 * @param st A pointer to the symbol so we know hash table size.
 * @param symbol The symbol, a c character array/string, to hash.
 *
 * @returns unsigned Returns an unsigned 32 bit integer value, the result
 *   of calculating the hash key for the given symbol string.
 */
unsigned st_hash(symbol_table* st, const char* symbol)
{
  // task 1 part 1 implementation goes here
  unsigned key = 0;
  for (int i = 0; symbol[i] != '\0'; i++)
  {
    key = key * 31 + (unsigned char)symbol[i];
  }
  return key % st->table_size;
}
