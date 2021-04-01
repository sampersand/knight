#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "../src/custom.h"
#include "../src/shared.h"
#include "../src/function.h"
#include "../src/parse.h"
#include "../src/ast.h"

struct kn_list *kn_list_new(kn_value value) {
	struct kn_list *list = xmalloc(sizeof(struct kn_list));

	list->data = value;
	list->refcount = 1;
	list->next = &kn_list_empty;

	return list;
}

struct kn_list *kn_list_clone(struct kn_list *list) {
	assert(list != NULL);
	assert(list->refcount != 0);

	if (0 < list->refcount)
		++list->refcount;

	return list;
}

void kn_list_free(struct kn_list *list) {
	assert(list != NULL);

	if (list->refcount < 0 || --list->refcount)
		return;

	assert(list->data != KN_UNDEFINED);

	kn_value_free(list->data);
	kn_list_free(list->next);

	free(list);
}

// caller needs to clone it if they want to own it.
kn_value kn_list_car(struct kn_list *list) {
	assert(list != NULL);
	assert(list->data != KN_UNDEFINED);

	return list->data;
}

// caller needs to clone it if they want to own it.
struct kn_list *kn_list_cdr(struct kn_list *list) {
	assert(list != NULL);
	assert(list != &kn_list_empty);

	return list->next;
}

bool kn_list_is_empty(const struct kn_list *list) {
	assert(list != NULL);
	assert((list->data == KN_UNDEFINED) == (list == &kn_list_empty));

	return list == &kn_list_empty;
}

void kn_list_dump(const struct kn_list *list) {
	printf("List(");

	if (kn_list_is_empty(list))
		goto end;

	kn_value_dump(list->data);
	list = list->next;

	while (!kn_list_is_empty(list)) {
		printf(", ");
		kn_value_dump(list->data);
		list = list->next;
	}

end:
	printf(")");
}

struct kn_list *kn_list_run(struct kn_list *list) {
	if (kn_list_is_empty(list))
		return list;

	struct kn_list *newlist = kn_list_new(kn_value_run(list->data));
	newlist->next = kn_list_run(list->next);

	return newlist;
}

static kn_number list_custom_to_number(void *data) {
	kn_number length = 0;
	struct kn_list *current = (struct kn_list *) data;

	while (!kn_list_is_empty(current)) {
		++length;
		current = current->next;
	}

	return length;
}


static kn_boolean list_custom_to_boolean(void *data) {
	return !kn_list_is_empty((struct kn_list *) data);
}

static struct kn_string *list_custom_to_string(void *data) {
	return kn_string_new(strdup("todo"), 4);
	// return !kn_list_is_empty(LST(data));
}

static kn_value list_custom_run(void *data) {
	return kn_value_new_custom(
		kn_list_run((struct kn_list *) data),
		&kn_list_vtable
	);
}

const struct kn_custom_vtable kn_list_vtable = {
	.clone = (void *(*) (void *)) kn_list_clone,
	.free = (void (*) (void *)) kn_list_free,
	.dump = (void (*) (void *)) kn_list_dump,
	.run = list_custom_run,
	.to_number = list_custom_to_number,
	.to_boolean = list_custom_to_boolean,
	.to_string = list_custom_to_string
};

KN_FUNCTION_DECLARE(car, 1, 'A') {
	kn_value run = kn_value_run(args[0]);
	struct kn_list *list = (struct kn_list *) kn_value_as_custom(run);

	kn_value ret = kn_value_clone(kn_list_car(list));
	kn_value_free(run);

	// kn_value_dump(ret);
	// exit(0);
	return ret;
}

KN_FUNCTION_DECLARE(cdr, 1, 'D') {
	kn_value run = kn_value_run(args[0]);
	struct kn_list *list = (struct kn_list *) kn_value_as_custom(run);
	struct kn_list *ret = kn_list_clone(kn_list_cdr(list));

	kn_value_free(run);

	return kn_value_new_custom(ret, &kn_list_vtable);
}

KN_FUNCTION_DECLARE(cons, 2, 'C') {
	kn_value run = kn_value_run(args[0]);
	struct kn_list *list = (struct kn_list *) kn_value_as_custom(run);
	struct kn_list *ret = kn_list_clone(kn_list_cdr(list));

	kn_value_free(run);

	return kn_value_new_custom(ret, &kn_list_vtable);
}

kn_value kn_parse_extension(const char **stream) {
	static unsigned depth;

	const struct kn_function *function;

	switch (*(*stream)++) {
	case '[': {
		unsigned current_depth = depth++;
		struct kn_list *head = NULL, *curr;
		kn_value parsed;

		while ((parsed = kn_parse(stream)) != KN_UNDEFINED) {
			if (head == NULL)
				head = curr = kn_list_new(parsed);
			else
				curr = curr->next = kn_list_new(parsed);
		}

		if (current_depth != depth) 
			die("missing closing 'X]'");

		if (head == NULL)
			head = &kn_list_empty;

		return kn_value_new_custom(head, &kn_list_vtable);
	}

	case ']':
		if (!depth--)
			die("unexpected `X]`");
		return KN_UNDEFINED;
	case 'A': 
		function = &kn_fn_car;
		goto parse_function;
	case 'D':
		function = &kn_fn_cdr;
		goto parse_function;
	case 'C':
		function = &kn_fn_cons;

	parse_function: {
		struct kn_ast *ast = kn_ast_alloc(function->arity);

		ast->func = function;
		ast->refcount = 1;

		for (size_t i = 0; i < function->arity; ++i) {
			if ((ast->args[i] = kn_parse(stream)) == KN_UNDEFINED) {
				die("unable to parse argume?nt %d for function '%c'",
					i, function->name);
			}
		}

		return kn_value_new_ast(ast);
	}

	default:
		die("unknown extension character '%c'", (*stream)[-1]);
	}
}



