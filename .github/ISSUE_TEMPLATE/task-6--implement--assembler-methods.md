---
name: 'Task 6: Implement `assembler` member methods'
about: Task 6 for Students
title: 'Task 6: Implement `assembler` member methods'
labels: enhancement
assignees: ''

---

**Description**

The `assembler.[h|c]` module contains the top level methods to perform
a two pass assembly and create and write a resulting binary file from
LC-3 assembly input file translation. In this module the `check_for_symbol()`
and `calculate_symbol_offset()` methods are stub functions that need to
be implemented and completed as described.


**Suggested Solution**

The `check_for_symbol()` needs to return the first token of the token
list if it is not an `opcode`.  You are required to reuse your
`is_keyword()` method here.  If the first token is a keyword you need
to return NULL, which indicates there is no label symbol on the line.
If the first token is not a keyword, it is assumed to be a symbol and
should thus be returned from this function.


The `calculate_symbol_offset()` is used in the second pass to determine the
relative offset from the PC for a defined symbol/label.  It needs to first
check the symbol table to fined the symbol entry and retrieve its assigned
address.  The offset is the difference from the corrent operand address to
the symbol/label address.  The result needs to be saved into the operand
value field in this method.  

**Additional Requirements**

- You are required to reuse `is_keyword()` when implementing the check for
  symbol method.
- You are required to do a simple error check in `calculate_symbol_offset()`
  that will display a message on standard error and exit if the symbole is
  not found in the symbol table.

