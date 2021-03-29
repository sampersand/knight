#include "../src/custom.h"
#include "../src/function.h"

struct hello_world {
	char *name, *greeting;
	unsigned rc;
};
#define HW(custom) ((struct hello_world *) (custom)->data)

struct kn_custom *hw_clone(struct kn_custom *custom) {
	++((struct hello_world *) (custom)->data)->rc;

	return custom;
}

void hw_free(struct kn_custom *custom) {
	if (--HW(custom)->rc) return;
	free(HW(custom)->name);
	free(HW(custom)->greeting);
	free(HW(custom));
	free(custom);
}

void hw_dump(struct kn_custom *custom) {
	printf("HelloWorld(\"%s\", \"%s\")", HW(custom)->greeting, HW(custom)->name);
}

struct kn_string *hw_string(struct kn_custom *custom) {
	struct kn_string *string = kn_string_alloc(
		strlen(HW(custom)->name) + strlen(HW(custom)->greeting) + 3
	);

	strcpy(kn_string_deref(string), HW(custom)->greeting);
	strcat(kn_string_deref(string), ", ");
	strcat(kn_string_deref(string), HW(custom)->name);
	strcat(kn_string_deref(string), "!");

	return string;
}

kn_value_t hw_run(struct kn_custom *custom) {
	return kn_value_new_string(hw_string(custom));
}

static struct kn_custom_vtable_t hw_vtable = {
	.clone = hw_clone,
	.free = hw_free,
	.dump = hw_dump,
	.run = hw_run,
	.to_number = NULL,
	.to_boolean = NULL,
	.to_string = hw_string
};

KN_FUNCTION_DECLARE(extension, 1, 'X') {
	struct kn_custom *custom = xmalloc(sizeof(struct kn_custom));
	struct hello_world *data = xmalloc(sizeof(struct hello_world));
	struct kn_string *name = kn_value_to_string(args[0]);

	data->name = strdup(kn_string_deref(name));
	kn_string_free(name);
	data->greeting = strdup("Hello");
	data->rc = 1;

	custom->data = data;
	custom->vtable = &hw_vtable;

	return kn_value_new_custom(custom);
}
