#include "first.h"

#include "base.h"
#include "connections.h"
#include "fdevent.h"
#include "http_header.h"
#include "log.h"

#include "plugin.h"

#include <sys/types.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>

typedef struct {
	buffer *config_url;
	buffer *status_url;
	buffer *statistics_url;

	int     sort;
} plugin_config;

typedef struct {
	PLUGIN_DATA;

	double traffic_out;
	double requests;

	double mod_5s_traffic_out[5];
	double mod_5s_requests[5];
	size_t mod_5s_ndx;

	double rel_traffic_out;
	double rel_requests;

	double abs_traffic_out;
	double abs_requests;

	double bytes_written;

	buffer *module_list;

	plugin_config **config_storage;

	plugin_config conf;
} plugin_data;

INIT_FUNC(mod_status_init) {
	plugin_data *p;
	size_t i;

	p = calloc(1, sizeof(*p));

	p->traffic_out = p->requests = 0;
	p->rel_traffic_out = p->rel_requests = 0;
	p->abs_traffic_out = p->abs_requests = 0;
	p->bytes_written = 0;
	p->module_list = buffer_init();

	for (i = 0; i < 5; i++) {
		p->mod_5s_traffic_out[i] = p->mod_5s_requests[i] = 0;
	}

	return p;
}

FREE_FUNC(mod_status_free) {
	plugin_data *p = p_d;

	UNUSED(srv);

	if (!p) return HANDLER_GO_ON;

	buffer_free(p->module_list);

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];
			if (NULL == s) continue;

			buffer_free(s->status_url);
			buffer_free(s->statistics_url);
			buffer_free(s->config_url);

			free(s);
		}
		free(p->config_storage);
	}


	free(p);

	return HANDLER_GO_ON;
}

SETDEFAULTS_FUNC(mod_status_set_defaults) {
	plugin_data *p = p_d;
	size_t i;

	config_values_t cv[] = {
		{ "status.status-url",           NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },
		{ "status.config-url",           NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },
		{ "status.enable-sort",          NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION },
		{ "status.statistics-url",       NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },
		{ NULL,                          NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->config_url    = buffer_init();
		s->status_url    = buffer_init();
		s->sort          = 1;
		s->statistics_url    = buffer_init();

		cv[0].destination = s->status_url;
		cv[1].destination = s->config_url;
		cv[2].destination = &(s->sort);
		cv[3].destination = s->statistics_url;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}
	}

	return HANDLER_GO_ON;
}



static int mod_status_row_append(buffer *b, const char *key, const char *value) {
	buffer_append_string_len(b, CONST_STR_LEN("   <tr>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("    <td><b>"));
	buffer_append_string(b, key);
	buffer_append_string_len(b, CONST_STR_LEN("</b></td>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("    <td>"));
	buffer_append_string(b, value);
	buffer_append_string_len(b, CONST_STR_LEN("</td>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("   </tr>\n"));

	return 0;
}

static int mod_status_header_append(buffer *b, const char *key) {
	buffer_append_string_len(b, CONST_STR_LEN("   <tr>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("    <th colspan=\"2\">"));
	buffer_append_string(b, key);
	buffer_append_string_len(b, CONST_STR_LEN("</th>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("   </tr>\n"));

	return 0;
}

static int mod_status_header_append_sort(buffer *b, void *p_d, const char* key) {
	plugin_data *p = p_d;

	if (p->conf.sort) {
		buffer_append_string_len(b, CONST_STR_LEN("<th class=\"status\"><a href=\"#\" class=\"sortheader\" onclick=\"resort(this);return false;\">"));
		buffer_append_string(b, key);
		buffer_append_string_len(b, CONST_STR_LEN("<span class=\"sortarrow\">:</span></a></th>\n"));
	} else {
		buffer_append_string_len(b, CONST_STR_LEN("<th class=\"status\">"));
		buffer_append_string(b, key);
		buffer_append_string_len(b, CONST_STR_LEN("</th>\n"));
	}

	return 0;
}

static int mod_status_get_multiplier(double *avg, char *multiplier, int size) {
	*multiplier = ' ';

	if (*avg > size) { *avg /= size; *multiplier = 'k'; }
	if (*avg > size) { *avg /= size; *multiplier = 'M'; }
	if (*avg > size) { *avg /= size; *multiplier = 'G'; }
	if (*avg > size) { *avg /= size; *multiplier = 'T'; }
	if (*avg > size) { *avg /= size; *multiplier = 'P'; }
	if (*avg > size) { *avg /= size; *multiplier = 'E'; }
	if (*avg > size) { *avg /= size; *multiplier = 'Z'; }
	if (*avg > size) { *avg /= size; *multiplier = 'Y'; }

	return 0;
}

static handler_t mod_status_handle_server_status_html(server *srv, connection *con, void *p_d) {
	plugin_data *p = p_d;
	buffer *b = chunkqueue_append_buffer_open(con->write_queue);
	size_t j;
	double avg;
	char multiplier = '\0';
	char buf[32];
	time_t ts;

	int days, hours, mins, seconds;

	/*(CON_STATE_CLOSE must be last state in enum connection_state_t)*/
	int cstates[CON_STATE_CLOSE+3];
	memset(cstates, 0, sizeof(cstates));

	buffer_copy_string_len(b, CONST_STR_LEN(
				 "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
				 "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
				 "         \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
				 "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
				 " <head>\n"
				 "  <title>Status</title>\n"

				   "  <style type=\"text/css\">\n"
				   "    table.status { border: black solid thin; }\n"
				   "    td { white-space: nowrap; }\n"
				   "    td.int { background-color: #f0f0f0; text-align: right }\n"
				   "    td.string { background-color: #f0f0f0; text-align: left }\n"
				   "    th.status { background-color: black; color: white; font-weight: bold; }\n"
				   "    a.sortheader { background-color: black; color: white; font-weight: bold; text-decoration: none; display: block; }\n"
				   "    span.sortarrow { color: white; text-decoration: none; }\n"
				   "  </style>\n"));

	if (!buffer_string_is_empty(con->uri.query) && 0 == memcmp(con->uri.query->ptr, CONST_STR_LEN("refresh="))) {
		/* Note: Refresh is an historical, but non-standard HTTP header
		 * References (meta http-equiv="refresh" use is deprecated):
		 *   https://www.w3.org/TR/WCAG10-HTML-TECHS/#meta-element
		 *   https://www.w3.org/TR/WCAG10-CORE-TECHS/#auto-page-refresh
		 *   https://www.w3.org/QA/Tips/reback
		 */
		const long refresh = strtol(con->uri.query->ptr+sizeof("refresh=")-1, NULL, 10);
		if (refresh > 0) {
			buffer_append_string_len(b, CONST_STR_LEN("<meta http-equiv=\"refresh\" content=\""));
			buffer_append_int(b, refresh < 604800 ? refresh : 604800);
			buffer_append_string_len(b, CONST_STR_LEN("\">\n"));
		}
	}

	if (p->conf.sort) {
		buffer_append_string_len(b, CONST_STR_LEN(
					   "<script type=\"text/javascript\">\n"
					   "// <!--\n"
					   "var sort_column;\n"
					   "var prev_span = null;\n"

					   "function get_inner_text(el) {\n"
					   " if((typeof el == 'string')||(typeof el == 'undefined'))\n"
					   "  return el;\n"
					   " if(el.innerText)\n"
					   "  return el.innerText;\n"
					   " else {\n"
					   "  var str = \"\";\n"
					   "  var cs = el.childNodes;\n"
					   "  var l = cs.length;\n"
					   "  for (i=0;i<l;i++) {\n"
					   "   if (cs[i].nodeType==1) str += get_inner_text(cs[i]);\n"
					   "   else if (cs[i].nodeType==3) str += cs[i].nodeValue;\n"
					   "  }\n"
					   " }\n"
					   " return str;\n"
					   "}\n"

					   "function sortfn(a,b) {\n"
					   " var at = get_inner_text(a.cells[sort_column]);\n"
					   " var bt = get_inner_text(b.cells[sort_column]);\n"
					   " if (a.cells[sort_column].className == 'int') {\n"
					   "  return parseInt(at)-parseInt(bt);\n"
					   " } else {\n"
					   "  aa = at.toLowerCase();\n"
					   "  bb = bt.toLowerCase();\n"
					   "  if (aa==bb) return 0;\n"
					   "  else if (aa<bb) return -1;\n"
					   "  else return 1;\n"
					   " }\n"
					   "}\n"

					   "function resort(lnk) {\n"
					   " var span = lnk.childNodes[1];\n"
					   " var table = lnk.parentNode.parentNode.parentNode.parentNode;\n"
					   " var rows = new Array();\n"
					   " for (j=1;j<table.rows.length;j++)\n"
					   "  rows[j-1] = table.rows[j];\n"
					   " sort_column = lnk.parentNode.cellIndex;\n"
					   " rows.sort(sortfn);\n"

					   " if (prev_span != null) prev_span.innerHTML = '';\n"
					   " if (span.getAttribute('sortdir')=='down') {\n"
					   "  span.innerHTML = '&uarr;';\n"
					   "  span.setAttribute('sortdir','up');\n"
					   "  rows.reverse();\n"
					   " } else {\n"
					   "  span.innerHTML = '&darr;';\n"
					   "  span.setAttribute('sortdir','down');\n"
					   " }\n"
					   " for (i=0;i<rows.length;i++)\n"
					   "  table.tBodies[0].appendChild(rows[i]);\n"
					   " prev_span = span;\n"
					   "}\n"
					   "// -->\n"
					   "</script>\n"));
	}

	buffer_append_string_len(b, CONST_STR_LEN(
				 " </head>\n"
				 " <body>\n"));



	/* connection listing */
	buffer_append_string_len(b, CONST_STR_LEN("<h1>Server-Status ("));
	buffer_append_string_buffer(b, con->conf.server_tag);
	buffer_append_string_len(b, CONST_STR_LEN(")</h1>"));

	buffer_append_string_len(b, CONST_STR_LEN("<table summary=\"status\" class=\"status\">"));
	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Hostname</td><td class=\"string\">"));
	buffer_append_string_buffer(b, con->uri.authority);
	buffer_append_string_len(b, CONST_STR_LEN(" ("));
	buffer_append_string_buffer(b, con->server_name);
	buffer_append_string_len(b, CONST_STR_LEN(")</td></tr>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Uptime</td><td class=\"string\">"));

	ts = srv->cur_ts - srv->startup_ts;

	days = ts / (60 * 60 * 24);
	ts %= (60 * 60 * 24);

	hours = ts / (60 * 60);
	ts %= (60 * 60);

	mins = ts / (60);
	ts %= (60);

	seconds = ts;

	if (days) {
		buffer_append_int(b, days);
		buffer_append_string_len(b, CONST_STR_LEN(" days "));
	}

	if (hours) {
		buffer_append_int(b, hours);
		buffer_append_string_len(b, CONST_STR_LEN(" hours "));
	}

	if (mins) {
		buffer_append_int(b, mins);
		buffer_append_string_len(b, CONST_STR_LEN(" min "));
	}

	buffer_append_int(b, seconds);
	buffer_append_string_len(b, CONST_STR_LEN(" s"));

	buffer_append_string_len(b, CONST_STR_LEN("</td></tr>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Started at</td><td class=\"string\">"));

	ts = srv->startup_ts;

	strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&ts));
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN("</td></tr>\n"));


	buffer_append_string_len(b, CONST_STR_LEN("<tr><th colspan=\"2\">absolute (since start)</th></tr>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Requests</td><td class=\"string\">"));
	avg = p->abs_requests;

	mod_status_get_multiplier(&avg, &multiplier, 1000);

	buffer_append_int(b, avg);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);
	buffer_append_string_len(b, CONST_STR_LEN("req</td></tr>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Traffic</td><td class=\"string\">"));
	avg = p->abs_traffic_out;

	mod_status_get_multiplier(&avg, &multiplier, 1024);

	snprintf(buf, sizeof(buf), "%.2f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);
	buffer_append_string_len(b, CONST_STR_LEN("byte</td></tr>\n"));



	buffer_append_string_len(b, CONST_STR_LEN("<tr><th colspan=\"2\">average (since start)</th></tr>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Requests</td><td class=\"string\">"));
	avg = p->abs_requests / (srv->cur_ts - srv->startup_ts);

	mod_status_get_multiplier(&avg, &multiplier, 1000);

	buffer_append_int(b, avg);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);
	buffer_append_string_len(b, CONST_STR_LEN("req/s</td></tr>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Traffic</td><td class=\"string\">"));
	avg = p->abs_traffic_out / (srv->cur_ts - srv->startup_ts);

	mod_status_get_multiplier(&avg, &multiplier, 1024);

	snprintf(buf, sizeof(buf), "%.2f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);
	buffer_append_string_len(b, CONST_STR_LEN("byte/s</td></tr>\n"));



	buffer_append_string_len(b, CONST_STR_LEN("<tr><th colspan=\"2\">average (5s sliding average)</th></tr>\n"));
	for (j = 0, avg = 0; j < 5; j++) {
		avg += p->mod_5s_requests[j];
	}

	avg /= 5;

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Requests</td><td class=\"string\">"));

	mod_status_get_multiplier(&avg, &multiplier, 1000);

	buffer_append_int(b, avg);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);

	buffer_append_string_len(b, CONST_STR_LEN("req/s</td></tr>\n"));

	for (j = 0, avg = 0; j < 5; j++) {
		avg += p->mod_5s_traffic_out[j];
	}

	avg /= 5;

	buffer_append_string_len(b, CONST_STR_LEN("<tr><td>Traffic</td><td class=\"string\">"));

	mod_status_get_multiplier(&avg, &multiplier, 1024);

	snprintf(buf, sizeof(buf), "%.2f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN(" "));
	if (multiplier)	buffer_append_string_len(b, &multiplier, 1);
	buffer_append_string_len(b, CONST_STR_LEN("byte/s</td></tr>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("</table>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<hr />\n<pre>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<b>"));
	buffer_append_int(b, srv->conns->used);
	buffer_append_string_len(b, CONST_STR_LEN(" connections</b>\n"));

	for (j = 0; j < srv->conns->used; j++) {
		connection *c = srv->conns->ptr[j];
		const char *state;

		if (CON_STATE_READ == c->state && !buffer_string_is_empty(c->request.orig_uri)) {
			state = "k";
			++cstates[CON_STATE_CLOSE+2];
		} else {
			state = connection_get_short_state(c->state);
			++cstates[(c->state <= CON_STATE_CLOSE ? c->state : CON_STATE_CLOSE+1)];
		}

		buffer_append_string_len(b, state, 1);

		if (((j + 1) % 50) == 0) {
			buffer_append_string_len(b, CONST_STR_LEN("\n"));
		}
	}
	buffer_append_string_len(b, CONST_STR_LEN("\n\n<table>\n"));
	buffer_append_string_len(b, CONST_STR_LEN("<tr><td style=\"text-align:right\">"));
	buffer_append_int(b, cstates[CON_STATE_CLOSE+2]);
	buffer_append_string_len(b, CONST_STR_LEN("<td>&nbsp;&nbsp;k = keep-alive</td></tr>\n"));
	for (j = 0; j < CON_STATE_CLOSE+2; ++j) {
		/*(skip "unknown" state if there are none; there should not be any unknown)*/
		if (0 == cstates[j] && j == CON_STATE_CLOSE+1) continue;
		buffer_append_string_len(b, CONST_STR_LEN("<tr><td style=\"text-align:right\">"));
		buffer_append_int(b, cstates[j]);
		buffer_append_string_len(b, CONST_STR_LEN("</td><td>&nbsp;&nbsp;"));
		buffer_append_string_len(b, connection_get_short_state(j), 1);
		buffer_append_string_len(b, CONST_STR_LEN(" = "));
		buffer_append_string(b, connection_get_state(j));
		buffer_append_string_len(b, CONST_STR_LEN("</td></tr>\n"));
	}
	buffer_append_string_len(b, CONST_STR_LEN("</table>"));

	buffer_append_string_len(b, CONST_STR_LEN("\n</pre><hr />\n<h2>Connections</h2>\n"));

	buffer_append_string_len(b, CONST_STR_LEN("<table summary=\"status\" class=\"status\">\n"));
	buffer_append_string_len(b, CONST_STR_LEN("<tr>"));
	mod_status_header_append_sort(b, p_d, "Client IP");
	mod_status_header_append_sort(b, p_d, "Read");
	mod_status_header_append_sort(b, p_d, "Written");
	mod_status_header_append_sort(b, p_d, "State");
	mod_status_header_append_sort(b, p_d, "Time");
	mod_status_header_append_sort(b, p_d, "Host");
	mod_status_header_append_sort(b, p_d, "URI");
	mod_status_header_append_sort(b, p_d, "File");
	buffer_append_string_len(b, CONST_STR_LEN("</tr>\n"));

	for (j = 0; j < srv->conns->used; j++) {
		connection *c = srv->conns->ptr[j];

		buffer_append_string_len(b, CONST_STR_LEN("<tr><td class=\"string\">"));

		buffer_append_string_buffer(b, c->dst_addr_buf);

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"int\">"));

		if (c->request.content_length) {
			buffer_append_int(b, c->request_content_queue->bytes_in);
			buffer_append_string_len(b, CONST_STR_LEN("/"));
			buffer_append_int(b, c->request.content_length);
		} else {
			buffer_append_string_len(b, CONST_STR_LEN("0/0"));
		}

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"int\">"));

		buffer_append_int(b, c->write_queue->bytes_out);
		buffer_append_string_len(b, CONST_STR_LEN("/"));
		buffer_append_int(b, c->write_queue->bytes_out + chunkqueue_length(c->write_queue));

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"string\">"));

		if (CON_STATE_READ == c->state && !buffer_string_is_empty(c->request.orig_uri)) {
			buffer_append_string_len(b, CONST_STR_LEN("keep-alive"));
		} else {
			buffer_append_string(b, connection_get_state(c->state));
		}

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"int\">"));

		buffer_append_int(b, srv->cur_ts - c->request_start);

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"string\">"));

		if (buffer_string_is_empty(c->server_name)) {
			buffer_append_string_buffer(b, c->uri.authority);
		}
		else {
			buffer_append_string_buffer(b, c->server_name);
		}

		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"string\">"));

		if (!buffer_string_is_empty(c->uri.path)) {
			buffer_append_string_encoded(b, CONST_BUF_LEN(c->uri.path), ENCODING_HTML);
		}

		if (!buffer_string_is_empty(c->uri.query)) {
			buffer_append_string_len(b, CONST_STR_LEN("?"));
			buffer_append_string_encoded(b, CONST_BUF_LEN(c->uri.query), ENCODING_HTML);
		}

		if (!buffer_string_is_empty(c->request.orig_uri)) {
			buffer_append_string_len(b, CONST_STR_LEN(" ("));
			buffer_append_string_encoded(b, CONST_BUF_LEN(c->request.orig_uri), ENCODING_HTML);
			buffer_append_string_len(b, CONST_STR_LEN(")"));
		}
		buffer_append_string_len(b, CONST_STR_LEN("</td><td class=\"string\">"));

		buffer_append_string_buffer(b, c->physical.path);

		buffer_append_string_len(b, CONST_STR_LEN("</td></tr>\n"));
	}


	buffer_append_string_len(b, CONST_STR_LEN(
		      "</table>\n"));


	buffer_append_string_len(b, CONST_STR_LEN(
		      " </body>\n"
		      "</html>\n"
		      ));

	chunkqueue_append_buffer_commit(con->write_queue);

	http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("text/html"));

	return 0;
}


static handler_t mod_status_handle_server_status_text(server *srv, connection *con, void *p_d) {
	plugin_data *p = p_d;
	buffer *b = chunkqueue_append_buffer_open(con->write_queue);
	double avg;
	time_t ts;
	char buf[32];
	unsigned int k;
	unsigned int l;

	/* output total number of requests */
	buffer_append_string_len(b, CONST_STR_LEN("Total Accesses: "));
	avg = p->abs_requests;
	snprintf(buf, sizeof(buf) - 1, "%.0f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	/* output total traffic out in kbytes */
	buffer_append_string_len(b, CONST_STR_LEN("Total kBytes: "));
	avg = p->abs_traffic_out / 1024;
	snprintf(buf, sizeof(buf) - 1, "%.0f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	/* output uptime */
	buffer_append_string_len(b, CONST_STR_LEN("Uptime: "));
	ts = srv->cur_ts - srv->startup_ts;
	buffer_append_int(b, ts);
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	/* output busy servers */
	buffer_append_string_len(b, CONST_STR_LEN("BusyServers: "));
	buffer_append_int(b, srv->conns->used);
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	buffer_append_string_len(b, CONST_STR_LEN("IdleServers: "));
	buffer_append_int(b, srv->conns->size - srv->conns->used);
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	/* output scoreboard */
	buffer_append_string_len(b, CONST_STR_LEN("Scoreboard: "));
	for (k = 0; k < srv->conns->used; k++) {
		connection *c = srv->conns->ptr[k];
		const char *state =
		  (CON_STATE_READ == c->state && !buffer_string_is_empty(c->request.orig_uri))
		    ? "k"
		    : connection_get_short_state(c->state);
		buffer_append_string_len(b, state, 1);
	}
	for (l = 0; l < srv->conns->size - srv->conns->used; l++) {
		buffer_append_string_len(b, CONST_STR_LEN("_"));
	}
	buffer_append_string_len(b, CONST_STR_LEN("\n"));

	chunkqueue_append_buffer_commit(con->write_queue);

	/* set text/plain output */
	http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("text/plain"));

	return 0;
}


static handler_t mod_status_handle_server_status_json(server *srv, connection *con, void *p_d) {
	plugin_data *p = p_d;
	buffer *b = chunkqueue_append_buffer_open(con->write_queue);
	double avg;
	time_t ts;
	char buf[32];
	size_t j;
	unsigned int jsonp = 0;

	if (buffer_string_length(con->uri.query) >= sizeof("jsonp=")-1
	   && 0 == memcmp(con->uri.query->ptr, CONST_STR_LEN("jsonp="))) {
		/* not a full parse of query string for multiple parameters,
		* not URL-decoding param and not XML-encoding (XSS protection),
		* so simply ensure that json function name isalnum() or '_' */
		const char *f = con->uri.query->ptr + sizeof("jsonp=")-1;
		int len = 0;
		while (light_isalnum(f[len]) || f[len] == '_') ++len;
		if (0 != len && light_isalpha(f[0]) && f[len] == '\0') {
			buffer_append_string_len(b, f, len);
			buffer_append_string_len(b, CONST_STR_LEN("("));
			jsonp = 1;
		}
	}

	/* output total number of requests */
	buffer_append_string_len(b, CONST_STR_LEN("{\n\t\"RequestsTotal\": "));
	avg = p->abs_requests;
	snprintf(buf, sizeof(buf) - 1, "%.0f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	/* output total traffic out in kbytes */
	buffer_append_string_len(b, CONST_STR_LEN("\t\"TrafficTotal\": "));
	avg = p->abs_traffic_out / 1024;
	snprintf(buf, sizeof(buf) - 1, "%.0f", avg);
	buffer_append_string(b, buf);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	/* output uptime */
	buffer_append_string_len(b, CONST_STR_LEN("\t\"Uptime\": "));
	ts = srv->cur_ts - srv->startup_ts;
	buffer_append_int(b, ts);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	/* output busy servers */
	buffer_append_string_len(b, CONST_STR_LEN("\t\"BusyServers\": "));
	buffer_append_int(b, srv->conns->used);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	buffer_append_string_len(b, CONST_STR_LEN("\t\"IdleServers\": "));
	buffer_append_int(b, srv->conns->size - srv->conns->used);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	for (j = 0, avg = 0; j < 5; j++) {
		avg += p->mod_5s_requests[j];
	}

	avg /= 5;

	buffer_append_string_len(b, CONST_STR_LEN("\t\"RequestAverage5s\":"));
	buffer_append_int(b, avg);
	buffer_append_string_len(b, CONST_STR_LEN(",\n"));

	for (j = 0, avg = 0; j < 5; j++) {
		avg += p->mod_5s_traffic_out[j];
	}

	avg /= 5;

	buffer_append_string_len(b, CONST_STR_LEN("\t\"TrafficAverage5s\":"));
	buffer_append_int(b, avg / 1024); /* kbps */
	buffer_append_string_len(b, CONST_STR_LEN("\n}"));

	if (jsonp) buffer_append_string_len(b, CONST_STR_LEN(");"));

	chunkqueue_append_buffer_commit(con->write_queue);

	/* set text/plain output */
	http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("application/javascript"));

	return 0;
}


static handler_t mod_status_handle_server_statistics(server *srv, connection *con, void *p_d) {
	buffer *b;
	size_t i;
	array *st = srv->status;
	UNUSED(p_d);

	if (0 == st->used) {
		/* we have nothing to send */
		con->http_status = 204;
		con->file_finished = 1;

		return HANDLER_FINISHED;
	}

	b = chunkqueue_append_buffer_open(con->write_queue);
	for (i = 0; i < st->used; i++) {
		size_t ndx = st->sorted[i];

		buffer_append_string_buffer(b, st->data[ndx]->key);
		buffer_append_string_len(b, CONST_STR_LEN(": "));
		buffer_append_int(b, ((data_integer *)(st->data[ndx]))->value);
		buffer_append_string_len(b, CONST_STR_LEN("\n"));
	}
	chunkqueue_append_buffer_commit(con->write_queue);

	http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("text/plain"));

	con->http_status = 200;
	con->file_finished = 1;

	return HANDLER_FINISHED;
}


static handler_t mod_status_handle_server_status(server *srv, connection *con, void *p_d) {

	if (buffer_is_equal_string(con->uri.query, CONST_STR_LEN("auto"))) {
		mod_status_handle_server_status_text(srv, con, p_d);
	} else if (buffer_string_length(con->uri.query) >= sizeof("json")-1
		   && 0 == memcmp(con->uri.query->ptr, CONST_STR_LEN("json"))) {
		mod_status_handle_server_status_json(srv, con, p_d);
	} else {
		mod_status_handle_server_status_html(srv, con, p_d);
	}

	con->http_status = 200;
	con->file_finished = 1;

	return HANDLER_FINISHED;
}


static handler_t mod_status_handle_server_config(server *srv, connection *con, void *p_d) {
	plugin_data *p = p_d;
	buffer *b = chunkqueue_append_buffer_open(con->write_queue);
	buffer *m = p->module_list;
	size_t i;

	buffer_copy_string_len(b, CONST_STR_LEN(
			   "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
			   "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
			   "         \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
			   "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
			   " <head>\n"
			   "  <title>Status</title>\n"
			   " </head>\n"
			   " <body>\n"
			   "  <h1>"));
	buffer_append_string_buffer(b, con->conf.server_tag);
	buffer_append_string_len(b, CONST_STR_LEN(
			   "</h1>\n"
			   "  <table summary=\"status\" border=\"1\">\n"));

	mod_status_header_append(b, "Server-Features");
#ifdef HAVE_PCRE_H
	mod_status_row_append(b, "RegEx Conditionals", "enabled");
#else
	mod_status_row_append(b, "RegEx Conditionals", "disabled - pcre missing");
#endif
	mod_status_header_append(b, "Network Engine");

	mod_status_row_append(b, "fd-Event-Handler", srv->srvconf.event_handler->ptr);

	mod_status_header_append(b, "Config-File-Settings");

	for (i = 0; i < srv->plugins.used; i++) {
		plugin **ps = srv->plugins.ptr;

		plugin *pl = ps[i];

		if (i == 0) {
			buffer_copy_buffer(m, pl->name);
		} else {
			buffer_append_string_len(m, CONST_STR_LEN("<br />"));
			buffer_append_string_buffer(m, pl->name);
		}
	}

	mod_status_row_append(b, "Loaded Modules", m->ptr);

	buffer_append_string_len(b, CONST_STR_LEN("  </table>\n"));

	buffer_append_string_len(b, CONST_STR_LEN(
		      " </body>\n"
		      "</html>\n"
		      ));

	chunkqueue_append_buffer_commit(con->write_queue);

	http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("text/html"));

	con->http_status = 200;
	con->file_finished = 1;

	return HANDLER_FINISHED;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_status_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(status_url);
	PATCH(config_url);
	PATCH(sort);
	PATCH(statistics_url);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("status.status-url"))) {
				PATCH(status_url);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("status.config-url"))) {
				PATCH(config_url);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("status.enable-sort"))) {
				PATCH(sort);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN("status.statistics-url"))) {
				PATCH(statistics_url);
			}
		}
	}

	return 0;
}

static handler_t mod_status_handler(server *srv, connection *con, void *p_d) {
	plugin_data *p = p_d;

	if (con->mode != DIRECT) return HANDLER_GO_ON;

	mod_status_patch_connection(srv, con, p);

	if (!buffer_string_is_empty(p->conf.status_url) &&
	    buffer_is_equal(p->conf.status_url, con->uri.path)) {
		return mod_status_handle_server_status(srv, con, p_d);
	} else if (!buffer_string_is_empty(p->conf.config_url) &&
	    buffer_is_equal(p->conf.config_url, con->uri.path)) {
		return mod_status_handle_server_config(srv, con, p_d);
	} else if (!buffer_string_is_empty(p->conf.statistics_url) &&
	    buffer_is_equal(p->conf.statistics_url, con->uri.path)) {
		return mod_status_handle_server_statistics(srv, con, p_d);
	}

	return HANDLER_GO_ON;
}

TRIGGER_FUNC(mod_status_trigger) {
	plugin_data *p = p_d;
	size_t i;

	/* check all connections */
	for (i = 0; i < srv->conns->used; i++) {
		connection *c = srv->conns->ptr[i];

		p->bytes_written += c->bytes_written_cur_second;
	}

	/* a sliding average */
	p->mod_5s_traffic_out[p->mod_5s_ndx] = p->bytes_written;
	p->mod_5s_requests   [p->mod_5s_ndx] = p->requests;

	p->mod_5s_ndx = (p->mod_5s_ndx+1) % 5;

	p->abs_traffic_out += p->bytes_written;
	p->rel_traffic_out += p->bytes_written;

	p->bytes_written = 0;

	/* reset storage - second */
	p->traffic_out = 0;
	p->requests    = 0;

	return HANDLER_GO_ON;
}

REQUESTDONE_FUNC(mod_status_account) {
	plugin_data *p = p_d;

	UNUSED(srv);

	p->requests++;
	p->rel_requests++;
	p->abs_requests++;

	p->bytes_written += con->bytes_written_cur_second;

	return HANDLER_GO_ON;
}

int mod_status_plugin_init(plugin *p);
int mod_status_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("status");

	p->init        = mod_status_init;
	p->cleanup     = mod_status_free;
	p->set_defaults= mod_status_set_defaults;

	p->handle_uri_clean    = mod_status_handler;
	p->handle_trigger      = mod_status_trigger;
	p->handle_request_done = mod_status_account;

	p->data        = NULL;

	return 0;
}
