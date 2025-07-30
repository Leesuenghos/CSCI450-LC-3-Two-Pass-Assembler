# 🛠️ CSCI450 - LC-3 Two-Pass Assembler

This project is a Two-Pass Assembler for the LC-3 instruction set, developed as part of the CSCI450 (Computer Architecture) course. It reads LC-3 assembly language source code, resolves labels and symbols, and generates machine code in two passes.

## 📌 Features

- Two-pass assembly process:
  - **Pass 1**: Scans the program and builds a symbol table
  - **Pass 2**: Generates object code using the symbol table
- Handles pseudo-operations (`.ORIG`, `.END`, `.FILL`, etc.)
- Converts assembly instructions to binary LC-3 machine code
- Error handling for undefined labels and incorrect syntax

## 📂 Structure

├── assembler.c # Main assembler logic
├── symbol_table.c # Symbol table implementation
├── parser.c # Assembly parser
├── output.obj # Sample generated machine code
├── README.md
└── test.asm # Sample LC-3 assembly input