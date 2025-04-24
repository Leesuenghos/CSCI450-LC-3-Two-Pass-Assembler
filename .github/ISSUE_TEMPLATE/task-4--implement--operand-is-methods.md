---
name: 'Task 4: Implement `operand` `is_*()` helper member methods'
about: Task 4 for Students
title: 'Task 4: Implement `operand` `is_*()` helper member methods'
labels: enhancement
assignees: ''

---

**Description**

The `is_register()`, `is_string()`, `is_hex_digit()`, `is_hex_literal()`, and
`is_decimal_literal()` helper methods of the `operand` class/file need
top be implemented for task 4.

The `is_hex_digit()` method is a bit different from the other 4.  It is given
a single `char` as an input parameter, and should return `true` if the character
is a valid hex digit in a hex literal.  The valid hex digits are `0-9` but also
the `A-F` and `a-f` characters to represent values 10 through 15 in
hexadecimal.

The other 4 methods all take a `token`, which is a pointer to an array
of characters as input.  An all of them return `true` if the token is
of the indicated operand type (potentially), and `false` if not.


**Suggested Solution**

For all methods it is suggested you use a simple brute force approach.  Most of
the methods need to simply test the first character, and if it is a particular
character or type of character, the token is detected as being a possible
operand of the indicated type.  the `is_hex_literal()` function is a bit
more complicated, you need to check if the first character is `x` or `X` and the second is a hex digit, or check for `0x` `0X` variations as well.

**Additional Requirements**

