#include "first.h"

#include "base.h"
#include "log.h"
#include "buffer.h"
#include "http_header.h"
#include "sock_addr.h"

#include "plugin.h"

#include <stdlib.h>
#include <string.h>

/**
 * mod_evasive
 *
 * we indent to implement all features the mod_evasive from apache has
 *
 * - limit of connections per IP
 * - provide a list of block-listed ip/networks (no access)
 * - provide a white-list of ips/network which is not affected by the limit
 *   (hmm, conditionals might be enough)
 * - provide a bandwidth limiter per IP
 *
 * started by:
 * - w1zzard@techpowerup.com
 */

typedef struct {
	unsigned short max_conns;
	unsigned short silent;
	buffer *location;
} plugin_config;

typedef struct {
	PLUGIN_DATA;

	plugin_config **config_storage;

	plugin_config conf;
} plugin_data;

INIT_FUNC(mod_evasive_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));

	return p;
}

FREE_FUNC(mod_evasive_free) {
	plugin_data *p = p_d;

	UNUSED(srv);

	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (NULL == s) continue;

			buffer_free(s->location);

			free(s);
		}
		free(p->config_storage);
	}

	free(p);

	return HANDLER_GO_ON;
}

SETDEFAULTS_FUNC(mod_evasive_set_defaults) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ "evasive.max-conns-per-ip",    NULL, T_CONFIG_SHORT, T_CONFIG_SCOPE_CONNECTION },   /* 0 */
		{ "evasive.silent",              NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 1 */
		{ "evasive.location",            NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 2 */
		{ NULL,                          NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->max_conns       = 0;
		s->silent          = 0;
		s->location        = buffer_init();

		cv[0].destination = &(s->max_conns);
		cv[1].destination = &(s->silent);
		cv[2].destination = s->location;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_evasive_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(max_conns);
	PATCH(silent);
	PATCH(location);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("evasive.max-conns-per-ip"))) {
				PATCH(max_conns);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("evasive.silent"))) {
				PATCH(silent);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("evasive.location"))) {
				PATCH(location);
			}
		}
	}

	return 0;
}
#undef PATCH

URIHANDLER_FUNC(mod_evasive_uri_handler) {
	plugin_data *p = p_d;
	size_t conns_by_ip = 0;
	size_t j;

	if (buffer_is_empty(con->uri.path)) return HANDLER_GO_ON;

	mod_evasive_patch_connection(srv, con, p);

	/* no limit set, nothing to block */
	if (p->conf.max_conns == 0) return HANDLER_GO_ON;

	for (j = 0; j < srv->conns->used; j++) {
		connection *c = srv->conns->ptr[j];

		/* check if other connections are already actively serving data for the same IP
		 * we can only ban connections which are already behind the 'read request' state
		 * */
		if (c->state <= CON_STATE_REQUEST_END) continue;

		if (!sock_addr_is_addr_eq(&c->dst_addr, &con->dst_addr)) continue;
		conns_by_ip++;

		if (conns_by_ip > p->conf.max_conns) {
			if (!p->conf.silent) {
				log_error_write(srv, __FILE__, __LINE__, "bs",
					con->dst_addr_buf,
					"turned away. Too many connections.");
			}

			if (!buffer_is_empty(p->conf.location)) {
				http_header_response_set(con, HTTP_HEADER_LOCATION, CONST_STR_LEN("Location"), CONST_BUF_LEN(p->conf.location));
				con->http_status = 302;
				con->file_finished = 1;
			} else {
				con->http_status = 403;
			}
			con->mode = DIRECT;
			return HANDLER_FINISHED;
		}
	}

	return HANDLER_GO_ON;
}


int mod_evasive_plugin_init(plugin *p);
int mod_evasive_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("evasive");

	p->init        = mod_evasive_init;
	p->set_defaults = mod_evasive_set_defaults;
	p->handle_uri_clean  = mod_evasive_uri_handler;
	p->cleanup     = mod_evasive_free;

	p->data        = NULL;

	return 0;
}
