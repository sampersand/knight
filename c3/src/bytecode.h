#ifndef KN_BYTECODE_H
#define KN_BYTECODE_H

#include "value.h"

#define KN_MAX_ARGC 4
#define KN_OPCODE_ARGC(arg) ((arg) / 0x20)

typedef enum {
	KN_OP_UNDEFINED = 0,
	KN_OP_PROMPT = 0x01,
	KN_OP_RANDOM,

	KN_OP_EVAL = 0x20,
	KN_OP_BLOCK,
	KN_OP_CALL,
	KN_OP_SYSTEM,
	KN_OP_QUIT,
	KN_OP_NOT,
	KN_OP_LENGTH,
	KN_OP_DUMP,
	KN_OP_OUTPUT,

	KN_OP_ADD = 0x40,
	KN_OP_SUB,
	KN_OP_MUL,
	KN_OP_DIV,
	KN_OP_MOD,
	KN_OP_POW,
	KN_OP_EQL,
	KN_OP_LTH,
	KN_OP_GTH,
	KN_OP_AND,
	KN_OP_OR,
	KN_OP_THEN,
	KN_OP_ASSIGN,
	KN_OP_WHILE,

	KN_OP_IF = 0x60,
	KN_OP_GET,

	KN_OP_SET = 0x80,
} kn_opcode_t;

typedef union {
	kn_opcode_t opcode;
	kn_value_t value;
} kn_bytecode_t;

typedef struct _kn_block_t {
	unsigned refcount;
	kn_bytecode_t *code;
} kn_block_t;

#endif