#include "first.h"

#include "mod_cml.h"

#include "base.h"
#include "buffer.h"
#include "log.h"
#include "plugin.h"

#include <sys/stat.h>
#include <time.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* init the plugin data */
INIT_FUNC(mod_cml_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));

	p->basedir         = buffer_init();
	p->baseurl         = buffer_init();
	p->trigger_handler = buffer_init();

	return p;
}

/* detroy the plugin data */
FREE_FUNC(mod_cml_free) {
	plugin_data *p = p_d;

	UNUSED(srv);

	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (NULL == s) continue;

			buffer_free(s->ext);

			buffer_free(s->mc_namespace);
			buffer_free(s->power_magnet);
			array_free(s->mc_hosts);

#if defined(USE_MEMCACHED)
			if (s->memc) memcached_free(s->memc);
#endif

			free(s);
		}
		free(p->config_storage);
	}

	buffer_free(p->trigger_handler);
	buffer_free(p->basedir);
	buffer_free(p->baseurl);

	free(p);

	return HANDLER_GO_ON;
}

/* handle plugin config and check values */

SETDEFAULTS_FUNC(mod_cml_set_defaults) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ "cml.extension",              NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 0 */
		{ "cml.memcache-hosts",         NULL, T_CONFIG_ARRAY, T_CONFIG_SCOPE_CONNECTION },        /* 1 */
		{ "cml.memcache-namespace",     NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 2 */
		{ "cml.power-magnet",           NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },       /* 3 */
		{ NULL,                         NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->ext    = buffer_init();
		s->mc_hosts       = array_init();
		s->mc_namespace   = buffer_init();
		s->power_magnet   = buffer_init();
#if defined(USE_MEMCACHED)
		s->memc = NULL;
#endif

		cv[0].destination = s->ext;
		cv[1].destination = s->mc_hosts;
		cv[2].destination = s->mc_namespace;
		cv[3].destination = s->power_magnet;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		if (!array_is_vlist(s->mc_hosts)) {
			log_error_write(srv, __FILE__, __LINE__, "s",
					"unexpected value for cml.memcache-hosts; expected list of \"host\"");
			return HANDLER_ERROR;
		}

		if (s->mc_hosts->used) {
#if defined(USE_MEMCACHED)
			buffer *option_string = buffer_init();
			size_t k;

			{
				data_string *ds = (data_string *)s->mc_hosts->data[0];

				buffer_append_string_len(option_string, CONST_STR_LEN("--SERVER="));
				buffer_append_string_buffer(option_string, ds->value);
			}

			for (k = 1; k < s->mc_hosts->used; k++) {
				data_string *ds = (data_string *)s->mc_hosts->data[k];

				buffer_append_string_len(option_string, CONST_STR_LEN(" --SERVER="));
				buffer_append_string_buffer(option_string, ds->value);
			}

			s->memc = memcached(CONST_BUF_LEN(option_string));

			if (NULL == s->memc) {
				log_error_write(srv, __FILE__, __LINE__, "sb",
					"configuring memcached failed for option string:",
					option_string);
			}
			buffer_free(option_string);

			if (NULL == s->memc) return HANDLER_ERROR;
#else
			log_error_write(srv, __FILE__, __LINE__, "s",
				"memcache support is not compiled in but cml.memcache-hosts is set, aborting");
			return HANDLER_ERROR;
#endif
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_cml_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(ext);
#if defined(USE_MEMCACHED)
	PATCH(memc);
#endif
	PATCH(mc_namespace);
	PATCH(power_magnet);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("cml.extension"))) {
				PATCH(ext);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("cml.memcache-hosts"))) {
#if defined(USE_MEMCACHED)
				PATCH(memc);
#endif
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("cml.memcache-namespace"))) {
				PATCH(mc_namespace);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("cml.power-magnet"))) {
				PATCH(power_magnet);
			}
		}
	}

	return 0;
}
#undef PATCH

static int cache_call_lua(server *srv, connection *con, plugin_data *p, buffer *cml_file) {
	buffer *b;
	char *c;

	/* cleanup basedir */
	b = p->baseurl;
	buffer_copy_buffer(b, con->uri.path);
	for (c = b->ptr + buffer_string_length(b); c > b->ptr && *c != '/'; c--);

	if (*c == '/') {
		buffer_string_set_length(b, c - b->ptr + 1);
	}

	b = p->basedir;
	buffer_copy_buffer(b, con->physical.path);
	for (c = b->ptr + buffer_string_length(b); c > b->ptr && *c != '/'; c--);

	if (*c == '/') {
		buffer_string_set_length(b, c - b->ptr + 1);
	}


	/* prepare variables
	 *   - cookie-based
	 *   - get-param-based
	 */
	return cache_parse_lua(srv, con, p, cml_file);
}

URIHANDLER_FUNC(mod_cml_power_magnet) {
	plugin_data *p = p_d;

	mod_cml_patch_connection(srv, con, p);

	buffer_clear(p->basedir);
	buffer_clear(p->baseurl);
	buffer_clear(p->trigger_handler);

	if (buffer_string_is_empty(p->conf.power_magnet)) return HANDLER_GO_ON;

	/*
	 * power-magnet:
	 * cml.power-magnet = server.docroot + "/rewrite.cml"
	 *
	 * is called on EACH request, take the original REQUEST_URI and modifies the
	 * request header as neccesary.
	 *
	 * First use:
	 * if file_exists("/maintainance.html") {
	 *   output_include = ( "/maintainance.html" )
	 *   return CACHE_HIT
	 * }
	 *
	 * as we only want to rewrite HTML like requests we should cover it in a conditional
	 *
	 * */

	switch(cache_call_lua(srv, con, p, p->conf.power_magnet)) {
	case -1:
		/* error */
		if (con->conf.log_request_handling) {
			log_error_write(srv, __FILE__, __LINE__, "s", "cache-error");
		}
		con->http_status = 500;
		return HANDLER_COMEBACK;
	case 0:
		if (con->conf.log_request_handling) {
			log_error_write(srv, __FILE__, __LINE__, "s", "cache-hit");
		}
		/* cache-hit */
		buffer_reset(con->physical.path);
		return HANDLER_FINISHED;
	case 1:
		/* cache miss */
		return HANDLER_GO_ON;
	default:
		con->http_status = 500;
		return HANDLER_COMEBACK;
	}
}

URIHANDLER_FUNC(mod_cml_is_handled) {
	plugin_data *p = p_d;

	if (buffer_string_is_empty(con->physical.path)) return HANDLER_ERROR;

	mod_cml_patch_connection(srv, con, p);

	buffer_clear(p->basedir);
	buffer_clear(p->baseurl);
	buffer_clear(p->trigger_handler);

	if (buffer_string_is_empty(p->conf.ext)) return HANDLER_GO_ON;

	if (!buffer_is_equal_right_len(con->physical.path, p->conf.ext, buffer_string_length(p->conf.ext))) {
		return HANDLER_GO_ON;
	}

	switch(cache_call_lua(srv, con, p, con->physical.path)) {
	case -1:
		/* error */
		if (con->conf.log_request_handling) {
			log_error_write(srv, __FILE__, __LINE__, "s", "cache-error");
		}
		con->http_status = 500;
		return HANDLER_COMEBACK;
	case 0:
		if (con->conf.log_request_handling) {
			log_error_write(srv, __FILE__, __LINE__, "s", "cache-hit");
		}
		/* cache-hit */
		buffer_reset(con->physical.path);
		return HANDLER_FINISHED;
	case 1:
		if (con->conf.log_request_handling) {
			log_error_write(srv, __FILE__, __LINE__, "s", "cache-miss");
		}
		/* cache miss */
		return HANDLER_COMEBACK;
	default:
		con->http_status = 500;
		return HANDLER_COMEBACK;
	}
}

int mod_cml_plugin_init(plugin *p);
int mod_cml_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("cache");

	p->init        = mod_cml_init;
	p->cleanup     = mod_cml_free;
	p->set_defaults  = mod_cml_set_defaults;

	p->handle_subrequest_start = mod_cml_is_handled;
	p->handle_physical         = mod_cml_power_magnet;

	p->data        = NULL;

	return 0;
}
