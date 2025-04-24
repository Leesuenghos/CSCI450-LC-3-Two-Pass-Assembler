---
name: 'Task 1: Implement `symbol_table` member methods'
about: Task 1 for Students
title: 'Task 1: Implement `symbol_table` member methods'
labels: enhancement, good first issue
assignees: ''

---

**Description**

Implement the stubbed out `st_hash()` and `st_lookup()` functions in the
`symbol_table.[h|c]` file.  The function prototypes have already been given
in `symbol_table.h` and stub functions that return 0/NULL have been declared
in `symbol_table.c`, you only need to complete their implementations.


**Suggested Solution**

The `st_hash()` function needs to do the following:

1. Declare a local variable (for example named `key`) and initialize it to 0.
   This will hold the hash key while we calculate it to be returned.  This should
   be of type `unsigned` just like the return type of the `st_hash()` function.
2. You need a loop that loops over each character of the `symbol` that is
   passed in as the second paramter.  `symbol` is a pointer to a null
   terminated array of characters.  So you should loop until the current
   character is `\0`.
   - First multiply the current `key` by 31.  Initialy when `key` is 0, 
     multiplying by 31 will still be 0, but then when you add in the
     ascii value of the first character in the `symbol` it will be non zero.
     After that, multiplying by 31 before adding in each character will increase
     the key.
   - After the multiply, add in the ascii value of the current character into
     the key.
3. The resulting returned key needs to be in the range from 0 to `table_size - 1`.
   You are passed in the `symbol_table` as the first parameter in this function,
   named `st`.  You can access the size of the symbol table you are hashing by
   accessing the `st->table_size` member, this holds the allocated size of the
   `entries` hash table array being used in this `symbol_table`.  You need to
   use modulus `%` arithmetic to rescale the `key` into the correct range from
   0 to `table_size - 1` before returning it.

The `st_lookup()` function should be implemented as follows:

1. Use your `st_hash()` function to determine the hash key/index for the
   given `symbol` that is to be looked up.
2. The `st->entries[key]` will be a pointer to a linked list of `st_entry`
   items.  So you need to search each entry on this list to see if you can find 
   the asked for symbol or not.  Start with the entry at the front of the
   linked list, for example:
```
st_entry* entry = st->entries[key];
```
3. Create a loop that executes as long as the pointer to the entry is not NULL.
   - Check if the `entry->symbol` is the symbol that you are searching for.  Use
     the `match()` function declared in the `assembler.[h|c]` file for this.
   - If  they symbol matches the entry, just return the found `st_entry` pointer.
   - otherwise move to the next entry in the linked list `entry = entry->next`;
4. If the symbol is not found in the loop search, it means the search failed.  
   So this function should return `NULL` as the result after the loop to indicate
   a failed lookup.

**Additional Requirements**

