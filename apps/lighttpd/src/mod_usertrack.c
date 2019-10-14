#include "first.h"

#include "base.h"
#include "log.h"
#include "buffer.h"
#include "rand.h"
#include "http_header.h"

#include "plugin.h"

#include <stdlib.h>
#include <string.h>

#include "md5.h"

/* plugin config for all request/connections */

typedef struct {
	buffer *cookie_name;
	buffer *cookie_attrs;
	buffer *cookie_domain;
	unsigned int cookie_max_age;
} plugin_config;

typedef struct {
	PLUGIN_DATA;

	plugin_config **config_storage;

	plugin_config conf;
} plugin_data;

/* init the plugin data */
INIT_FUNC(mod_usertrack_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));

	return p;
}

/* detroy the plugin data */
FREE_FUNC(mod_usertrack_free) {
	plugin_data *p = p_d;

	UNUSED(srv);

	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (NULL == s) continue;

			buffer_free(s->cookie_name);
			buffer_free(s->cookie_attrs);
			buffer_free(s->cookie_domain);

			free(s);
		}
		free(p->config_storage);
	}

	free(p);

	return HANDLER_GO_ON;
}

/* handle plugin config and check values */

SETDEFAULTS_FUNC(mod_usertrack_set_defaults) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ "usertrack.cookie-name",       NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 0 */
		{ "usertrack.cookie-max-age",    NULL, T_CONFIG_INT, T_CONFIG_SCOPE_CONNECTION },          /* 1 */
		{ "usertrack.cookie-domain",     NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 2 */
		{ "usertrack.cookie-attrs",      NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 3 */

		{ NULL,                          NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->cookie_name    = buffer_init();
		s->cookie_attrs   = buffer_init();
		s->cookie_domain  = buffer_init();
		s->cookie_max_age = 0;

		cv[0].destination = s->cookie_name;
		cv[1].destination = &(s->cookie_max_age);
		cv[2].destination = s->cookie_domain;
		cv[3].destination = s->cookie_attrs;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		if (buffer_string_is_empty(s->cookie_name)) {
			buffer_copy_string_len(s->cookie_name, CONST_STR_LEN("TRACKID"));
		} else {
			size_t j, len = buffer_string_length(s->cookie_name);
			for (j = 0; j < len; j++) {
				char c = s->cookie_name->ptr[j] | 32;
				if (c < 'a' || c > 'z') {
					log_error_write(srv, __FILE__, __LINE__, "sb",
							"invalid character in usertrack.cookie-name:",
							s->cookie_name);

					return HANDLER_ERROR;
				}
			}
		}

		if (!buffer_string_is_empty(s->cookie_domain)) {
			size_t j, len = buffer_string_length(s->cookie_domain);
			for (j = 0; j < len; j++) {
				char c = s->cookie_domain->ptr[j];
				if (c <= 32 || c >= 127 || c == '"' || c == '\\') {
					log_error_write(srv, __FILE__, __LINE__, "sb",
							"invalid character in usertrack.cookie-domain:",
							s->cookie_domain);

					return HANDLER_ERROR;
				}
			}
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_usertrack_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(cookie_name);
	PATCH(cookie_attrs);
	PATCH(cookie_domain);
	PATCH(cookie_max_age);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("usertrack.cookie-name"))) {
				PATCH(cookie_name);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("usertrack.cookie-attrs"))) {
				PATCH(cookie_attrs);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("usertrack.cookie-max-age"))) {
				PATCH(cookie_max_age);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("usertrack.cookie-domain"))) {
				PATCH(cookie_domain);
			}
		}
	}

	return 0;
}
#undef PATCH

URIHANDLER_FUNC(mod_usertrack_uri_handler) {
	plugin_data *p = p_d;
	buffer *cookie;
	buffer *b;
	unsigned char h[16];
	li_MD5_CTX Md5Ctx;
	char hh[LI_ITOSTRING_LENGTH];

	if (buffer_is_empty(con->uri.path)) return HANDLER_GO_ON;

	mod_usertrack_patch_connection(srv, con, p);

	if (NULL != (b = http_header_request_get(con, HTTP_HEADER_COOKIE, CONST_STR_LEN("Cookie")))) {
		char *g;
		/* we have a cookie, does it contain a valid name ? */

		/* parse the cookie
		 *
		 * check for cookiename + (WS | '=')
		 *
		 */

		if (NULL != (g = strstr(b->ptr, p->conf.cookie_name->ptr))) {
			char *nc;

			/* skip WS */
			for (nc = g + buffer_string_length(p->conf.cookie_name); *nc == ' ' || *nc == '\t'; nc++);

			if (*nc == '=') {
				/* ok, found the key of our own cookie */

				if (strlen(nc) > 32) {
					/* i'm lazy */
					return HANDLER_GO_ON;
				}
			}
		}
	}

	/* set a cookie */
	cookie = srv->tmp_buf;
	buffer_copy_buffer(cookie, p->conf.cookie_name);
	buffer_append_string_len(cookie, CONST_STR_LEN("="));


	/* taken from mod_auth.c */

	/* generate shared-secret */
	li_MD5_Init(&Md5Ctx);
	li_MD5_Update(&Md5Ctx, CONST_BUF_LEN(con->uri.path));
	li_MD5_Update(&Md5Ctx, CONST_STR_LEN("+"));

	li_itostrn(hh, sizeof(hh), srv->cur_ts);
	li_MD5_Update(&Md5Ctx, (unsigned char *)hh, strlen(hh));
	li_itostrn(hh, sizeof(hh), li_rand_pseudo());
	li_MD5_Update(&Md5Ctx, (unsigned char *)hh, strlen(hh));

	li_MD5_Final(h, &Md5Ctx);

	buffer_append_string_encoded_hex_lc(cookie, (char *)h, 16);

	/* usertrack.cookie-attrs, if set, replaces all other attrs */
	if (!buffer_string_is_empty(p->conf.cookie_attrs)) {
		buffer_append_string_buffer(cookie, p->conf.cookie_attrs);
		http_header_response_insert(con, HTTP_HEADER_SET_COOKIE, CONST_STR_LEN("Set-Cookie"), CONST_BUF_LEN(cookie));
		return HANDLER_GO_ON;
	}

	buffer_append_string_len(cookie, CONST_STR_LEN("; Path=/"));
	buffer_append_string_len(cookie, CONST_STR_LEN("; Version=1"));

	if (!buffer_string_is_empty(p->conf.cookie_domain)) {
		buffer_append_string_len(cookie, CONST_STR_LEN("; Domain="));
		buffer_append_string_encoded(cookie, CONST_BUF_LEN(p->conf.cookie_domain), ENCODING_REL_URI);
	}

	if (p->conf.cookie_max_age) {
		buffer_append_string_len(cookie, CONST_STR_LEN("; max-age="));
		buffer_append_int(cookie, p->conf.cookie_max_age);
	}

	http_header_response_insert(con, HTTP_HEADER_SET_COOKIE, CONST_STR_LEN("Set-Cookie"), CONST_BUF_LEN(cookie));

	return HANDLER_GO_ON;
}

/* this function is called at dlopen() time and inits the callbacks */

int mod_usertrack_plugin_init(plugin *p);
int mod_usertrack_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("usertrack");

	p->init        = mod_usertrack_init;
	p->handle_uri_clean  = mod_usertrack_uri_handler;
	p->set_defaults  = mod_usertrack_set_defaults;
	p->cleanup     = mod_usertrack_free;

	p->data        = NULL;

	return 0;
}
