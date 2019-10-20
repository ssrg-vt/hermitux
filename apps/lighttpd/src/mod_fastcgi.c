#include "first.h"

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "gw_backend.h"
typedef gw_plugin_config plugin_config;
typedef gw_plugin_data   plugin_data;
typedef gw_handler_ctx   handler_ctx;

#include "base.h"
#include "buffer.h"
#include "fdevent.h"
#include "http_chunk.h"
#include "log.h"
#include "status_counter.h"

#ifdef HAVE_FASTCGI_FASTCGI_H
# include <fastcgi/fastcgi.h>
#else
# ifdef HAVE_FASTCGI_H
#  include <fastcgi.h>
# else
#  include "fastcgi.h"
# endif
#endif /* HAVE_FASTCGI_FASTCGI_H */

#if GW_RESPONDER  != FCGI_RESPONDER
#error "mismatched defines: (GW_RESPONDER != FCGI_RESPONDER)"
#endif
#if GW_AUTHORIZER != FCGI_AUTHORIZER
#error "mismatched defines: (GW_AUTHORIZER != FCGI_AUTHORIZER)"
#endif
#if GW_FILTER     != FCGI_FILTER
#error "mismatched defines: (GW_FILTER != FCGI_FILTER)"
#endif

SETDEFAULTS_FUNC(mod_fastcgi_set_defaults) {
	plugin_data *p = p_d;
	data_unset *du;
	size_t i = 0;

	config_values_t cv[] = {
		{ "fastcgi.server",              NULL, T_CONFIG_LOCAL, T_CONFIG_SCOPE_CONNECTION },       /* 0 */
		{ "fastcgi.debug",               NULL, T_CONFIG_INT  , T_CONFIG_SCOPE_CONNECTION },       /* 1 */
		{ "fastcgi.map-extensions",      NULL, T_CONFIG_ARRAY, T_CONFIG_SCOPE_CONNECTION },       /* 2 */
		{ "fastcgi.balance",             NULL, T_CONFIG_LOCAL, T_CONFIG_SCOPE_CONNECTION },       /* 3 */
		{ NULL,                          NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));
	force_assert(p->config_storage);

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		force_assert(s);
		s->exts          = NULL;
		s->exts_auth     = NULL;
		s->exts_resp     = NULL;
		s->debug         = 0;
		s->ext_mapping   = array_init();

		cv[0].destination = s->exts; /* not used; T_CONFIG_LOCAL */
		cv[1].destination = &(s->debug);
		cv[2].destination = s->ext_mapping;
		cv[3].destination = NULL;    /* not used; T_CONFIG_LOCAL */

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		du = array_get_element(config->value, "fastcgi.server");
		if (!gw_set_defaults_backend(srv, p, du, i, 0)) {
			return HANDLER_ERROR;
		}

		du = array_get_element(config->value, "fastcgi.balance");
		if (!gw_set_defaults_balance(srv, s, du)) {
			return HANDLER_ERROR;
		}
	}

	return HANDLER_GO_ON;
}

static int fcgi_env_add(void *venv, const char *key, size_t key_len, const char *val, size_t val_len) {
	buffer *env = venv;
	size_t len;
	char len_enc[8];
	size_t len_enc_len = 0;
	char *dst;

	if (!key || !val) return -1;

	len = key_len + val_len;

	len += key_len > 127 ? 4 : 1;
	len += val_len > 127 ? 4 : 1;

	if (buffer_string_length(env) + len >= FCGI_MAX_LENGTH + sizeof(FCGI_BeginRequestRecord) + sizeof(FCGI_Header)) {
		/**
		 * we can't append more headers, ignore it
		 */
		return -1;
	}

	/**
	 * field length can be 31bit max
	 *
	 * HINT: this can't happen as FCGI_MAX_LENGTH is only 16bit
	 */
	force_assert(key_len < 0x7fffffffu);
	force_assert(val_len < 0x7fffffffu);

	if (buffer_string_space(env) < len) {
		size_t extend = env->size * 2 - buffer_string_length(env);
		extend = extend > len ? extend : len + 4095;
		buffer_string_prepare_append(env, extend);
	}

	if (key_len > 127) {
		len_enc[len_enc_len++] = ((key_len >> 24) & 0xff) | 0x80;
		len_enc[len_enc_len++] = (key_len >> 16) & 0xff;
		len_enc[len_enc_len++] = (key_len >> 8) & 0xff;
		len_enc[len_enc_len++] = (key_len >> 0) & 0xff;
	} else {
		len_enc[len_enc_len++] = (key_len >> 0) & 0xff;
	}

	if (val_len > 127) {
		len_enc[len_enc_len++] = ((val_len >> 24) & 0xff) | 0x80;
		len_enc[len_enc_len++] = (val_len >> 16) & 0xff;
		len_enc[len_enc_len++] = (val_len >> 8) & 0xff;
		len_enc[len_enc_len++] = (val_len >> 0) & 0xff;
	} else {
		len_enc[len_enc_len++] = (val_len >> 0) & 0xff;
	}

	dst = buffer_string_prepare_append(env, len);
	memcpy(dst, len_enc, len_enc_len);
	memcpy(dst + len_enc_len, key, key_len);
	memcpy(dst + len_enc_len + key_len, val, val_len);
	buffer_commit(env, len);

	return 0;
}

static void fcgi_header(FCGI_Header * header, unsigned char type, int request_id, int contentLength, unsigned char paddingLength) {
	force_assert(contentLength <= FCGI_MAX_LENGTH);
	
	header->version = FCGI_VERSION_1;
	header->type = type;
	header->requestIdB0 = request_id & 0xff;
	header->requestIdB1 = (request_id >> 8) & 0xff;
	header->contentLengthB0 = contentLength & 0xff;
	header->contentLengthB1 = (contentLength >> 8) & 0xff;
	header->paddingLength = paddingLength;
	header->reserved = 0;
}

static handler_t fcgi_stdin_append(server *srv, handler_ctx *hctx) {
	FCGI_Header header;
	connection *con = hctx->remote_conn;
	chunkqueue *req_cq = con->request_content_queue;
	off_t offset, weWant;
	const off_t req_cqlen = req_cq->bytes_in - req_cq->bytes_out;
	int request_id = hctx->request_id;
	UNUSED(srv);

	/* something to send ? */
	for (offset = 0; offset != req_cqlen; offset += weWant) {
		weWant = req_cqlen - offset > FCGI_MAX_LENGTH ? FCGI_MAX_LENGTH : req_cqlen - offset;

		if (-1 != hctx->wb_reqlen) {
			if (hctx->wb_reqlen >= 0) {
				hctx->wb_reqlen += sizeof(header);
			} else {
				hctx->wb_reqlen -= sizeof(header);
			}
		}

		fcgi_header(&(header), FCGI_STDIN, request_id, weWant, 0);
		(chunkqueue_is_empty(hctx->wb) || hctx->wb->first->type == MEM_CHUNK) /* else FILE_CHUNK for temp file */
		  ? chunkqueue_append_mem(hctx->wb, (const char *)&header, sizeof(header))
		  : chunkqueue_append_mem_min(hctx->wb, (const char *)&header, sizeof(header));
		chunkqueue_steal(hctx->wb, req_cq, weWant);
		/*(hctx->wb_reqlen already includes content_length)*/
	}

	if (hctx->wb->bytes_in == hctx->wb_reqlen) {
		/* terminate STDIN */
		/* (future: must defer ending FCGI_STDIN
		 *  if might later upgrade protocols
		 *  and then have more data to send) */
		fcgi_header(&(header), FCGI_STDIN, request_id, 0, 0);
		chunkqueue_append_mem(hctx->wb, (const char *)&header, sizeof(header));
		hctx->wb_reqlen += (int)sizeof(header);
	}

	return HANDLER_GO_ON;
}

static handler_t fcgi_create_env(server *srv, handler_ctx *hctx) {
	FCGI_BeginRequestRecord beginRecord;
	FCGI_Header header;
	int request_id;

	gw_host *host = hctx->host;
	connection *con   = hctx->remote_conn;

	http_cgi_opts opts = {
	  (hctx->gw_mode == FCGI_AUTHORIZER),
	  host->break_scriptfilename_for_php,
	  host->docroot,
	  host->strip_request_uri
	};

	size_t rsz = (size_t)(con->read_queue->bytes_out - hctx->wb->bytes_in);
	buffer * const b = chunkqueue_prepend_buffer_open_sz(hctx->wb, rsz < 65536 ? rsz : con->header_len);

	/* send FCGI_BEGIN_REQUEST */

	if (hctx->request_id == 0) {
		hctx->request_id = 1; /* always use id 1 as we don't use multiplexing */
	} else {
		log_error_write(srv, __FILE__, __LINE__, "sd",
				"fcgi-request is already in use:", hctx->request_id);
	}
	request_id = hctx->request_id;

	fcgi_header(&(beginRecord.header), FCGI_BEGIN_REQUEST, request_id, sizeof(beginRecord.body), 0);
	beginRecord.body.roleB0 = hctx->gw_mode;
	beginRecord.body.roleB1 = 0;
	beginRecord.body.flags = 0;
	memset(beginRecord.body.reserved, 0, sizeof(beginRecord.body.reserved));

	buffer_copy_string_len(b, (const char *)&beginRecord, sizeof(beginRecord));
	fcgi_header(&header, FCGI_PARAMS, request_id, 0, 0); /*(set aside space to fill in later)*/
	buffer_append_string_len(b, (const char *)&header, sizeof(header));

	/* send FCGI_PARAMS */

	if (0 != http_cgi_headers(srv, con, &opts, fcgi_env_add, b)) {
		con->http_status = 400;
		con->mode = DIRECT;
		buffer_clear(b);
		chunkqueue_remove_finished_chunks(hctx->wb);
		return HANDLER_FINISHED;
	} else {
		fcgi_header(&(header), FCGI_PARAMS, request_id,
			    buffer_string_length(b) - sizeof(FCGI_BeginRequestRecord) - sizeof(FCGI_Header), 0);
		memcpy(b->ptr+sizeof(FCGI_BeginRequestRecord), (const char *)&header, sizeof(header));

		fcgi_header(&(header), FCGI_PARAMS, request_id, 0, 0);
		buffer_append_string_len(b, (const char *)&header, sizeof(header));

		hctx->wb_reqlen = buffer_string_length(b);
		chunkqueue_prepend_buffer_commit(hctx->wb);
	}

	if (con->request.content_length) {
		/*chunkqueue_append_chunkqueue(hctx->wb, con->request_content_queue);*/
		if (con->request.content_length > 0)
			hctx->wb_reqlen += con->request.content_length;/* (eventual) (minimal) total request size, not necessarily including all fcgi_headers around content length yet */
		else /* as-yet-unknown total request size (Transfer-Encoding: chunked)*/
			hctx->wb_reqlen = -hctx->wb_reqlen;
	}
	fcgi_stdin_append(srv, hctx);

	status_counter_inc(srv, CONST_STR_LEN("fastcgi.requests"));
	return HANDLER_GO_ON;
}

typedef struct {
	unsigned int len;
	int      type;
	int      padding;
	int      request_id;
} fastcgi_response_packet;

static int fastcgi_get_packet(server *srv, handler_ctx *hctx, fastcgi_response_packet *packet) {
	FCGI_Header header;
	size_t toread = sizeof(FCGI_Header), flen = 0;
	off_t rblen = chunkqueue_length(hctx->rb);
	if (rblen < (off_t)sizeof(FCGI_Header)) {
		/* no header */
		if (hctx->conf.debug && 0 != rblen) {
			log_error_write(srv, __FILE__, __LINE__, "sosds", "FastCGI: header too small:", rblen, "bytes <", sizeof(FCGI_Header), "bytes, waiting for more data");
		}
		return -1;
	}

	/* get at least the FastCGI header */
	for (chunk *c = hctx->rb->first; c; c = c->next) {
		size_t weHave = buffer_string_length(c->mem) - c->offset;
		if (weHave >= toread) {
			memcpy((char *)&header + flen, c->mem->ptr + c->offset, toread);
			break;
		}

		memcpy((char *)&header + flen, c->mem->ptr + c->offset, weHave);
		flen += weHave;
		toread -= weHave;
	}

	/* we have at least a header, now check how much we have to fetch */
	packet->len = (header.contentLengthB0 | (header.contentLengthB1 << 8)) + header.paddingLength;
	packet->request_id = (header.requestIdB0 | (header.requestIdB1 << 8));
	packet->type = header.type;
	packet->padding = header.paddingLength;

	if (packet->len > (unsigned int)rblen-sizeof(FCGI_Header)) {
		return -1; /* we didn't get the full packet */
	}

	chunkqueue_mark_written(hctx->rb, sizeof(FCGI_Header));
	return 0;
}

static void fastcgi_get_packet_body(buffer *b, handler_ctx *hctx, fastcgi_response_packet *packet) {
	/* copy content; hctx->rb must contain at least packet->len content */
	size_t toread = packet->len - packet->padding;
	buffer_string_prepare_append(b, toread);
	for (chunk *c = hctx->rb->first; c; c = c->next) {
		size_t weHave = buffer_string_length(c->mem) - c->offset;
		if (weHave >= toread) {
			buffer_append_string_len(b, c->mem->ptr + c->offset, toread);
			break;
		}

		buffer_append_string_len(b, c->mem->ptr + c->offset, weHave);
		toread -= weHave;
	}
	chunkqueue_mark_written(hctx->rb, packet->len);
}

static handler_t fcgi_recv_parse(server *srv, connection *con, struct http_response_opts_t *opts, buffer *b, size_t n) {
	handler_ctx *hctx = (handler_ctx *)opts->pdata;
	int fin = 0;

	if (0 == n) {
		if (-1 == hctx->request_id) return HANDLER_FINISHED; /*(flag request ended)*/
		if (!(fdevent_fdnode_interest(hctx->fdn) & FDEVENT_IN)
		    && !(con->conf.stream_response_body & FDEVENT_STREAM_RESPONSE_POLLRDHUP))
			return HANDLER_GO_ON;
		log_error_write(srv, __FILE__, __LINE__, "ssdsb",
				"unexpected end-of-file (perhaps the fastcgi process died):",
				"pid:", hctx->proc->pid,
				"socket:", hctx->proc->connection_name);

		return HANDLER_ERROR;
	}

	chunkqueue_append_buffer(hctx->rb, b);

	/*
	 * parse the fastcgi packets and forward the content to the write-queue
	 *
	 */
	while (fin == 0) {
		fastcgi_response_packet packet;

		/* check if we have at least one packet */
		if (0 != fastcgi_get_packet(srv, hctx, &packet)) {
			/* no full packet */
			break;
		}

		switch(packet.type) {
		case FCGI_STDOUT:
			if (packet.len == 0) break;

			/* is the header already finished */
			if (0 == con->file_started) {
				/* split header from body */
				buffer *hdrs = hctx->response;
				if (NULL == hdrs) {
					hdrs = srv->tmp_buf;
					buffer_clear(srv->tmp_buf);
				}
				fastcgi_get_packet_body(hdrs, hctx, &packet);
				if (HANDLER_GO_ON != http_response_parse_headers(srv, con, &hctx->opts, hdrs)) {
					hctx->send_content_body = 0;
					fin = 1;
					break;
				}
				if (0 == con->file_started) {
					if (!hctx->response) {
						hctx->response = chunk_buffer_acquire();
						buffer_copy_buffer(hctx->response, hdrs);
					}
				}
				else if (hctx->gw_mode == GW_AUTHORIZER &&
					 (con->http_status == 0 || con->http_status == 200)) {
					/* authorizer approved request; ignore the content here */
					hctx->send_content_body = 0;
				}
			} else if (hctx->send_content_body) {
				if (0 != http_chunk_transfer_cqlen(srv, con, hctx->rb, packet.len - packet.padding)) {
					/* error writing to tempfile;
					 * truncate response or send 500 if nothing sent yet */
					fin = 1;
				}
				if (packet.padding) chunkqueue_mark_written(hctx->rb, packet.padding);
			} else {
				chunkqueue_mark_written(hctx->rb, packet.len);
			}
			break;
		case FCGI_STDERR:
			if (packet.len == 0) break;

			buffer_clear(srv->tmp_buf);
			fastcgi_get_packet_body(srv->tmp_buf, hctx, &packet);
			log_error_write_multiline_buffer(srv, __FILE__, __LINE__, srv->tmp_buf, "s",
					"FastCGI-stderr:");

			break;
		case FCGI_END_REQUEST:
			hctx->request_id = -1; /*(flag request ended)*/
			fin = 1;
			break;
		default:
			log_error_write(srv, __FILE__, __LINE__, "sd",
					"FastCGI: header.type not handled: ", packet.type);
			chunkqueue_mark_written(hctx->rb, packet.len);
			break;
		}
	}

	return 0 == fin ? HANDLER_GO_ON : HANDLER_FINISHED;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int fcgi_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(exts);
	PATCH(exts_auth);
	PATCH(exts_resp);
	PATCH(debug);
	PATCH(balance);
	PATCH(ext_mapping);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("fastcgi.server"))) {
				PATCH(exts);
				PATCH(exts_auth);
				PATCH(exts_resp);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("fastcgi.debug"))) {
				PATCH(debug);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("fastcgi.balance"))) {
				PATCH(balance);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("fastcgi.map-extensions"))) {
				PATCH(ext_mapping);
			}
		}
	}

	return 0;
}
#undef PATCH

static handler_t fcgi_check_extension(server *srv, connection *con, void *p_d, int uri_path_handler) {
	plugin_data *p = p_d;
	handler_t rc;

	if (con->mode != DIRECT) return HANDLER_GO_ON;

	fcgi_patch_connection(srv, con, p);
	if (NULL == p->conf.exts) return HANDLER_GO_ON;

	rc = gw_check_extension(srv, con, p, uri_path_handler, 0);
	if (HANDLER_GO_ON != rc) return rc;

	if (con->mode == p->id) {
		handler_ctx *hctx = con->plugin_ctx[p->id];
		hctx->opts.backend = BACKEND_FASTCGI;
		hctx->opts.parse = fcgi_recv_parse;
		hctx->opts.pdata = hctx;
		hctx->stdin_append = fcgi_stdin_append;
		hctx->create_env = fcgi_create_env;
		if (!hctx->rb) {
			hctx->rb = chunkqueue_init();
		}
		else {
			chunkqueue_reset(hctx->rb);
		}
	}

	return HANDLER_GO_ON;
}

/* uri-path handler */
static handler_t fcgi_check_extension_1(server *srv, connection *con, void *p_d) {
	return fcgi_check_extension(srv, con, p_d, 1);
}

/* start request handler */
static handler_t fcgi_check_extension_2(server *srv, connection *con, void *p_d) {
	return fcgi_check_extension(srv, con, p_d, 0);
}


int mod_fastcgi_plugin_init(plugin *p);
int mod_fastcgi_plugin_init(plugin *p) {
	p->version      = LIGHTTPD_VERSION_ID;
	p->name         = buffer_init_string("fastcgi");

	p->init         = gw_init;
	p->cleanup      = gw_free;
	p->set_defaults = mod_fastcgi_set_defaults;
	p->connection_reset        = gw_connection_reset;
	p->handle_uri_clean        = fcgi_check_extension_1;
	p->handle_subrequest_start = fcgi_check_extension_2;
	p->handle_subrequest       = gw_handle_subrequest;
	p->handle_trigger          = gw_handle_trigger;
	p->handle_waitpid          = gw_handle_waitpid_cb;

	p->data         = NULL;

	return 0;
}
