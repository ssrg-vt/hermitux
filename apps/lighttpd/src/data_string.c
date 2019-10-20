#include "first.h"

#include "array.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static data_unset *data_string_copy(const data_unset *s) {
	data_string *src = (data_string *)s;
	data_string *ds = data_string_init();

	buffer_copy_buffer(ds->key, src->key);
	buffer_copy_buffer(ds->value, src->value);
	ds->is_index_key = src->is_index_key;
	return (data_unset *)ds;
}

static void data_string_free(data_unset *d) {
	data_string *ds = (data_string *)d;

	buffer_free(ds->key);
	buffer_free(ds->value);

	free(d);
}

static void data_string_reset(data_unset *d) {
	data_string *ds = (data_string *)d;

	/* reused array elements */
	buffer_reset(ds->key);
	buffer_reset(ds->value);
}

static int data_string_insert_dup(data_unset *dst, data_unset *src) {
	data_string *ds_dst = (data_string *)dst;
	data_string *ds_src = (data_string *)src;

	if (!buffer_is_empty(ds_dst->value)) {
		buffer_append_string_len(ds_dst->value, CONST_STR_LEN(", "));
		buffer_append_string_buffer(ds_dst->value, ds_src->value);
	} else {
		buffer_copy_buffer(ds_dst->value, ds_src->value);
	}

	src->fn->free(src);

	return 0;
}

static void data_string_print(const data_unset *d, int depth) {
	data_string *ds = (data_string *)d;
	size_t i, len;
	UNUSED(depth);

	/* empty and uninitialized strings */
	if (buffer_string_is_empty(ds->value)) {
		fputs("\"\"", stdout);
		return;
	}

	/* print out the string as is, except prepend " with backslash */
	putc('"', stdout);
	len = buffer_string_length(ds->value);
	for (i = 0; i < len; i++) {
		unsigned char c = ds->value->ptr[i];
		if (c == '"') {
			fputs("\\\"", stdout);
		} else {
			putc(c, stdout);
		}
	}
	putc('"', stdout);
}


data_string *data_string_init(void) {
	static const struct data_methods fn = {
		data_string_reset,
		data_string_copy,
		data_string_free,
		data_string_insert_dup,
		data_string_print,
	};
	data_string *ds;

	ds = calloc(1, sizeof(*ds));
	force_assert(NULL != ds);

	ds->key = buffer_init();
	ds->value = buffer_init();

	ds->type = TYPE_STRING;
	ds->fn = &fn;

	return ds;
}
