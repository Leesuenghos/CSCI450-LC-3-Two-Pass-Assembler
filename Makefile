
# source files in this project (for beautification)
PROJECT_NAME=assg06
assg_src = assembler.c \
		tokenizer.c \
		symbol-table.c \
		opcode.c \
		operand.c \
		operation-list.c

test_src = ${PROJECT_NAME}-tests.cpp \
	  ${assg_src}

prog_src  = lc3asm.c \
	  ${assg_src}

# template files, list all files that define template classes
# or functions and should not be compiled separately (template
# is included where used)
template-files =

# assignment description documentation
assg_doc = ${PROJECT_NAME}.pdf

# common targets and variables used for all assignments/projects
include include/Makefile.inc

# assignment header file specific dependencies
${OBJ_DIR}/assembler.o: ${INC_DIR}/assembler.h ${INC_DIR}/symbol-table.h ${INC_DIR}/operation-list.h ${SRC_DIR}/assembler.c
${OBJ_DIR}/tokenizer.o: ${INC_DIR}/tokenizer.h ${SRC_DIR}/tokenizer.c
${OBJ_DIR}/symbol-table.o: ${INC_DIR}/symbol-table.h ${SRC_DIR}/symbol-table.c
${OBJ_DIR}/opcode.o: ${INC_DIR}/opcode.h ${SRC_DIR}/opcode.c
${OBJ_DIR}/operand.o: ${INC_DIR}/operand.h ${SRC_DIR}/operand.c
${OBJ_DIR}/operation-list.o: ${INC_DIR}/operation-list.h ${SRC_DIR}/operation-list.c
${OBJ_DIR}/${PROJECT_NAME}-tests.o: ${INC_DIR}/assembler.h ${INC_DIR}/symbol-table.h ${INC_DIR}/operation-list.h ${SRC_DIR}/${PROJECT_NAME}-tests.cpp
${OBJ_DIR}/${PROJECT_NAME}-sim.o: ${INC_DIR}/assembler.h ${INC_DIR}/symbol-table.h ${INC_DIR}/operation-list.h ${SRC_DIR}/${PROJECT_NAME}-sim.c

