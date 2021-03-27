#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "../src/custom.h"
#include "../src/shared.h"
#include "../src/function.h"
#include "../src/parse.h"

struct kn_custom kn_list_empty_custom = { 
	.data = (void *) &kn_list_empty,
	.vtable = &kn_list_vtable
};

kn_value kn_list_empty_value = ((uint64_t) &kn_list_empty_custom) | 6;

struct kn_list kn_list_empty = {
	.data = KN_UNDEFINED,
	.next = kn_list_empty_value,
	.refcount = -1,
};

struct kn_list *kn_list_new(kn_value value) {
	struct kn_list *list = xmalloc(sizeof(struct kn_list));

	list->data = value;
	list->refcount = 1;
	list->next = kn_list_empty_value;

	return list;
}

struct kn_list *kn_list_clone(struct kn_list *list) {
	assert(list != NULL);
	if (list->refcount > 0)
		++list->refcount;

	return list;
}

void kn_list_free(struct kn_list *list) {
	assert(list != NULL);
	if (list->refcount < 0 || --list->refcount)
		return;

	assert(list->data != KN_UNDEFINED);

	kn_value_free(list->data);
	kn_value_free(list->next);
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


#define LST(custom) ((struct kn_list *) (custom)->data)

static struct kn_custom *list_custom_clone(struct kn_custom *custom) {
	(void) kn_list_clone(LST(custom));

	return custom;
}

static void list_custom_free(struct kn_custom *custom) {
	// TODO: this can memleak for structs that contain themselves.
	int freecustom = LST(custom)->refcount == 1;

	kn_list_free(LST(custom));

	if (freecustom)
		free(custom);
}

static void list_custom_dump(struct kn_custom *custom) {
	struct kn_list *list = LST(custom);

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

static kn_number list_custom_to_number(struct kn_custom *custom) {
	kn_number length = 0;
	struct kn_list *current = LST(custom);

	while (!kn_list_is_empty(current)) {
		++length;
		current = current->next;
	}

	return length;
}


static kn_boolean list_custom_to_boolean(struct kn_custom *custom) {
	return !kn_list_is_empty(LST(custom));
}

static struct kn_string *list_custom_to_string(struct kn_custom *custom) {
	return kn_string_new(strdup("todo"), 4);
	// return !kn_list_is_empty(LST(custom));
}

const struct kn_custom_vtable kn_list_vtable = {
	.clone = list_custom_clone,
	.free = list_custom_free,
	.dump = list_custom_dump,
	.run = NULL, // simply clone it
	.to_number = list_custom_to_number,
	.to_boolean = list_custom_to_boolean,
	.to_string = list_custom_to_string
};

KN_FUNCTION_DECLARE(car, 'A', 1) {
	kn_value run = kn_value_run(args[0]);
	struct kn_list *list = LST(kn_value_as_custom(run));

	kn_value ret = kn_list_car(list->data);
	kn_vaule_free(run);

	return ret;
}

KN_FUNCTION_DECLARE(cdr, 'D', 1) {
	kn_value run = kn_value_run(args[0]);
	struct kn_list *list = LST(kn_value_as_custom(run));

	kn_value ret = kn_list_cdr(list->next);
	kn_vaule_free(run);

	return ret;
}

kn_value kn_parse_extension(const char **stream) {
	static unsigned depth;

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
			return kn_value_new_custom(&kn_list_empty_custom);

		struct kn_custom *custom = xmalloc(sizeof(struct kn_custom));
		custom->data = head;
		custom->vtable = &kn_list_vtable;

		return kn_value_new_custom(custom);
	}

	case ']':
		if (!depth--)
			die("unexpected `X]`");
		return KN_UNDEFINED;
	case 'A':
		return KN_UNDEFINED; 

	default:
		die("unknown extension character '%c'", (*stream)[-1]);
	}
}



