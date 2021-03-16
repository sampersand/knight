#ifndef BYTECODE_H
#define BYTECODE_H

#include "value.h"

#define MAX_ARGC 4
#define OPCODE_ARGC(arg) ((arg) / 0x20)

typedef enum {
	OP_UNDEFINED = 0,
	OP_PROMPT = 0x01,
	OP_RANDOM,

	OP_EVAL = 0x20,
	OP_BLOCK,
	OP_CALL,
	OP_SYSTEM,
	OP_QUIT,
	OP_NOT,
	OP_LENGTH,
	OP_DUMP,
	OP_OUTPUT,

	OP_ADD = 0x40,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_POW,
	OP_EQL,
	OP_LTH,
	OP_GTH,
	OP_AND,
	OP_OR,
	OP_THEN,
	OP_ASSIGN,
	OP_WHILE,

	OP_IF = 0x60,
	OP_GET,

	OP_SET = 0x80,

	OP_PUSHL = 0xA0,
	OP_POP,
} opcode_t;

typedef union {
	opcode_t opcode;
	value_t value;
} bytecode_t;

typedef struct _block_t {
	unsigned refcount;
	bytecode_t *code;
} block_t;

#endif
