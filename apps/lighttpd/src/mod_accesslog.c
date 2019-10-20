#include "first.h"

#include "base.h"
#include "fdevent.h"
#include "log.h"
#include "buffer.h"
#include "http_header.h"
#include "sock_addr.h"

#include "plugin.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#ifdef HAVE_SYSLOG_H
# include <syslog.h>
#endif

typedef struct {
	char key;
	enum {
		FORMAT_UNSET,
			FORMAT_UNSUPPORTED,
			FORMAT_PERCENT,
			FORMAT_REMOTE_HOST,
			FORMAT_REMOTE_IDENT,
			FORMAT_REMOTE_USER,
			FORMAT_TIMESTAMP,
			FORMAT_REQUEST_LINE,
			FORMAT_STATUS,
			FORMAT_BYTES_OUT_NO_HEADER,
			FORMAT_HEADER,

			FORMAT_REMOTE_ADDR,
			FORMAT_LOCAL_ADDR,
			FORMAT_COOKIE,
			FORMAT_TIME_USED_US,
			FORMAT_ENV,
			FORMAT_FILENAME,
			FORMAT_REQUEST_PROTOCOL,
			FORMAT_REQUEST_METHOD,
			FORMAT_SERVER_PORT,
			FORMAT_QUERY_STRING,
			FORMAT_TIME_USED,
			FORMAT_URL,
			FORMAT_SERVER_NAME,
			FORMAT_HTTP_HOST,
			FORMAT_CONNECTION_STATUS,
			FORMAT_BYTES_IN,
			FORMAT_BYTES_OUT,

			FORMAT_KEEPALIVE_COUNT,
			FORMAT_RESPONSE_HEADER,
			FORMAT_NOTE
	} type;
} format_mapping;

/**
 *
 *
 * "%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\""
 *
 */

static const format_mapping fmap[] =
{
	{ '%', FORMAT_PERCENT },
	{ 'h', FORMAT_REMOTE_HOST },
	{ 'l', FORMAT_REMOTE_IDENT },
	{ 'u', FORMAT_REMOTE_USER },
	{ 't', FORMAT_TIMESTAMP },
	{ 'r', FORMAT_REQUEST_LINE },
	{ 's', FORMAT_STATUS },
	{ 'b', FORMAT_BYTES_OUT_NO_HEADER },
	{ 'i', FORMAT_HEADER },

	{ 'a', FORMAT_REMOTE_ADDR },
	{ 'A', FORMAT_LOCAL_ADDR },
	{ 'B', FORMAT_BYTES_OUT_NO_HEADER },
	{ 'C', FORMAT_COOKIE },
	{ 'D', FORMAT_TIME_USED_US },
	{ 'e', FORMAT_ENV },
	{ 'f', FORMAT_FILENAME },
	{ 'H', FORMAT_REQUEST_PROTOCOL },
	{ 'k', FORMAT_KEEPALIVE_COUNT },
	{ 'm', FORMAT_REQUEST_METHOD },
	{ 'n', FORMAT_NOTE },
	{ 'p', FORMAT_SERVER_PORT },
	{ 'P', FORMAT_UNSUPPORTED }, /* we are only one process */
	{ 'q', FORMAT_QUERY_STRING },
	{ 'T', FORMAT_TIME_USED },
	{ 'U', FORMAT_URL }, /* w/o querystring */
	{ 'v', FORMAT_SERVER_NAME },
	{ 'V', FORMAT_HTTP_HOST },
	{ 'X', FORMAT_CONNECTION_STATUS },
	{ 'I', FORMAT_BYTES_IN },
	{ 'O', FORMAT_BYTES_OUT },

	{ 'o', FORMAT_RESPONSE_HEADER },

	{ '\0', FORMAT_UNSET }
};


enum e_optflags_time {
	/* format string is passed to strftime unless other format optflags set
	 * (besides FORMAT_FLAG_TIME_BEGIN or FORMAT_FLAG_TIME_END) */
	FORMAT_FLAG_TIME_END       = 0x00,/* use request end time (default) */
	FORMAT_FLAG_TIME_BEGIN     = 0x01,/* use request start time */
	FORMAT_FLAG_TIME_SEC       = 0x02,/* request time as num  sec since epoch */
	FORMAT_FLAG_TIME_MSEC      = 0x04,/* request time as num msec since epoch */
	FORMAT_FLAG_TIME_USEC      = 0x08,/* request time as num usec since epoch */
	FORMAT_FLAG_TIME_NSEC      = 0x10,/* request time as num nsec since epoch */
	FORMAT_FLAG_TIME_MSEC_FRAC = 0x20,/* request time msec fraction */
	FORMAT_FLAG_TIME_USEC_FRAC = 0x40,/* request time usec fraction */
	FORMAT_FLAG_TIME_NSEC_FRAC = 0x80 /* request time nsec fraction */
};

enum e_optflags_port {
	FORMAT_FLAG_PORT_LOCAL     = 0x01,/* (default) */
	FORMAT_FLAG_PORT_REMOTE    = 0x02
};


typedef struct {
	enum { FIELD_UNSET, FIELD_STRING, FIELD_FORMAT } type;

	buffer *string;
	int field;
	int opt;
} format_field;

typedef struct {
	format_field **ptr;

	size_t used;
	size_t size;
} format_fields;

typedef struct {
	buffer *access_logfile;
	int    log_access_fd;
	buffer *access_logbuffer; /* each logfile has a separate buffer */

	unsigned short use_syslog; /* syslog has global buffer */
	unsigned short syslog_level;

	buffer *format;

	time_t last_generated_accesslog_ts;
	time_t *last_generated_accesslog_ts_ptr;

	buffer *ts_accesslog_str;

	format_fields *parsed_format;
} plugin_config;

typedef struct {
	PLUGIN_DATA;

	plugin_config **config_storage;
	plugin_config conf;

	buffer *syslog_logbuffer; /* syslog has global buffer. no caching, always written directly */
} plugin_data;

INIT_FUNC(mod_accesslog_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));
	p->syslog_logbuffer = buffer_init();

	return p;
}

static void accesslog_write_all(server *srv, const buffer *filename, int fd, const void* buf, size_t count) {
	if (-1 == write_all(fd, buf, count)) {
		log_error_write(srv, __FILE__, __LINE__, "sbs",
			"writing access log entry failed:", filename, strerror(errno));
	}
}

static void accesslog_append_escaped(buffer *dest, buffer *str) {
	char *ptr, *start, *end;

	/* replaces non-printable chars with \xHH where HH is the hex representation of the byte */
	/* exceptions: " => \", \ => \\, whitespace chars => \n \t etc. */
	if (buffer_string_is_empty(str)) return;
	buffer_string_prepare_append(dest, buffer_string_length(str));

	for (ptr = start = str->ptr, end = str->ptr + buffer_string_length(str); ptr < end; ptr++) {
		unsigned char const c = (unsigned char) *ptr;
		if (c >= ' ' && c <= '~' && c != '"' && c != '\\') {
			/* nothing to change, add later as one block */
		} else {
			/* copy previous part */
			if (start < ptr) {
				buffer_append_string_len(dest, start, ptr - start);
			}
			start = ptr + 1;

			switch (c) {
			case '"':
				BUFFER_APPEND_STRING_CONST(dest, "\\\"");
				break;
			case '\\':
				BUFFER_APPEND_STRING_CONST(dest, "\\\\");
				break;
			case '\b':
				BUFFER_APPEND_STRING_CONST(dest, "\\b");
				break;
			case '\n':
				BUFFER_APPEND_STRING_CONST(dest, "\\n");
				break;
			case '\r':
				BUFFER_APPEND_STRING_CONST(dest, "\\r");
				break;
			case '\t':
				BUFFER_APPEND_STRING_CONST(dest, "\\t");
				break;
			case '\v':
				BUFFER_APPEND_STRING_CONST(dest, "\\v");
				break;
			default: {
					/* non printable char => \xHH */
					char hh[5] = {'\\','x',0,0,0};
					char h = c / 16;
					hh[2] = (h > 9) ? (h - 10 + 'A') : (h + '0');
					h = c % 16;
					hh[3] = (h > 9) ? (h - 10 + 'A') : (h + '0');
					buffer_append_string_len(dest, &hh[0], 4);
				}
				break;
			}
		}
	}

	if (start < end) {
		buffer_append_string_len(dest, start, end - start);
	}
}

static int accesslog_parse_format(server *srv, format_fields *fields, buffer *format) {
	size_t i, j, k = 0, start = 0;

	if (buffer_is_empty(format)) return -1;

	for (i = 0; i < buffer_string_length(format); i++) {
		switch(format->ptr[i]) {
		case '%':
			if (i > 0 && start != i) {
				/* copy the string before this % */
				if (fields->used == fields->size) {
					fields->size += 16;
					fields->ptr = realloc(fields->ptr, fields->size * sizeof(format_field * ));
				}

				fields->ptr[fields->used] = malloc(sizeof(format_field));
				fields->ptr[fields->used]->type = FIELD_STRING;
				fields->ptr[fields->used]->string = buffer_init();

				buffer_copy_string_len(fields->ptr[fields->used]->string, format->ptr + start, i - start);

				fields->used++;
			}

			/* we need a new field */

			if (fields->used == fields->size) {
				fields->size += 16;
				fields->ptr = realloc(fields->ptr, fields->size * sizeof(format_field * ));
			}

			/* search for the terminating command */
			switch (format->ptr[i+1]) {
			case '>':
			case '<':
				/* after the } has to be a character */
				if (format->ptr[i+2] == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "%< and %> have to be followed by a format-specifier");
					return -1;
				}


				for (j = 0; fmap[j].key != '\0'; j++) {
					if (fmap[j].key != format->ptr[i+2]) continue;

					/* found key */

					fields->ptr[fields->used] = malloc(sizeof(format_field));
					fields->ptr[fields->used]->type = FIELD_FORMAT;
					fields->ptr[fields->used]->field = fmap[j].type;
					fields->ptr[fields->used]->string = NULL;
					fields->ptr[fields->used]->opt = 0;

					fields->used++;

					break;
				}

				if (fmap[j].key == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "%< and %> have to be followed by a valid format-specifier");
					return -1;
				}

				start = i + 3;
				i = start - 1; /* skip the string */

				break;
			case '{':
				/* go forward to } */

				for (k = i+2; k < buffer_string_length(format); k++) {
					if (format->ptr[k] == '}') break;
				}

				if (k == buffer_string_length(format)) {
					log_error_write(srv, __FILE__, __LINE__, "s", "%{ has to be terminated by a }");
					return -1;
				}

				/* after the } has to be a character */
				if (format->ptr[k+1] == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "%{...} has to be followed by a format-specifier");
					return -1;
				}

				if (k == i + 2) {
					log_error_write(srv, __FILE__, __LINE__, "s", "%{...} has to contain a string");
					return -1;
				}

				for (j = 0; fmap[j].key != '\0'; j++) {
					if (fmap[j].key != format->ptr[k+1]) continue;

					/* found key */

					fields->ptr[fields->used] = malloc(sizeof(format_field));
					fields->ptr[fields->used]->type = FIELD_FORMAT;
					fields->ptr[fields->used]->field = fmap[j].type;
					fields->ptr[fields->used]->string = buffer_init();
					fields->ptr[fields->used]->opt = 0;

					buffer_copy_string_len(fields->ptr[fields->used]->string, format->ptr + i + 2, k - (i + 2));

					fields->used++;

					break;
				}

				if (fmap[j].key == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "%{...} has to be followed by a valid format-specifier");
					return -1;
				}

				start = k + 2;
				i = start - 1; /* skip the string */

				break;
			default:
				/* after the % has to be a character */
				if (format->ptr[i+1] == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "% has to be followed by a format-specifier");
					return -1;
				}

				for (j = 0; fmap[j].key != '\0'; j++) {
					if (fmap[j].key != format->ptr[i+1]) continue;

					/* found key */

					fields->ptr[fields->used] = malloc(sizeof(format_field));
					fields->ptr[fields->used]->type = FIELD_FORMAT;
					fields->ptr[fields->used]->field = fmap[j].type;
					fields->ptr[fields->used]->string = NULL;
					fields->ptr[fields->used]->opt = 0;

					fields->used++;

					break;
				}

				if (fmap[j].key == '\0') {
					log_error_write(srv, __FILE__, __LINE__, "s", "% has to be followed by a valid format-specifier");
					return -1;
				}

				start = i + 2;
				i = start - 1; /* skip the string */

				break;
			}

			break;
		}
	}

	if (start < i) {
		/* copy the string */
		if (fields->used == fields->size) {
			fields->size += 16;
			fields->ptr = realloc(fields->ptr, fields->size * sizeof(format_field * ));
		}

		fields->ptr[fields->used] = malloc(sizeof(format_field));
		fields->ptr[fields->used]->type = FIELD_STRING;
		fields->ptr[fields->used]->string = buffer_init();

		buffer_copy_string_len(fields->ptr[fields->used]->string, format->ptr + start, i - start);

		fields->used++;
	}

	return 0;
}

FREE_FUNC(mod_accesslog_free) {
	plugin_data *p = p_d;
	size_t i;

	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {

		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (NULL == s) continue;

			if (!buffer_string_is_empty(s->access_logbuffer)) {
				if (s->log_access_fd != -1) {
					accesslog_write_all(srv, s->access_logfile, s->log_access_fd, CONST_BUF_LEN(s->access_logbuffer));
				}
			}

			if (s->log_access_fd != -1) {
				if (buffer_string_is_empty(s->access_logfile) || *s->access_logfile->ptr != '|') {
					close(s->log_access_fd);
				} /*(else piped loggers closed in fdevent_close_logger_pipes())*/
			}

			buffer_free(s->ts_accesslog_str);
			buffer_free(s->access_logbuffer);
			buffer_free(s->format);
			buffer_free(s->access_logfile);

			if (s->parsed_format) {
				size_t j;
				for (j = 0; j < s->parsed_format->used; j++) {
					if (s->parsed_format->ptr[j]->string) buffer_free(s->parsed_format->ptr[j]->string);
					free(s->parsed_format->ptr[j]);
				}
				free(s->parsed_format->ptr);
				free(s->parsed_format);
			}

			free(s);
		}

		free(p->config_storage);
	}

	if (p->syslog_logbuffer) buffer_free(p->syslog_logbuffer);
	free(p);

	return HANDLER_GO_ON;
}

SETDEFAULTS_FUNC(log_access_open) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ "accesslog.filename",             NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },
		{ "accesslog.use-syslog",           NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION },
		{ "accesslog.format",               NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },
		{ "accesslog.syslog-level",         NULL, T_CONFIG_SHORT, T_CONFIG_SCOPE_CONNECTION },
		{ NULL,                             NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->access_logfile = buffer_init();
		s->format = buffer_init();
		s->access_logbuffer = buffer_init();
		s->ts_accesslog_str = buffer_init();
		s->log_access_fd = -1;
		s->last_generated_accesslog_ts = 0;
		s->last_generated_accesslog_ts_ptr = &(s->last_generated_accesslog_ts);
		s->syslog_level = LOG_INFO;


		cv[0].destination = s->access_logfile;
		cv[1].destination = &(s->use_syslog);
		cv[2].destination = s->format;
		cv[3].destination = &(s->syslog_level);

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		if (i == 0 && buffer_string_is_empty(s->format)) {
			/* set a default logfile string */

			buffer_copy_string_len(s->format, CONST_STR_LEN("%h %V %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-Agent}i\""));
		}

		/* parse */

		if (!buffer_is_empty(s->format)) {
			size_t j, tcount = 0;

			s->parsed_format = calloc(1, sizeof(*(s->parsed_format)));

			if (-1 == accesslog_parse_format(srv, s->parsed_format, s->format)) {

				log_error_write(srv, __FILE__, __LINE__, "sb",
						"parsing accesslog-definition failed:", s->format);

				return HANDLER_ERROR;
			}

			for (j = 0; j < s->parsed_format->used; ++j) {
				format_field * const f = s->parsed_format->ptr[j];
				if (FIELD_FORMAT != f->type) continue;
				if (FORMAT_TIMESTAMP == f->field) {
					if (!buffer_string_is_empty(f->string)) {
						const char *ptr = f->string->ptr;
						if (0 == strncmp(ptr, "begin:", sizeof("begin:")-1)) {
							f->opt |= FORMAT_FLAG_TIME_BEGIN;
							ptr += sizeof("begin:")-1;
						} else if (0 == strncmp(ptr, "end:", sizeof("end:")-1)) {
							f->opt |= FORMAT_FLAG_TIME_END;
							ptr += sizeof("end:")-1;
						}
						if      (0 == strcmp(ptr, "sec"))       f->opt |= FORMAT_FLAG_TIME_SEC;
						else if (0 == strcmp(ptr, "msec"))      f->opt |= FORMAT_FLAG_TIME_MSEC;
						else if (0 == strcmp(ptr, "usec"))      f->opt |= FORMAT_FLAG_TIME_USEC;
						else if (0 == strcmp(ptr, "nsec"))      f->opt |= FORMAT_FLAG_TIME_NSEC;
						else if (0 == strcmp(ptr, "msec_frac")) f->opt |= FORMAT_FLAG_TIME_MSEC_FRAC;
						else if (0 == strcmp(ptr, "usec_frac")) f->opt |= FORMAT_FLAG_TIME_USEC_FRAC;
						else if (0 == strcmp(ptr, "nsec_frac")) f->opt |= FORMAT_FLAG_TIME_NSEC_FRAC;
						else if (NULL == strchr(ptr, '%')) {
							log_error_write(srv, __FILE__, __LINE__, "sb",
								"constant string for time format (misspelled token? or missing '%'):", s->format);

							return HANDLER_ERROR;
						}
					}

					/* make sure they didn't try to send the timestamp in twice
					 * (would invalidate s->ts_accesslog_str cache of timestamp str) */
					if (!(f->opt & ~(FORMAT_FLAG_TIME_BEGIN|FORMAT_FLAG_TIME_END|FORMAT_FLAG_TIME_SEC)) && ++tcount > 1) {
						log_error_write(srv, __FILE__, __LINE__, "sb",
							"you may not use strftime timestamp format %{}t twice in the same access log:", s->format);

						return HANDLER_ERROR;
					}

					if (f->opt & FORMAT_FLAG_TIME_BEGIN) srv->srvconf.high_precision_timestamps = 1;
				} else if (FORMAT_TIME_USED_US == f->field) {
					f->opt |= FORMAT_FLAG_TIME_USEC;
					srv->srvconf.high_precision_timestamps = 1;
				} else if (FORMAT_TIME_USED == f->field) {
					if (buffer_string_is_empty(f->string)
					      || buffer_is_equal_string(f->string, CONST_STR_LEN("s"))
					      || buffer_is_equal_string(f->string, CONST_STR_LEN("sec")))  f->opt |= FORMAT_FLAG_TIME_SEC;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("ms"))
					      || buffer_is_equal_string(f->string, CONST_STR_LEN("msec"))) f->opt |= FORMAT_FLAG_TIME_MSEC;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("us"))
					      || buffer_is_equal_string(f->string, CONST_STR_LEN("usec"))) f->opt |= FORMAT_FLAG_TIME_USEC;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("ns"))
					      || buffer_is_equal_string(f->string, CONST_STR_LEN("nsec"))) f->opt |= FORMAT_FLAG_TIME_NSEC;
					else {
						log_error_write(srv, __FILE__, __LINE__, "sb",
							"invalid time unit in %{UNIT}T:", s->format);

						return HANDLER_ERROR;
					}

					if (f->opt & ~(FORMAT_FLAG_TIME_SEC)) srv->srvconf.high_precision_timestamps = 1;
				} else if (FORMAT_COOKIE == f->field) {
					if (buffer_string_is_empty(f->string)) f->type = FIELD_STRING; /*(blank)*/
				} else if (FORMAT_SERVER_PORT == f->field) {
					if (buffer_string_is_empty(f->string))
						f->opt |= FORMAT_FLAG_PORT_LOCAL;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("canonical")))
						f->opt |= FORMAT_FLAG_PORT_LOCAL;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("local")))
						f->opt |= FORMAT_FLAG_PORT_LOCAL;
					else if (buffer_is_equal_string(f->string, CONST_STR_LEN("remote")))
						f->opt |= FORMAT_FLAG_PORT_REMOTE;
					else {
						log_error_write(srv, __FILE__, __LINE__, "sb",
								"invalid format %{canonical,local,remote}p:", s->format);

						return HANDLER_ERROR;
					}
				}
			}

#if 0
			/* debugging */
			for (j = 0; j < s->parsed_format->used; j++) {
				switch (s->parsed_format->ptr[j]->type) {
				case FIELD_FORMAT:
					log_error_write(srv, __FILE__, __LINE__, "ssds",
							"config:", "format", s->parsed_format->ptr[j]->field,
							s->parsed_format->ptr[j]->string ?
							s->parsed_format->ptr[j]->string->ptr : "" );
					break;
				case FIELD_STRING:
					log_error_write(srv, __FILE__, __LINE__, "ssbs", "config:", "string '", s->parsed_format->ptr[j]->string, "'");
					break;
				default:
					break;
				}
			}
#endif
		}

		if (s->use_syslog) {
			/* ignore the next checks */
			continue;
		}

		if (buffer_string_is_empty(s->access_logfile)) continue;

		if (srv->srvconf.preflight_check) continue;

		if (-1 == (s->log_access_fd = fdevent_open_logger(s->access_logfile->ptr))) {
			log_error_write(srv, __FILE__, __LINE__, "SBSS",
					"opening log '", s->access_logfile,
					"' failed: ", strerror(errno));
			return HANDLER_ERROR;
		}
	}

	return HANDLER_GO_ON;
}

static void log_access_flush(server *srv, void *p_d) {
	plugin_data *p = p_d;
	size_t i;

	for (i = 0; i < srv->config_context->used; i++) {
		plugin_config *s = p->config_storage[i];

		if (!buffer_string_is_empty(s->access_logbuffer)) {
			if (s->log_access_fd != -1) {
				accesslog_write_all(srv, s->access_logfile, s->log_access_fd, CONST_BUF_LEN(s->access_logbuffer));
			}

			buffer_clear(s->access_logbuffer);
		}
	}
}

TRIGGER_FUNC(log_access_periodic_flush) {
	/* flush buffered access logs every 4 seconds */
	if (0 == (srv->cur_ts & 3)) log_access_flush(srv, p_d);
	return HANDLER_GO_ON;
}

SIGHUP_FUNC(log_access_cycle) {
	plugin_data *p = p_d;
	size_t i;

	if (!p->config_storage) return HANDLER_GO_ON;

	log_access_flush(srv, p);

	for (i = 0; i < srv->config_context->used; i++) {
		plugin_config *s = p->config_storage[i];
		if (s->use_syslog == 0
			&& !buffer_string_is_empty(s->access_logfile)
			&& s->access_logfile->ptr[0] != '|') {

			if (-1 == fdevent_cycle_logger(s->access_logfile->ptr, &s->log_access_fd)) {
				log_error_write(srv, __FILE__, __LINE__, "ss", "cycling access-log failed:", strerror(errno));
				return HANDLER_ERROR;
			}
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_accesslog_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(access_logfile);
	PATCH(log_access_fd);
	PATCH(last_generated_accesslog_ts_ptr);
	PATCH(access_logbuffer);
	PATCH(ts_accesslog_str);
	PATCH(parsed_format);
	PATCH(use_syslog);
	PATCH(syslog_level);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("accesslog.filename"))) {
				PATCH(access_logfile);
				PATCH(log_access_fd);
				PATCH(access_logbuffer);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("accesslog.format"))) {
				PATCH(parsed_format);
				PATCH(last_generated_accesslog_ts_ptr);
				PATCH(ts_accesslog_str);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("accesslog.use-syslog"))) {
				PATCH(use_syslog);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("accesslog.syslog-level"))) {
				PATCH(syslog_level);
			}
		}
	}

	return 0;
}
#undef PATCH

REQUESTDONE_FUNC(log_access_write) {
	plugin_data *p = p_d;
	buffer *b;
	size_t j;

	int newts = 0;
	buffer *vb;
	struct timespec ts = { 0, 0 };

	mod_accesslog_patch_connection(srv, con, p);

	/* No output device, nothing to do */
	if (!p->conf.use_syslog && p->conf.log_access_fd == -1) return HANDLER_GO_ON;

	if (p->conf.use_syslog) {
		b = p->syslog_logbuffer;
	} else {
		b = p->conf.access_logbuffer;
	}

	for (j = 0; j < p->conf.parsed_format->used; j++) {
		const format_field * const f = p->conf.parsed_format->ptr[j];
		switch(f->type) {
		case FIELD_STRING:
			buffer_append_string_buffer(b, f->string);
			break;
		case FIELD_FORMAT:
			switch(f->field) {
			case FORMAT_TIMESTAMP:

				if (f->opt & ~(FORMAT_FLAG_TIME_BEGIN|FORMAT_FLAG_TIME_END)) {
					if (f->opt & FORMAT_FLAG_TIME_SEC) {
						time_t t = (!(f->opt & FORMAT_FLAG_TIME_BEGIN)) ? srv->cur_ts : con->request_start;
						buffer_append_int(b, (intmax_t)t);
					} else if (f->opt & (FORMAT_FLAG_TIME_MSEC|FORMAT_FLAG_TIME_USEC|FORMAT_FLAG_TIME_NSEC)) {
						off_t t; /*(expected to be 64-bit since large file support enabled)*/
						long ns;
						if (!(f->opt & FORMAT_FLAG_TIME_BEGIN)) {
							if (0 == ts.tv_sec) log_clock_gettime_realtime(&ts);
							t = (off_t)ts.tv_sec;
							ns = ts.tv_nsec;
						} else {
							t = (off_t)con->request_start_hp.tv_sec;
							ns = con->request_start_hp.tv_nsec;
						}
						if (f->opt & FORMAT_FLAG_TIME_MSEC) {
							t *= 1000;
							t += (ns + 999999) / 1000000; /* ceil */
						} else if (f->opt & FORMAT_FLAG_TIME_USEC) {
							t *= 1000000;
							t += (ns + 999) / 1000; /* ceil */
						} else {/*(f->opt & FORMAT_FLAG_TIME_NSEC)*/
							t *= 1000000000;
							t += ns;
						}
						buffer_append_int(b, (intmax_t)t);
					} else { /*(FORMAT_FLAG_TIME_MSEC_FRAC|FORMAT_FLAG_TIME_USEC_FRAC|FORMAT_FLAG_TIME_NSEC_FRAC)*/
						long ns;
						char *ptr;
						if (!(f->opt & FORMAT_FLAG_TIME_BEGIN)) {
							if (0 == ts.tv_sec) log_clock_gettime_realtime(&ts);
							ns = ts.tv_nsec;
						} else {
							ns = con->request_start_hp.tv_nsec;
						}
						/*assert(t < 1000000000);*/
						if (f->opt & FORMAT_FLAG_TIME_MSEC_FRAC) {
							ns +=  999999; /* ceil */
							ns /= 1000000;
							buffer_append_string_len(b, CONST_STR_LEN("000"));
						} else if (f->opt & FORMAT_FLAG_TIME_USEC_FRAC) {
							ns +=  999; /* ceil */
							ns /= 1000;
							buffer_append_string_len(b, CONST_STR_LEN("000000"));
						} else {/*(f->opt & FORMAT_FLAG_TIME_NSEC_FRAC)*/
							buffer_append_string_len(b, CONST_STR_LEN("000000000"));
						}
						for (ptr = b->ptr + buffer_string_length(b); ns > 0; ns /= 10)
							*--ptr = (ns % 10) + '0';
					}
				} else if (!(f->opt & FORMAT_FLAG_TIME_BEGIN) && srv->cur_ts == *(p->conf.last_generated_accesslog_ts_ptr)) {
					buffer_append_string_buffer(b, p->conf.ts_accesslog_str);
				} else {
					/* cache the generated timestamp (only if ! FORMAT_FLAG_TIME_BEGIN) */
					struct tm *tmptr;
					time_t t;
				      #if defined(HAVE_STRUCT_TM_GMTOFF)
				      # ifdef HAVE_LOCALTIME_R
					struct tm tm;
				      # endif /* HAVE_LOCALTIME_R */
				      #else /* HAVE_STRUCT_TM_GMTOFF */
				      # ifdef HAVE_GMTIME_R
					struct tm tm;
				      # endif /* HAVE_GMTIME_R */
				      #endif /* HAVE_STRUCT_TM_GMTOFF */

					if (!(f->opt & FORMAT_FLAG_TIME_BEGIN)) {
						t = *(p->conf.last_generated_accesslog_ts_ptr) = srv->cur_ts;
						newts = 1;
					} else {
						t = con->request_start;
					}

				      #if defined(HAVE_STRUCT_TM_GMTOFF)
				      # ifdef HAVE_LOCALTIME_R
					tmptr = localtime_r(&t, &tm);
				      # else /* HAVE_LOCALTIME_R */
					tmptr = localtime(&t);
				      # endif /* HAVE_LOCALTIME_R */
				      #else /* HAVE_STRUCT_TM_GMTOFF */
				      # ifdef HAVE_GMTIME_R
					tmptr = gmtime_r(&t, &tm);
				      # else /* HAVE_GMTIME_R */
					tmptr = gmtime(&t);
				      # endif /* HAVE_GMTIME_R */
				      #endif /* HAVE_STRUCT_TM_GMTOFF */

					buffer_clear(p->conf.ts_accesslog_str);

					if (buffer_string_is_empty(f->string)) {
					      #if defined(HAVE_STRUCT_TM_GMTOFF)
						long scd, hrs, min;
						buffer_append_strftime(p->conf.ts_accesslog_str, "[%d/%b/%Y:%H:%M:%S ", tmptr);
						buffer_append_string_len(p->conf.ts_accesslog_str, tmptr->tm_gmtoff >= 0 ? "+" : "-", 1);

						scd = labs(tmptr->tm_gmtoff);
						hrs = scd / 3600;
						min = (scd % 3600) / 60;

						/* hours */
						if (hrs < 10) buffer_append_string_len(p->conf.ts_accesslog_str, CONST_STR_LEN("0"));
						buffer_append_int(p->conf.ts_accesslog_str, hrs);

						if (min < 10) buffer_append_string_len(p->conf.ts_accesslog_str, CONST_STR_LEN("0"));
						buffer_append_int(p->conf.ts_accesslog_str, min);
						buffer_append_string_len(p->conf.ts_accesslog_str, CONST_STR_LEN("]"));
					      #else
						buffer_append_strftime(p->conf.ts_accesslog_str, "[%d/%b/%Y:%H:%M:%S +0000]", tmptr);
					      #endif /* HAVE_STRUCT_TM_GMTOFF */
					} else {
						buffer_append_strftime(p->conf.ts_accesslog_str, f->string->ptr, tmptr);
					}

					buffer_append_string_buffer(b, p->conf.ts_accesslog_str);
				}
				break;
			case FORMAT_TIME_USED:
			case FORMAT_TIME_USED_US:
				if (f->opt & FORMAT_FLAG_TIME_SEC) {
					buffer_append_int(b, srv->cur_ts - con->request_start);
				} else {
					const struct timespec * const bs = &con->request_start_hp;
					off_t tdiff; /*(expected to be 64-bit since large file support enabled)*/
					if (0 == ts.tv_sec) log_clock_gettime_realtime(&ts);
					tdiff = (off_t)(ts.tv_sec - bs->tv_sec)*1000000000 + (ts.tv_nsec - bs->tv_nsec);
					if (tdiff <= 0) {
						/* sanity check for time moving backwards
						 * (daylight savings adjustment or leap seconds or ?) */
						tdiff  = -1;
					} else if (f->opt & FORMAT_FLAG_TIME_MSEC) {
						tdiff +=  999999; /* ceil */
						tdiff /= 1000000;
					} else if (f->opt & FORMAT_FLAG_TIME_USEC) {
						tdiff +=  999; /* ceil */
						tdiff /= 1000;
					} /* else (f->opt & FORMAT_FLAG_TIME_NSEC) */
					buffer_append_int(b, (intmax_t)tdiff);
				}
				break;
			case FORMAT_REMOTE_ADDR:
			case FORMAT_REMOTE_HOST:
				buffer_append_string_buffer(b, con->dst_addr_buf);
				break;
			case FORMAT_REMOTE_IDENT:
				/* ident */
				buffer_append_string_len(b, CONST_STR_LEN("-"));
				break;
			case FORMAT_REMOTE_USER:
				if (NULL != (vb = http_header_env_get(con, CONST_STR_LEN("REMOTE_USER")))) {
					accesslog_append_escaped(b, vb);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_REQUEST_LINE:
				/*(attempt to reconstruct request line)*/
				buffer_append_string(b, get_http_method_name(con->request.http_method));
				buffer_append_string_len(b, CONST_STR_LEN(" "));
				accesslog_append_escaped(b, con->request.orig_uri);
				buffer_append_string_len(b, CONST_STR_LEN(" "));
				buffer_append_string(b, get_http_version_name(con->request.http_version));
				break;
			case FORMAT_STATUS:
				buffer_append_int(b, con->http_status);
				break;

			case FORMAT_BYTES_OUT_NO_HEADER:
				if (con->bytes_written > 0) {
					buffer_append_int(b,
							    con->bytes_written - con->bytes_header <= 0 ? 0 : con->bytes_written - con->bytes_header);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_HEADER:
				if (NULL != (vb = http_header_request_get(con, HTTP_HEADER_UNSPECIFIED, CONST_BUF_LEN(f->string)))) {
					accesslog_append_escaped(b, vb);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_RESPONSE_HEADER:
				if (NULL != (vb = http_header_response_get(con, HTTP_HEADER_UNSPECIFIED, CONST_BUF_LEN(f->string)))) {
					accesslog_append_escaped(b, vb);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_ENV:
			case FORMAT_NOTE:
				if (NULL != (vb = http_header_env_get(con, CONST_BUF_LEN(f->string)))) {
					accesslog_append_escaped(b, vb);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_FILENAME:
				if (!buffer_string_is_empty(con->physical.path)) {
					buffer_append_string_buffer(b, con->physical.path);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_BYTES_OUT:
				if (con->bytes_written > 0) {
					buffer_append_int(b, con->bytes_written);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_BYTES_IN:
				if (con->bytes_read > 0) {
					buffer_append_int(b, con->bytes_read);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_SERVER_NAME:
				if (!buffer_string_is_empty(con->server_name)) {
					buffer_append_string_buffer(b, con->server_name);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_HTTP_HOST:
				if (!buffer_string_is_empty(con->uri.authority)) {
					accesslog_append_escaped(b, con->uri.authority);
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("-"));
				}
				break;
			case FORMAT_REQUEST_PROTOCOL:
				buffer_append_string_len(b,
					con->request.http_version == HTTP_VERSION_1_1 ? "HTTP/1.1" : "HTTP/1.0", 8);
				break;
			case FORMAT_REQUEST_METHOD:
				http_method_append(b, con->request.http_method);
				break;
			case FORMAT_PERCENT:
				buffer_append_string_len(b, CONST_STR_LEN("%"));
				break;
			case FORMAT_LOCAL_ADDR:
				{
					/* (perf: not using getsockname() and inet_ntop_cache_get_ip())
					 * (still useful if admin has configured explicit listen IPs) */
					const char *colon;
					buffer *srvtoken = con->srv_socket->srv_token;
					if (srvtoken->ptr[0] == '[') {
						colon = strstr(srvtoken->ptr, "]:");
					} else {
						colon = strchr(srvtoken->ptr, ':');
					}
					if (colon) {
						buffer_append_string_len(b, srvtoken->ptr, (size_t)(colon - srvtoken->ptr));
					} else {
						buffer_append_string_buffer(b, srvtoken);
					}
				}
				break;
			case FORMAT_SERVER_PORT:
				if (f->opt & FORMAT_FLAG_PORT_REMOTE) {
					buffer_append_int(b, sock_addr_get_port(&con->dst_addr));
				} else { /* if (f->opt & FORMAT_FLAG_PORT_LOCAL) *//*(default)*/
					const char *colon;
					buffer *srvtoken = ((server_socket*)(con->srv_socket))->srv_token;
					if (srvtoken->ptr[0] == '[') {
						colon = strstr(srvtoken->ptr, "]:");
					} else {
						colon = strchr(srvtoken->ptr, ':');
					}
					if (colon) {
						buffer_append_string(b, colon+1);
					} else {
						buffer_append_int(b, srv->srvconf.port);
					}
				}
				break;
			case FORMAT_QUERY_STRING:
				accesslog_append_escaped(b, con->uri.query);
				break;
			case FORMAT_URL:
				accesslog_append_escaped(b, con->uri.path_raw);
				break;
			case FORMAT_CONNECTION_STATUS:
				if (con->state == CON_STATE_RESPONSE_END) {
					if (0 == con->keep_alive) {
						buffer_append_string_len(b, CONST_STR_LEN("-"));
					} else {
						buffer_append_string_len(b, CONST_STR_LEN("+"));
					}
				} else { /* CON_STATE_ERROR */
					buffer_append_string_len(b, CONST_STR_LEN("X"));
				}
				break;
			case FORMAT_KEEPALIVE_COUNT:
				if (con->request_count > 1) {
					buffer_append_int(b, (intmax_t)(con->request_count-1));
				} else {
					buffer_append_string_len(b, CONST_STR_LEN("0"));
				}
				break;
			case FORMAT_COOKIE:
				if (NULL != (vb = http_header_request_get(con, HTTP_HEADER_COOKIE, CONST_STR_LEN("Cookie")))) {
					char *str = vb->ptr;
					size_t len = buffer_string_length(f->string);
					do {
						while (*str == ' ' || *str == '\t') ++str;
						if (0 == strncmp(str, f->string->ptr, len) && str[len] == '=') {
							char *v = str+len+1;
							buffer *bstr;
							for (str = v; *str != '\0' && *str != ';'; ++str) ;
							if (str == v) break;
							do { --str; } while (str > v && (*str == ' ' || *str == '\t'));
							bstr = buffer_init();
							buffer_copy_string_len(bstr, v, str - v + 1);
							accesslog_append_escaped(b, bstr);
							buffer_free(bstr);
							break;
						} else {
							do { ++str; } while (*str != ' ' && *str != '\t' && *str != '\0');
						}
						while (*str == ' ' || *str == '\t') ++str;
					} while (*str++ == ';');
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	if (p->conf.use_syslog) { /* syslog doesn't cache */
#ifdef HAVE_SYSLOG_H
		if (!buffer_string_is_empty(b)) {
			/*(syslog appends a \n on its own)*/
			syslog(p->conf.syslog_level, "%s", b->ptr);
		}
#endif
		buffer_clear(b);
		return HANDLER_GO_ON;
	}

	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	if ((!buffer_string_is_empty(p->conf.access_logfile) && p->conf.access_logfile->ptr[0] == '|') || /* pipes don't cache */
	    newts ||
	    buffer_string_length(b) >= BUFFER_MAX_REUSE_SIZE) {
		if (p->conf.log_access_fd >= 0) {
			accesslog_write_all(srv, p->conf.access_logfile, p->conf.log_access_fd, CONST_BUF_LEN(b));
		}
		buffer_clear(b);
	}

	return HANDLER_GO_ON;
}


int mod_accesslog_plugin_init(plugin *p);
int mod_accesslog_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("accesslog");

	p->init        = mod_accesslog_init;
	p->set_defaults= log_access_open;
	p->cleanup     = mod_accesslog_free;

	p->handle_request_done  = log_access_write;
	p->handle_trigger       = log_access_periodic_flush;
	p->handle_sighup        = log_access_cycle;

	p->data        = NULL;

	return 0;
}
