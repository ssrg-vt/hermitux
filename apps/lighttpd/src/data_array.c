#include "first.h"

#include "array.h"

#include <string.h>
#include <stdlib.h>

static data_unset *data_array_copy(const data_unset *s) {
	data_array *src = (data_array *)s;
	data_array *ds = data_array_init();

	buffer_copy_buffer(ds->key, src->key);
	array_free(ds->value);
	ds->value = array_init_array(src->value);
	ds->is_index_key = src->is_index_key;
	return (data_unset *)ds;
}

static void data_array_free(data_unset *d) {
	data_array *ds = (data_array *)d;

	buffer_free(ds->key);
	array_free(ds->value);

	free(d);
}

static void data_array_reset(data_unset *d) {
	data_array *ds = (data_array *)d;

	/* reused array elements */
	buffer_reset(ds->key);
	array_reset(ds->value);
}

static int data_array_insert_dup(data_unset *dst, data_unset *src) {
	UNUSED(dst);

	src->fn->free(src);

	return 0;
}

static void data_array_print(const data_unset *d, int depth) {
	data_array *ds = (data_array *)d;

	array_print(ds->value, depth);
}

data_array *data_array_init(void) {
	static const struct data_methods fn = {
		data_array_reset,
		data_array_copy,
		data_array_free,
		data_array_insert_dup,
		data_array_print,
	};
	data_array *ds;

	ds = calloc(1, sizeof(*ds));
	force_assert(NULL != ds);

	ds->key = buffer_init();
	ds->value = array_init();

	ds->type = TYPE_ARRAY;
	ds->fn = &fn;

	return ds;
}
