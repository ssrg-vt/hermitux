#include "first.h"

#include "base.h"
#include "buffer.h"
#include "array.h"
#include "log.h"
#include "fdevent.h"
#include "http_header.h"
#include "sock_addr.h"

#include "configfile.h"

#include <string.h>
#include <stdlib.h>

/**
 * like all glue code this file contains functions which
 * are the external interface of lighttpd. The functions
 * are used by the server itself and the plugins.
 *
 * The main-goal is to have a small library in the end
 * which is linked against both and which will define
 * the interface itself in the end.
 *
 */


/* handle global options */

/* parse config array */
int config_insert_values_internal(server *srv, array *ca, const config_values_t cv[], config_scope_type_t scope) {
	size_t i;
	data_unset *du;

	for (i = 0; cv[i].key; i++) {

		if (NULL == (du = array_get_element_klen(ca, cv[i].key, strlen(cv[i].key)))) {
			/* no found */

			continue;
		}

		if ((T_CONFIG_SCOPE_SERVER == cv[i].scope)
		    && (T_CONFIG_SCOPE_SERVER != scope)) {
			/* server scope options should only be set in server scope, not in conditionals */
			log_error_write(srv, __FILE__, __LINE__, "ss",
				"DEPRECATED: don't set server options in conditionals, variable:",
				cv[i].key);
		}

		switch (cv[i].type) {
		case T_CONFIG_ARRAY:
			if (du->type == TYPE_ARRAY) {
				size_t j;
				data_array *da = (data_array *)du;

				for (j = 0; j < da->value->used; j++) {
					data_unset *ds = da->value->data[j];
					if (ds->type == TYPE_STRING || ds->type == TYPE_INTEGER || ds->type == TYPE_ARRAY) {
						array_insert_unique(cv[i].destination, ds->fn->copy(ds));
					} else {
						log_error_write(srv, __FILE__, __LINE__, "sssbsd",
								"the value of an array can only be a string, variable:",
								cv[i].key, "[", ds->key, "], type:", ds->type);

						return -1;
					}
				}
			} else {
				log_error_write(srv, __FILE__, __LINE__, "ss", cv[i].key, "should have been a array of strings like ... = ( \"...\" )");

				return -1;
			}
			break;
		case T_CONFIG_STRING:
			if (du->type == TYPE_STRING) {
				data_string *ds = (data_string *)du;

				buffer_copy_buffer(cv[i].destination, ds->value);
			} else {
				log_error_write(srv, __FILE__, __LINE__, "ss", cv[i].key, "should have been a string like ... = \"...\"");

				return -1;
			}
			break;
		case T_CONFIG_SHORT:
			switch(du->type) {
			case TYPE_INTEGER: {
				data_integer *di = (data_integer *)du;

				*((unsigned short *)(cv[i].destination)) = di->value;
				break;
			}
			case TYPE_STRING: {
				data_string *ds = (data_string *)du;

				/* If the value came from an environment variable, then it is a
				 * data_string, although it may contain a number in ASCII
				 * decimal format.  We try to interpret the string as a decimal
				 * short before giving up, in order to support setting numeric
				 * values with environment variables (eg, port number).
				 */
				if (ds->value->ptr && *ds->value->ptr) {
					char *e;
					long l = strtol(ds->value->ptr, &e, 10);
					if (e != ds->value->ptr && !*e && l >=0 && l <= 65535) {
						*((unsigned short *)(cv[i].destination)) = l;
						break;
					}
				}

				log_error_write(srv, __FILE__, __LINE__, "ssb", "got a string but expected a short:", cv[i].key, ds->value);

				return -1;
			}
			default:
				log_error_write(srv, __FILE__, __LINE__, "ssds", "unexpected type for key:", cv[i].key, du->type, "expected a short integer, range 0 ... 65535");
				return -1;
			}
			break;
		case T_CONFIG_INT:
			switch(du->type) {
			case TYPE_INTEGER: {
				data_integer *di = (data_integer *)du;

				*((unsigned int *)(cv[i].destination)) = di->value;
				break;
			}
			case TYPE_STRING: {
				data_string *ds = (data_string *)du;

				if (ds->value->ptr && *ds->value->ptr) {
					char *e;
					long l = strtol(ds->value->ptr, &e, 10);
					if (e != ds->value->ptr && !*e && l >= 0) {
						*((unsigned int *)(cv[i].destination)) = l;
						break;
					}
				}

				log_error_write(srv, __FILE__, __LINE__, "ssb", "got a string but expected an integer:", cv[i].key, ds->value);

				return -1;
			}
			default:
				log_error_write(srv, __FILE__, __LINE__, "ssds", "unexpected type for key:", cv[i].key, du->type, "expected an integer, range 0 ... 4294967295");
				return -1;
			}
			break;
		case T_CONFIG_BOOLEAN:
			if (du->type == TYPE_STRING) {
				data_string *ds = (data_string *)du;

				if (buffer_is_equal_string(ds->value, CONST_STR_LEN("enable"))) {
					*((unsigned short *)(cv[i].destination)) = 1;
				} else if (buffer_is_equal_string(ds->value, CONST_STR_LEN("disable"))) {
					*((unsigned short *)(cv[i].destination)) = 0;
				} else {
					log_error_write(srv, __FILE__, __LINE__, "ssbs", "ERROR: unexpected value for key:", cv[i].key, ds->value, "(enable|disable)");

					return -1;
				}
			} else {
				log_error_write(srv, __FILE__, __LINE__, "ssss", "ERROR: unexpected type for key:", cv[i].key, "(string)", "\"(enable|disable)\"");

				return -1;
			}
			break;
		case T_CONFIG_LOCAL:
		case T_CONFIG_UNSET:
			break;
		case T_CONFIG_UNSUPPORTED:
			log_error_write(srv, __FILE__, __LINE__, "ssss", "ERROR: found unsupported key:", cv[i].key, "-", (char *)(cv[i].destination));

			srv->config_unsupported = 1;

			break;
		case T_CONFIG_DEPRECATED:
			log_error_write(srv, __FILE__, __LINE__, "ssss", "ERROR: found deprecated key:", cv[i].key, "-", (char *)(cv[i].destination));

			srv->config_deprecated = 1;

			break;
		}
	}

	return 0;
}

int config_insert_values_global(server *srv, array *ca, const config_values_t cv[], config_scope_type_t scope) {
	size_t i;
	data_unset *du;

	for (i = 0; cv[i].key; i++) {
		if (NULL == (du = array_get_element_klen(ca, cv[i].key, strlen(cv[i].key)))) {
			/* no found */

			continue;
		}
		array_set_key_value(srv->config_touched, CONST_BUF_LEN(du->key), CONST_STR_LEN(""));
	}

	return config_insert_values_internal(srv, ca, cv, scope);
}

static const char* cond_result_to_string(cond_result_t cond_result) {
	switch (cond_result) {
	case COND_RESULT_UNSET: return "unset";
	case COND_RESULT_SKIP: return "skipped";
	case COND_RESULT_FALSE: return "false";
	case COND_RESULT_TRUE: return "true";
	default: return "invalid cond_result_t";
	}
}

static int config_addrstr_eq_remote_ip_mask(server *srv, const char *addrstr, int nm_bits, sock_addr *rmt) {
	/* special-case 0 == nm_bits to mean "all bits of the address" in addrstr */
	sock_addr addr;
	if (1 == sock_addr_inet_pton(&addr, addrstr, AF_INET, 0)) {
		if (nm_bits > 32) {
			log_error_write(srv, __FILE__, __LINE__, "sd", "ERROR: ipv4 netmask too large:", nm_bits);
			return -1;
		}
	} else if (1 == sock_addr_inet_pton(&addr, addrstr, AF_INET6, 0)) {
		if (nm_bits > 128) {
			log_error_write(srv, __FILE__, __LINE__, "sd", "ERROR: ipv6 netmask too large:", nm_bits);
			return -1;
		}
	} else {
		log_error_write(srv, __FILE__, __LINE__, "ss", "ERROR: ip addr is invalid:", addrstr);
		return -1;
	}
	return sock_addr_is_addr_eq_bits(&addr, rmt, nm_bits);
}

static int config_addrbuf_eq_remote_ip_mask(server *srv, buffer *string, char *nm_slash, sock_addr *rmt) {
	char *err;
	int nm_bits = strtol(nm_slash + 1, &err, 10);
	size_t addrstrlen = (size_t)(nm_slash - string->ptr);
	char addrstr[64]; /*(larger than INET_ADDRSTRLEN and INET6_ADDRSTRLEN)*/

	if (*err) {
		log_error_write(srv, __FILE__, __LINE__, "sbs", "ERROR: non-digit found in netmask:", string, err);
		return -1;
	}

	if (nm_bits <= 0) {
		if (*(nm_slash+1) == '\0') {
			log_error_write(srv, __FILE__, __LINE__, "sb", "ERROR: no number after / ", string);
		} else {
			log_error_write(srv, __FILE__, __LINE__, "sbs", "ERROR: invalid netmask <= 0:", string, err);
		}
		return -1;
	}

	if (addrstrlen >= sizeof(addrstr)) {
		log_error_write(srv, __FILE__, __LINE__, "sb", "ERROR: address string too long:", string);
		return -1;
	}

	memcpy(addrstr, string->ptr, addrstrlen);
	addrstr[addrstrlen] = '\0';

	return config_addrstr_eq_remote_ip_mask(srv, addrstr, nm_bits, rmt);
}

static cond_result_t config_check_cond_cached(server *srv, connection *con, data_config *dc);

static cond_result_t config_check_cond_nocache(server *srv, connection *con, data_config *dc) {
	buffer *l;
	server_socket *srv_sock = con->srv_socket;
	cond_cache_t *cache = &con->cond_cache[dc->context_ndx];

	/* check parent first */
	if (dc->parent && dc->parent->context_ndx) {
		/**
		 * a nested conditional 
		 *
		 * if the parent is not decided yet or false, we can't be true either 
		 */
		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__,  "sb", "go parent", dc->parent->key);
		}

		switch (config_check_cond_cached(srv, con, dc->parent)) {
		case COND_RESULT_UNSET:
			/* decide later */
			return COND_RESULT_UNSET;
		case COND_RESULT_SKIP:
		case COND_RESULT_FALSE:
			/* failed precondition */
			return COND_RESULT_SKIP;
		case COND_RESULT_TRUE:
			/* proceed */
			break;
		}
	}

	if (dc->prev) {
		/**
		 * a else branch; can only be executed if the previous branch
		 * was evaluated as "false" (not unset/skipped/true)
		 */
		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__,  "sb", "go prev", dc->prev->key);
		}

		/* make sure prev is checked first */
		switch (config_check_cond_cached(srv, con, dc->prev)) {
		case COND_RESULT_UNSET:
			/* decide later */
			return COND_RESULT_UNSET;
		case COND_RESULT_SKIP:
		case COND_RESULT_TRUE:
			/* failed precondition */
			return COND_RESULT_SKIP;
		case COND_RESULT_FALSE:
			/* proceed */
			break;
		}
	}

	if (!con->conditional_is_valid[dc->comp]) {
		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__,  "dss", 
				dc->comp,
				dc->key->ptr,
				"not available yet");
		}

		return COND_RESULT_UNSET;
	}

	/* if we had a real result before and weren't cleared just return it */
	switch (cache->local_result) {
	case COND_RESULT_TRUE:
	case COND_RESULT_FALSE:
		return cache->local_result;
	default:
		break;
	}

	if (CONFIG_COND_ELSE == dc->cond) return COND_RESULT_TRUE;

	/* pass the rules */

	switch (dc->comp) {
	case COMP_HTTP_HOST: {
		char *ck_colon = NULL, *val_colon = NULL;
		unsigned short port;

		if (!buffer_string_is_empty(con->uri.authority)) {

			/*
			 * append server-port to the HTTP_POST if necessary
			 */

			l = con->uri.authority;

			switch(dc->cond) {
			case CONFIG_COND_NE:
			case CONFIG_COND_EQ:
				port = sock_addr_get_port(&srv_sock->addr);
				if (0 == port) break;
				ck_colon = strchr(dc->string->ptr, ':');
				val_colon = strchr(l->ptr, ':');

				if (NULL != ck_colon && NULL == val_colon) {
					/* condition "host:port" but client send "host" */
					buffer_copy_buffer(srv->cond_check_buf, l);
					buffer_append_string_len(srv->cond_check_buf, CONST_STR_LEN(":"));
					buffer_append_int(srv->cond_check_buf, port);
					l = srv->cond_check_buf;
				} else if (NULL != val_colon && NULL == ck_colon) {
					/* condition "host" but client send "host:port" */
					buffer_copy_string_len(srv->cond_check_buf, l->ptr, val_colon - l->ptr);
					l = srv->cond_check_buf;
				}
				break;
			default:
				break;
			}
		} else {
			l = srv->empty_string;
		}
		break;
	}
	case COMP_HTTP_REMOTE_IP: {
		char *nm_slash;
		/* handle remoteip limitations
		 *
		 * "10.0.0.1" is provided for all comparisions
		 *
		 * only for == and != we support
		 *
		 * "10.0.0.1/24"
		 */

		if ((dc->cond == CONFIG_COND_EQ ||
		     dc->cond == CONFIG_COND_NE) &&
		    (NULL != (nm_slash = strchr(dc->string->ptr, '/')))) {
			switch (config_addrbuf_eq_remote_ip_mask(srv, dc->string, nm_slash, &con->dst_addr)) {
			case  1: return (dc->cond == CONFIG_COND_EQ) ? COND_RESULT_TRUE : COND_RESULT_FALSE;
			case  0: return (dc->cond == CONFIG_COND_EQ) ? COND_RESULT_FALSE : COND_RESULT_TRUE;
			case -1: return COND_RESULT_FALSE; /*(error parsing configfile entry)*/
			}
		}
		l = con->dst_addr_buf;
		break;
	}
	case COMP_HTTP_SCHEME:
		l = con->uri.scheme;
		break;

	case COMP_HTTP_URL:
		l = con->uri.path;
		break;

	case COMP_HTTP_QUERY_STRING:
		l = con->uri.query;
		break;

	case COMP_SERVER_SOCKET:
		l = srv_sock->srv_token;
		break;

	case COMP_HTTP_REQUEST_HEADER:
		l = http_header_request_get(con, HTTP_HEADER_UNSPECIFIED, CONST_BUF_LEN(dc->comp_tag));
		if (NULL == l) l = srv->empty_string;
		break;
	case COMP_HTTP_REQUEST_METHOD:
		l = srv->tmp_buf;
		buffer_clear(l);
		http_method_append(l, con->request.http_method);
		break;
	default:
		return COND_RESULT_FALSE;
	}

	if (NULL == l) {
		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__,  "bsbs", dc->comp_key,
					"(", l, ") compare to NULL");
		}
		return COND_RESULT_FALSE;
	}

	if (con->conf.log_condition_handling) {
		log_error_write(srv, __FILE__, __LINE__,  "bsbsb", dc->comp_key,
				"(", l, ") compare to ", dc->string);
	}
	switch(dc->cond) {
	case CONFIG_COND_NE:
	case CONFIG_COND_EQ:
		if (buffer_is_equal(l, dc->string)) {
			return (dc->cond == CONFIG_COND_EQ) ? COND_RESULT_TRUE : COND_RESULT_FALSE;
		} else {
			return (dc->cond == CONFIG_COND_EQ) ? COND_RESULT_FALSE : COND_RESULT_TRUE;
		}
	case CONFIG_COND_NOMATCH:
	case CONFIG_COND_MATCH: {
		if (data_config_pcre_exec(dc, cache, l) > 0) {
			return (dc->cond == CONFIG_COND_MATCH) ? COND_RESULT_TRUE : COND_RESULT_FALSE;
		} else {
			/* cache is already cleared */
			return (dc->cond == CONFIG_COND_MATCH) ? COND_RESULT_FALSE : COND_RESULT_TRUE;
		}
	}
	default:
		/* no way */
		break;
	}

	return COND_RESULT_FALSE;
}

static cond_result_t config_check_cond_cached(server *srv, connection *con, data_config *dc) {
	cond_cache_t *caches = con->cond_cache;

	if (COND_RESULT_UNSET == caches[dc->context_ndx].result) {
		caches[dc->context_ndx].result = config_check_cond_nocache(srv, con, dc);
		switch (caches[dc->context_ndx].result) {
		case COND_RESULT_FALSE:
		case COND_RESULT_TRUE:
			/* remember result of local condition for a partial reset */
			caches[dc->context_ndx].local_result = caches[dc->context_ndx].result;
			break;
		default:
			break;
		}

		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__, "dss",
				dc->context_ndx,
				"(uncached) result:",
				cond_result_to_string(caches[dc->context_ndx].result));
		}
	} else {
		if (con->conf.log_condition_handling) {
			log_error_write(srv, __FILE__, __LINE__, "dss",
				dc->context_ndx,
				"(cached) result:",
				cond_result_to_string(caches[dc->context_ndx].result));
		}
	}
	return caches[dc->context_ndx].result;
}

/* if we reset the cache result for a node, we also need to clear all
 * child nodes and else-branches*/
static void config_cond_clear_node(server *srv, connection *con, data_config *dc) {
	/* if a node is "unset" all children are unset too */
	if (con->cond_cache[dc->context_ndx].result != COND_RESULT_UNSET) {
		size_t i;

	      #if 0
		/* (redundant; matches not relevant unless COND_RESULT_TRUE) */
		switch (con->cond_cache[dc->context_ndx].local_result) {
		case COND_RESULT_TRUE:
		case COND_RESULT_FALSE:
			break;
		default:
			con->cond_cache[dc->context_ndx].patterncount = 0;
			con->cond_cache[dc->context_ndx].comp_value = NULL;
		}
	      #endif
		con->cond_cache[dc->context_ndx].result = COND_RESULT_UNSET;

		for (i = 0; i < dc->children.used; ++i) {
			data_config *dc_child = dc->children.data[i];
			if (NULL == dc_child->prev) {
				/* only call for first node in if-else chain */
				config_cond_clear_node(srv, con, dc_child);
			}
		}
		if (NULL != dc->next) config_cond_clear_node(srv, con, dc->next);
	}
}

/**
 * reset the config-cache for a named item
 *
 * if the item is COND_LAST_ELEMENT we reset all items
 */
void config_cond_cache_reset_item(server *srv, connection *con, comp_key_t item) {
	size_t i;

	for (i = 0; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];

		if (item == dc->comp) {
			/* clear local_result */
			con->cond_cache[i].local_result = COND_RESULT_UNSET;
			/* clear result in subtree (including the node itself) */
			config_cond_clear_node(srv, con, dc);
		}
	}
}

/**
 * reset the config cache to its initial state at connection start
 */
void config_cond_cache_reset(server *srv, connection *con) {
	size_t i;

	/* resetting all entries; no need to follow children as in config_cond_cache_reset_item */
	for (i = 0; i < srv->config_context->used; i++) {
		con->cond_cache[i].result = COND_RESULT_UNSET;
		con->cond_cache[i].local_result = COND_RESULT_UNSET;
		con->cond_cache[i].patterncount = 0;
		con->cond_cache[i].comp_value = NULL;
	}

	for (i = 0; i < COMP_LAST_ELEMENT; i++) {
		con->conditional_is_valid[i] = 0;
	}
}

int config_check_cond(server *srv, connection *con, data_config *dc) {
	if (con->conf.log_condition_handling) {
		log_error_write(srv, __FILE__, __LINE__,  "s",  "=== start of condition block ===");
	}
	return (config_check_cond_cached(srv, con, dc) == COND_RESULT_TRUE);
}
