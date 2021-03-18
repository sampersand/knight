#ifndef BYTECODE_H
#define BYTECODE_H

#include "value.h"

#define MAX_ARGC 4
#define OPCODE_ARGC(arg) ((arg) / 0x20)

typedef enum {
	OP_UNDEFINED = 0,
	OP_PROMPT = 0x01,
	OP_RANDOM,
	OP_PUSHL, // technically has one argc, but we dont want to pop it

	OP_EVAL = 0x20,
	// OP_BLOCK, <-- doesn't exist, it's a PUSHL
	OP_CALL,
	OP_SYSTEM,
	OP_QUIT,
	OP_NOT,
	OP_LENGTH,
	OP_DUMP,
	OP_OUTPUT,
	OP_POP,

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
} opcode_t;

typedef union {
	opcode_t opcode;
	value_t value;
} bytecode_t;

typedef struct {
	unsigned length;
	union { unsigned capacity, rc; }; // capacity when parsing
	bytecode_t *code;
} block_t;

typedef struct _blockptr_t {
	block_t *block;
	unsigned ip, rc;
} blockptr_t;

block_t *block_parse(const char **);
void block_free(block_t *);
void block_clone(block_t *);

#endif
