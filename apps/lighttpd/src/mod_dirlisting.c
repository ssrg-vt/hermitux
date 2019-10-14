#include "first.h"

#include "base.h"
#include "log.h"
#include "buffer.h"
#include "fdevent.h"
#include "http_header.h"

#include "plugin.h"

#include "stat_cache.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#ifdef HAVE_PCRE_H
#include <pcre.h>
#endif

/**
 * this is a dirlisting for a lighttpd plugin
 */

#ifdef HAVE_ATTR_ATTRIBUTES_H
#include <attr/attributes.h>
#endif

#ifdef HAVE_SYS_EXTATTR_H
#include <sys/extattr.h>
#endif

/* plugin config for all request/connections */

typedef struct {
#ifdef HAVE_PCRE_H
	pcre *regex;
#endif
	buffer *string;
} excludes;

typedef struct {
	excludes **ptr;

	size_t used;
	size_t size;
} excludes_buffer;

typedef struct {
	unsigned short dir_listing;
	unsigned short hide_dot_files;
	unsigned short hide_readme_file;
	unsigned short encode_readme;
	unsigned short hide_header_file;
	unsigned short encode_header;
	unsigned short auto_layout;

	excludes_buffer *excludes;

	buffer *show_readme;
	buffer *show_header;
	buffer *external_css;
	buffer *external_js;
	buffer *encoding;
	buffer *set_footer;
} plugin_config;

typedef struct {
	PLUGIN_DATA;

	buffer *tmp_buf;
	buffer *content_charset;

	plugin_config **config_storage;

	plugin_config conf;
} plugin_data;

static excludes_buffer *excludes_buffer_init(void) {
	excludes_buffer *exb;

	exb = calloc(1, sizeof(*exb));

	return exb;
}

#ifdef HAVE_PCRE_H
static int excludes_buffer_append(excludes_buffer *exb, buffer *string) {
	size_t i;
	const char *errptr;
	int erroff;

	if (!string) return -1;

	if (exb->used == exb->size) {
		exb->size += 4;

		exb->ptr = realloc(exb->ptr, exb->size * sizeof(*exb->ptr));

		for(i = exb->used; i < exb->size; i++) {
			exb->ptr[i] = calloc(1, sizeof(**exb->ptr));
		}
	}


	if (NULL == (exb->ptr[exb->used]->regex = pcre_compile(string->ptr, 0,
						    &errptr, &erroff, NULL))) {
		return -1;
	}

	exb->ptr[exb->used]->string = buffer_init();
	buffer_copy_buffer(exb->ptr[exb->used]->string, string);

	exb->used++;

	return 0;
}
#endif

static void excludes_buffer_free(excludes_buffer *exb) {
#ifdef HAVE_PCRE_H
	size_t i;

	for (i = 0; i < exb->size; i++) {
		if (exb->ptr[i]->regex) pcre_free(exb->ptr[i]->regex);
		if (exb->ptr[i]->string) buffer_free(exb->ptr[i]->string);
		free(exb->ptr[i]);
	}

	if (exb->ptr) free(exb->ptr);
#endif

	free(exb);
}

/* init the plugin data */
INIT_FUNC(mod_dirlisting_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));

	p->tmp_buf = buffer_init();
	p->content_charset = buffer_init();

	return p;
}

/* detroy the plugin data */
FREE_FUNC(mod_dirlisting_free) {
	plugin_data *p = p_d;

	UNUSED(srv);

	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (!s) continue;

			excludes_buffer_free(s->excludes);
			buffer_free(s->show_readme);
			buffer_free(s->show_header);
			buffer_free(s->external_css);
			buffer_free(s->external_js);
			buffer_free(s->encoding);
			buffer_free(s->set_footer);

			free(s);
		}
		free(p->config_storage);
	}

	buffer_free(p->tmp_buf);
	buffer_free(p->content_charset);

	free(p);

	return HANDLER_GO_ON;
}

/* handle plugin config and check values */

#define CONFIG_EXCLUDE          "dir-listing.exclude"
#define CONFIG_ACTIVATE         "dir-listing.activate"
#define CONFIG_HIDE_DOTFILES    "dir-listing.hide-dotfiles"
#define CONFIG_EXTERNAL_CSS     "dir-listing.external-css"
#define CONFIG_EXTERNAL_JS      "dir-listing.external-js"
#define CONFIG_ENCODING         "dir-listing.encoding"
#define CONFIG_SHOW_README      "dir-listing.show-readme"
#define CONFIG_HIDE_README_FILE "dir-listing.hide-readme-file"
#define CONFIG_SHOW_HEADER      "dir-listing.show-header"
#define CONFIG_HIDE_HEADER_FILE "dir-listing.hide-header-file"
#define CONFIG_DIR_LISTING      "server.dir-listing"
#define CONFIG_SET_FOOTER       "dir-listing.set-footer"
#define CONFIG_ENCODE_README    "dir-listing.encode-readme"
#define CONFIG_ENCODE_HEADER    "dir-listing.encode-header"
#define CONFIG_AUTO_LAYOUT      "dir-listing.auto-layout"


SETDEFAULTS_FUNC(mod_dirlisting_set_defaults) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ CONFIG_EXCLUDE,          NULL, T_CONFIG_LOCAL, T_CONFIG_SCOPE_CONNECTION },   /* 0 */
		{ CONFIG_ACTIVATE,         NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 1 */
		{ CONFIG_HIDE_DOTFILES,    NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 2 */
		{ CONFIG_EXTERNAL_CSS,     NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 3 */
		{ CONFIG_ENCODING,         NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 4 */
		{ CONFIG_SHOW_README,      NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 5 */
		{ CONFIG_HIDE_README_FILE, NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 6 */
		{ CONFIG_SHOW_HEADER,      NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 7 */
		{ CONFIG_HIDE_HEADER_FILE, NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 8 */
		{ CONFIG_DIR_LISTING,      NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 9 */
		{ CONFIG_SET_FOOTER,       NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 10 */
		{ CONFIG_ENCODE_README,    NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 11 */
		{ CONFIG_ENCODE_HEADER,    NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 12 */
		{ CONFIG_AUTO_LAYOUT,      NULL, T_CONFIG_BOOLEAN, T_CONFIG_SCOPE_CONNECTION }, /* 13 */
		{ CONFIG_EXTERNAL_JS,      NULL, T_CONFIG_STRING, T_CONFIG_SCOPE_CONNECTION },  /* 14 */

		{ NULL,                          NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(srv->config_context->used, sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;
		data_unset *du_excludes;

		s = calloc(1, sizeof(plugin_config));
		s->excludes = excludes_buffer_init();
		s->dir_listing = 0;
		s->show_readme = buffer_init();
		s->show_header = buffer_init();
		s->external_css = buffer_init();
		s->external_js = buffer_init();
		s->hide_dot_files = 1;
		s->hide_readme_file = 0;
		s->hide_header_file = 0;
		s->encode_readme = 1;
		s->encode_header = 1;
		s->auto_layout = 1;

		s->encoding = buffer_init();
		s->set_footer = buffer_init();

		cv[0].destination = s->excludes;
		cv[1].destination = &(s->dir_listing);
		cv[2].destination = &(s->hide_dot_files);
		cv[3].destination = s->external_css;
		cv[4].destination = s->encoding;
		cv[5].destination = s->show_readme;
		cv[6].destination = &(s->hide_readme_file);
		cv[7].destination = s->show_header;
		cv[8].destination = &(s->hide_header_file);
		cv[9].destination = &(s->dir_listing); /* old name */
		cv[10].destination = s->set_footer;
		cv[11].destination = &(s->encode_readme);
		cv[12].destination = &(s->encode_header);
		cv[13].destination = &(s->auto_layout);
		cv[14].destination = s->external_js;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		if (NULL != (du_excludes = array_get_element(config->value, CONFIG_EXCLUDE))) {
			array *excludes_list;

			excludes_list = ((data_array*)du_excludes)->value;

			if (du_excludes->type != TYPE_ARRAY || !array_is_vlist(excludes_list)) {
				log_error_write(srv, __FILE__, __LINE__, "s",
					"unexpected type for " CONFIG_EXCLUDE "; expected list of \"regex\"");
				return HANDLER_ERROR;
			}

#ifndef HAVE_PCRE_H
			if (excludes_list->used > 0) {
				log_error_write(srv, __FILE__, __LINE__, "sss",
					"pcre support is missing for: ", CONFIG_EXCLUDE, ", please install libpcre and the headers");
				return HANDLER_ERROR;
			}
#else
			for (size_t j = 0; j < excludes_list->used; ++j) {
				data_unset *du_exclude = excludes_list->data[j];

				if (du_exclude->type != TYPE_STRING) {
					log_error_write(srv, __FILE__, __LINE__, "sssbs",
						"unexpected type for key: ", CONFIG_EXCLUDE, "[",
						du_exclude->key, "](string)");
					return HANDLER_ERROR;
				}

				if (0 != excludes_buffer_append(s->excludes, ((data_string*)(du_exclude))->value)) {
					log_error_write(srv, __FILE__, __LINE__, "sb",
						"pcre-compile failed for", ((data_string*)(du_exclude))->value);
					return HANDLER_ERROR;
				}
			}
#endif
		}

		if (!buffer_string_is_empty(s->show_readme)) {
			if (buffer_is_equal_string(s->show_readme, CONST_STR_LEN("enable"))) {
				buffer_copy_string_len(s->show_readme, CONST_STR_LEN("README.txt"));
			}
			else if (buffer_is_equal_string(s->show_readme, CONST_STR_LEN("disable"))) {
				buffer_clear(s->show_readme);
			}
		}

		if (!buffer_string_is_empty(s->show_header)) {
			if (buffer_is_equal_string(s->show_header, CONST_STR_LEN("enable"))) {
				buffer_copy_string_len(s->show_header, CONST_STR_LEN("HEADER.txt"));
			}
			else if (buffer_is_equal_string(s->show_header, CONST_STR_LEN("disable"))) {
				buffer_clear(s->show_header);
			}
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_dirlisting_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(dir_listing);
	PATCH(external_css);
	PATCH(external_js);
	PATCH(hide_dot_files);
	PATCH(encoding);
	PATCH(show_readme);
	PATCH(hide_readme_file);
	PATCH(show_header);
	PATCH(hide_header_file);
	PATCH(excludes);
	PATCH(set_footer);
	PATCH(encode_readme);
	PATCH(encode_header);
	PATCH(auto_layout);

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_ACTIVATE)) ||
			    buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_DIR_LISTING))) {
				PATCH(dir_listing);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_HIDE_DOTFILES))) {
				PATCH(hide_dot_files);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_EXTERNAL_CSS))) {
				PATCH(external_css);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_EXTERNAL_JS))) {
				PATCH(external_js);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_ENCODING))) {
				PATCH(encoding);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_SHOW_README))) {
				PATCH(show_readme);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_HIDE_README_FILE))) {
				PATCH(hide_readme_file);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_SHOW_HEADER))) {
				PATCH(show_header);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_HIDE_HEADER_FILE))) {
				PATCH(hide_header_file);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_SET_FOOTER))) {
				PATCH(set_footer);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_EXCLUDE))) {
				PATCH(excludes);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_ENCODE_README))) {
				PATCH(encode_readme);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_ENCODE_HEADER))) {
				PATCH(encode_header);
			} else if (buffer_is_equal_string(du->key, CONST_STR_LEN(CONFIG_AUTO_LAYOUT))) {
				PATCH(auto_layout);
			}
		}
	}

	return 0;
}
#undef PATCH

typedef struct {
	size_t  namelen;
	time_t  mtime;
	off_t   size;
} dirls_entry_t;

typedef struct {
	dirls_entry_t **ent;
	size_t used;
	size_t size;
} dirls_list_t;

#define DIRLIST_ENT_NAME(ent)	((char*)(ent) + sizeof(dirls_entry_t))
#define DIRLIST_BLOB_SIZE		16

/* simple combsort algorithm */
static void http_dirls_sort(dirls_entry_t **ent, int num) {
	int gap = num;
	int i, j;
	int swapped;
	dirls_entry_t *tmp;

	do {
		gap = (gap * 10) / 13;
		if (gap == 9 || gap == 10)
			gap = 11;
		if (gap < 1)
			gap = 1;
		swapped = 0;

		for (i = 0; i < num - gap; i++) {
			j = i + gap;
			if (strcmp(DIRLIST_ENT_NAME(ent[i]), DIRLIST_ENT_NAME(ent[j])) > 0) {
				tmp = ent[i];
				ent[i] = ent[j];
				ent[j] = tmp;
				swapped = 1;
			}
		}

	} while (gap > 1 || swapped);
}

/* buffer must be able to hold "999.9K"
 * conversion is simple but not perfect
 */
static int http_list_directory_sizefmt(char *buf, size_t bufsz, off_t size) {
	const char unit[] = " KMGTPE";	/* Kilo, Mega, Giga, Tera, Peta, Exa */
	const char *u = unit;		/* u will always increment at least once */
	int remain;
	size_t buflen;

	if (size < 100)
		size += 99;
	if (size < 100)
		size = 0;

	while (1) {
		remain = (int) size & 1023;
		size >>= 10;
		u++;
		if ((size & (~0 ^ 1023)) == 0)
			break;
	}

	remain /= 100;
	if (remain > 9)
		remain = 9;
	if (size > 999) {
		size   = 0;
		remain = 9;
		u++;
	}

	li_itostrn(buf, bufsz, size);
	buflen = strlen(buf);
	if (buflen + 3 >= bufsz) return buflen;
	buf[buflen+0] = '.';
	buf[buflen+1] = remain + '0';
	buf[buflen+2] = *u;
	buf[buflen+3] = '\0';

	return buflen + 3;
}

static void http_list_directory_include_file(buffer *out, int symlinks, buffer *path, const char *classname, int encode) {
	int fd = fdevent_open_cloexec(path->ptr, symlinks, O_RDONLY, 0);
	ssize_t rd;
	char buf[8192];

	if (-1 == fd) return;

	if (encode) {
		buffer_append_string_len(out, CONST_STR_LEN("<pre class=\""));
		buffer_append_string(out, classname);
		buffer_append_string_len(out, CONST_STR_LEN("\">"));
	}

	while ((rd = read(fd, buf, sizeof(buf))) > 0) {
		if (encode) {
			buffer_append_string_encoded(out, buf, (size_t)rd, ENCODING_MINIMAL_XML);
		} else {
			buffer_append_string_len(out, buf, (size_t)rd);
		}
	}
	close(fd);

	if (encode) {
		buffer_append_string_len(out, CONST_STR_LEN("</pre>"));
	}
}

/* portions copied from mod_status
 * modified and specialized for stable dirlist sorting by name */
static const char js_simple_table_resort[] = \
"var click_column;\n" \
"var name_column = 0;\n" \
"var date_column = 1;\n" \
"var size_column = 2;\n" \
"var type_column = 3;\n" \
"var prev_span = null;\n" \
"\n" \
"if (typeof(String.prototype.localeCompare) === 'undefined') {\n" \
" String.prototype.localeCompare = function(str, locale, options) {\n" \
"   return ((this == str) ? 0 : ((this > str) ? 1 : -1));\n" \
" };\n" \
"}\n" \
"\n" \
"if (typeof(String.prototype.toLocaleUpperCase) === 'undefined') {\n" \
" String.prototype.toLocaleUpperCase = function() {\n" \
"  return this.toUpperCase();\n" \
" };\n" \
"}\n" \
"\n" \
"function get_inner_text(el) {\n" \
" if((typeof el == 'string')||(typeof el == 'undefined'))\n" \
"  return el;\n" \
" if(el.innerText)\n" \
"  return el.innerText;\n" \
" else {\n" \
"  var str = \"\";\n" \
"  var cs = el.childNodes;\n" \
"  var l = cs.length;\n" \
"  for (i=0;i<l;i++) {\n" \
"   if (cs[i].nodeType==1) str += get_inner_text(cs[i]);\n" \
"   else if (cs[i].nodeType==3) str += cs[i].nodeValue;\n" \
"  }\n" \
" }\n" \
" return str;\n" \
"}\n" \
"\n" \
"function isdigit(c) {\n" \
" return (c >= '0' && c <= '9');\n" \
"}\n" \
"\n" \
"function unit_multiplier(unit) {\n" \
" return (unit=='K') ? 1000\n" \
"      : (unit=='M') ? 1000000\n" \
"      : (unit=='G') ? 1000000000\n" \
"      : (unit=='T') ? 1000000000000\n" \
"      : (unit=='P') ? 1000000000000000\n" \
"      : (unit=='E') ? 1000000000000000000 : 1;\n" \
"}\n" \
"\n" \
"var li_date_regex=/(\\d{4})-(\\w{3})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})/;\n" \
"\n" \
"var li_mon = ['Jan','Feb','Mar','Apr','May','Jun',\n" \
"              'Jul','Aug','Sep','Oct','Nov','Dec'];\n" \
"\n" \
"function li_mon_num(mon) {\n" \
" var i; for (i = 0; i < 12 && mon != li_mon[i]; ++i); return i;\n" \
"}\n" \
"\n" \
"function li_date_cmp(s1, s2) {\n" \
" var dp1 = li_date_regex.exec(s1)\n" \
" var dp2 = li_date_regex.exec(s2)\n" \
" for (var i = 1; i < 7; ++i) {\n" \
"  var cmp = (2 != i)\n" \
"   ? parseInt(dp1[i]) - parseInt(dp2[i])\n" \
"   : li_mon_num(dp1[2]) - li_mon_num(dp2[2]);\n" \
"  if (0 != cmp) return cmp;\n" \
" }\n" \
" return 0;\n" \
"}\n" \
"\n" \
"function sortfn_then_by_name(a,b,sort_column) {\n" \
" if (sort_column == name_column || sort_column == type_column) {\n" \
"  var ad = (a.cells[type_column].innerHTML === 'Directory');\n" \
"  var bd = (b.cells[type_column].innerHTML === 'Directory');\n" \
"  if (ad != bd) return (ad ? -1 : 1);\n" \
" }\n" \
" var at = get_inner_text(a.cells[sort_column]);\n" \
" var bt = get_inner_text(b.cells[sort_column]);\n" \
" var cmp;\n" \
" if (sort_column == name_column) {\n" \
"  if (at == '..') return -1;\n" \
"  if (bt == '..') return  1;\n" \
" }\n" \
" if (a.cells[sort_column].className == 'int') {\n" \
"  cmp = parseInt(at)-parseInt(bt);\n" \
" } else if (sort_column == date_column) {\n" \
"  var ad = isdigit(at.substr(0,1));\n" \
"  var bd = isdigit(bt.substr(0,1));\n" \
"  if (ad != bd) return (!ad ? -1 : 1);\n" \
"  cmp = li_date_cmp(at,bt);\n" \
" } else if (sort_column == size_column) {\n" \
"  var ai = parseInt(at, 10) * unit_multiplier(at.substr(-1,1));\n" \
"  var bi = parseInt(bt, 10) * unit_multiplier(bt.substr(-1,1));\n" \
"  if (at.substr(0,1) == '-') ai = -1;\n" \
"  if (bt.substr(0,1) == '-') bi = -1;\n" \
"  cmp = ai - bi;\n" \
" } else {\n" \
"  cmp = at.toLocaleUpperCase().localeCompare(bt.toLocaleUpperCase());\n" \
"  if (0 != cmp) return cmp;\n" \
"  cmp = at.localeCompare(bt);\n" \
" }\n" \
" if (0 != cmp || sort_column == name_column) return cmp;\n" \
" return sortfn_then_by_name(a,b,name_column);\n" \
"}\n" \
"\n" \
"function sortfn(a,b) {\n" \
" return sortfn_then_by_name(a,b,click_column);\n" \
"}\n" \
"\n" \
"function resort(lnk) {\n" \
" var span = lnk.childNodes[1];\n" \
" var table = lnk.parentNode.parentNode.parentNode.parentNode;\n" \
" var rows = new Array();\n" \
" for (j=1;j<table.rows.length;j++)\n" \
"  rows[j-1] = table.rows[j];\n" \
" click_column = lnk.parentNode.cellIndex;\n" \
" rows.sort(sortfn);\n" \
"\n" \
" if (prev_span != null) prev_span.innerHTML = '';\n" \
" if (span.getAttribute('sortdir')=='down') {\n" \
"  span.innerHTML = '&uarr;';\n" \
"  span.setAttribute('sortdir','up');\n" \
"  rows.reverse();\n" \
" } else {\n" \
"  span.innerHTML = '&darr;';\n" \
"  span.setAttribute('sortdir','down');\n" \
" }\n" \
" for (i=0;i<rows.length;i++)\n" \
"  table.tBodies[0].appendChild(rows[i]);\n" \
" prev_span = span;\n" \
"}\n";

/* portions copied from mod_dirlist (lighttpd2) */
static const char js_simple_table_init_sort[] = \
"\n" \
"function init_sort(init_sort_column, ascending) {\n" \
" var tables = document.getElementsByTagName(\"table\");\n" \
" for (var i = 0; i < tables.length; i++) {\n" \
"  var table = tables[i];\n" \
"  //var c = table.getAttribute(\"class\")\n" \
"  //if (-1 != c.split(\" \").indexOf(\"sort\")) {\n" \
"   var row = table.rows[0].cells;\n" \
"   for (var j = 0; j < row.length; j++) {\n" \
"    var n = row[j];\n" \
"    if (n.childNodes.length == 1 && n.childNodes[0].nodeType == 3) {\n" \
"     var link = document.createElement(\"a\");\n" \
"     var title = n.childNodes[0].nodeValue.replace(/:$/, \"\");\n" \
"     link.appendChild(document.createTextNode(title));\n" \
"     link.setAttribute(\"href\", \"#\");\n" \
"     link.setAttribute(\"class\", \"sortheader\");\n" \
"     link.setAttribute(\"onclick\", \"resort(this);return false;\");\n" \
"     var arrow = document.createElement(\"span\");\n" \
"     arrow.setAttribute(\"class\", \"sortarrow\");\n" \
"     arrow.appendChild(document.createTextNode(\":\"));\n" \
"     link.appendChild(arrow)\n" \
"     n.replaceChild(link, n.firstChild);\n" \
"    }\n" \
"   }\n" \
"   var lnk = row[init_sort_column].firstChild;\n" \
"   if (ascending) {\n" \
"    var span = lnk.childNodes[1];\n" \
"    span.setAttribute('sortdir','down');\n" \
"   }\n" \
"   resort(lnk);\n" \
"  //}\n" \
" }\n" \
"}\n";

static void http_dirlist_append_js_table_resort (buffer *b, connection *con) {
	char col = '0';
	char ascending = '0';
	if (!buffer_string_is_empty(con->uri.query)) {
		const char *qs = con->uri.query->ptr;
		do {
			if (qs[0] == 'C' && qs[1] == '=') {
				switch (qs[2]) {
				case 'N': col = '0'; break;
				case 'M': col = '1'; break;
				case 'S': col = '2'; break;
				case 'T':
				case 'D': col = '3'; break;
				default:  break;
				}
			}
			else if (qs[0] == 'O' && qs[1] == '=') {
				switch (qs[2]) {
				case 'A': ascending = '1'; break;
				case 'D': ascending = '0'; break;
				default:  break;
				}
			}
		} while ((qs = strchr(qs, '&')) && *++qs);
	}

	buffer_append_string_len(b, CONST_STR_LEN("\n<script type=\"text/javascript\">\n// <!--\n\n"));
	buffer_append_string_len(b, js_simple_table_resort, sizeof(js_simple_table_resort)-1);
	buffer_append_string_len(b, js_simple_table_init_sort, sizeof(js_simple_table_init_sort)-1);
	buffer_append_string_len(b, CONST_STR_LEN("\ninit_sort("));
	buffer_append_string_len(b, &col, 1);
	buffer_append_string_len(b, CONST_STR_LEN(", "));
	buffer_append_string_len(b, &ascending, 1);
	buffer_append_string_len(b, CONST_STR_LEN(");\n\n// -->\n</script>\n\n"));
}

static void http_list_directory_header(server *srv, connection *con, plugin_data *p, buffer *out) {
	UNUSED(srv);

	if (p->conf.auto_layout) {
		buffer_append_string_len(out, CONST_STR_LEN(
			"<!DOCTYPE html>\n"
			"<html>\n"
			"<head>\n"
		));
		if (!buffer_string_is_empty(p->conf.encoding)) {
			buffer_append_string_len(out, CONST_STR_LEN("<meta charset=\""));
			buffer_append_string_buffer(out, p->conf.encoding);
			buffer_append_string_len(out, CONST_STR_LEN("\">\n"));
		}
		buffer_append_string_len(out, CONST_STR_LEN("<title>Index of "));
		buffer_append_string_encoded(out, CONST_BUF_LEN(con->uri.path), ENCODING_MINIMAL_XML);
		buffer_append_string_len(out, CONST_STR_LEN("</title>\n"));

		if (!buffer_string_is_empty(p->conf.external_css)) {
			buffer_append_string_len(out, CONST_STR_LEN("<meta name=\"viewport\" content=\"initial-scale=1\">"));
			buffer_append_string_len(out, CONST_STR_LEN("<link rel=\"stylesheet\" type=\"text/css\" href=\""));
			buffer_append_string_buffer(out, p->conf.external_css);
			buffer_append_string_len(out, CONST_STR_LEN("\">\n"));
		} else {
			buffer_append_string_len(out, CONST_STR_LEN(
				"<style type=\"text/css\">\n"
				"a, a:active {text-decoration: none; color: blue;}\n"
				"a:visited {color: #48468F;}\n"
				"a:hover, a:focus {text-decoration: underline; color: red;}\n"
				"body {background-color: #F5F5F5;}\n"
				"h2 {margin-bottom: 12px;}\n"
				"table {margin-left: 12px;}\n"
				"th, td {"
				" font: 90% monospace;"
				" text-align: left;"
				"}\n"
				"th {"
				" font-weight: bold;"
				" padding-right: 14px;"
				" padding-bottom: 3px;"
				"}\n"
				"td {padding-right: 14px;}\n"
				"td.s, th.s {text-align: right;}\n"
				"div.list {"
				" background-color: white;"
				" border-top: 1px solid #646464;"
				" border-bottom: 1px solid #646464;"
				" padding-top: 10px;"
				" padding-bottom: 14px;"
				"}\n"
				"div.foot {"
				" font: 90% monospace;"
				" color: #787878;"
				" padding-top: 4px;"
				"}\n"
				"</style>\n"
			));
		}

		buffer_append_string_len(out, CONST_STR_LEN("</head>\n<body>\n"));
	}

	if (!buffer_string_is_empty(p->conf.show_header)) {
		/* if we have a HEADER file, display it in <pre class="header"></pre> */

		buffer *hb = p->conf.show_header;
		if (hb->ptr[0] != '/') {
			buffer_copy_buffer(p->tmp_buf, con->physical.path);
			buffer_append_path_len(p->tmp_buf, CONST_BUF_LEN(p->conf.show_header));
			hb = p->tmp_buf;
		}

		http_list_directory_include_file(out, con->conf.follow_symlink, hb, "header", p->conf.encode_header);
	}

	buffer_append_string_len(out, CONST_STR_LEN("<h2>Index of "));
	buffer_append_string_encoded(out, CONST_BUF_LEN(con->uri.path), ENCODING_MINIMAL_XML);
	buffer_append_string_len(out, CONST_STR_LEN(
		"</h2>\n"
		"<div class=\"list\">\n"
		"<table summary=\"Directory Listing\" cellpadding=\"0\" cellspacing=\"0\">\n"
		"<thead>"
		"<tr>"
			"<th class=\"n\">Name</th>"
			"<th class=\"m\">Last Modified</th>"
			"<th class=\"s\">Size</th>"
			"<th class=\"t\">Type</th>"
		"</tr>"
		"</thead>\n"
		"<tbody>\n"
	));
	if (!buffer_is_equal_string(con->uri.path, CONST_STR_LEN("/"))) {
		buffer_append_string_len(out, CONST_STR_LEN(
		"<tr class=\"d\">"
			"<td class=\"n\"><a href=\"../\">..</a>/</td>"
			"<td class=\"m\">&nbsp;</td>"
			"<td class=\"s\">- &nbsp;</td>"
			"<td class=\"t\">Directory</td>"
		"</tr>\n"
		));
	}
}

static void http_list_directory_footer(server *srv, connection *con, plugin_data *p, buffer *out) {
	UNUSED(srv);

	buffer_append_string_len(out, CONST_STR_LEN(
		"</tbody>\n"
		"</table>\n"
		"</div>\n"
	));

	if (!buffer_string_is_empty(p->conf.show_readme)) {
		/* if we have a README file, display it in <pre class="readme"></pre> */

		buffer *rb = p->conf.show_readme;
		if (rb->ptr[0] != '/') {
			buffer_copy_buffer(p->tmp_buf,  con->physical.path);
			buffer_append_path_len(p->tmp_buf, CONST_BUF_LEN(p->conf.show_readme));
			rb = p->tmp_buf;
		}

		http_list_directory_include_file(out, con->conf.follow_symlink, rb, "readme", p->conf.encode_readme);
	}

	if(p->conf.auto_layout) {

		buffer_append_string_len(out, CONST_STR_LEN(
			"<div class=\"foot\">"
		));

		if (!buffer_string_is_empty(p->conf.set_footer)) {
			buffer_append_string_buffer(out, p->conf.set_footer);
		} else {
			buffer_append_string_buffer(out, con->conf.server_tag);
		}

		buffer_append_string_len(out, CONST_STR_LEN(
			"</div>\n"
		));

		if (!buffer_string_is_empty(p->conf.external_js)) {
			buffer_append_string_len(out, CONST_STR_LEN("<script type=\"text/javascript\" src=\""));
			buffer_append_string_buffer(out, p->conf.external_js);
			buffer_append_string_len(out, CONST_STR_LEN("\"></script>\n"));
		} else if (buffer_is_empty(p->conf.external_js)) {
			http_dirlist_append_js_table_resort(out, con);
		}

		buffer_append_string_len(out, CONST_STR_LEN(
			"</body>\n"
			"</html>\n"
		));
	}
}

static int http_list_directory(server *srv, connection *con, plugin_data *p, buffer *dir) {
	DIR *dp;
	buffer *out;
	struct dirent *dent;
	struct stat st;
	char *path, *path_file;
	size_t i;
	int hide_dotfiles = p->conf.hide_dot_files;
	dirls_list_t dirs, files, *list;
	dirls_entry_t *tmp;
	char sizebuf[sizeof("999.9K")];
	char datebuf[sizeof("2005-Jan-01 22:23:24")];
	const char *content_type;
	long name_max;
#if defined(HAVE_XATTR) || defined(HAVE_EXTATTR)
	char attrval[128];
	int attrlen;
#endif
#ifdef HAVE_LOCALTIME_R
	struct tm tm;
#endif

	if (buffer_string_is_empty(dir)) return -1;

	i = buffer_string_length(dir);

#ifdef HAVE_PATHCONF
	if (0 >= (name_max = pathconf(dir->ptr, _PC_NAME_MAX))) {
		/* some broken fs (fuse) return 0 instead of -1 */
#ifdef NAME_MAX
		name_max = NAME_MAX;
#else
		name_max = 255; /* stupid default */
#endif
	}
#elif defined __WIN32
	name_max = FILENAME_MAX;
#else
	name_max = NAME_MAX;
#endif

	path = malloc(i + name_max + 1);
	force_assert(NULL != path);
	memcpy(path, dir->ptr, i+1);
	path_file = path + i;

	if (NULL == (dp = opendir(path))) {
		log_error_write(srv, __FILE__, __LINE__, "sbs",
			"opendir failed:", dir, strerror(errno));

		free(path);
		return -1;
	}

	dirs.ent   = (dirls_entry_t**) malloc(sizeof(dirls_entry_t*) * DIRLIST_BLOB_SIZE);
	force_assert(dirs.ent);
	dirs.size  = DIRLIST_BLOB_SIZE;
	dirs.used  = 0;
	files.ent  = (dirls_entry_t**) malloc(sizeof(dirls_entry_t*) * DIRLIST_BLOB_SIZE);
	force_assert(files.ent);
	files.size = DIRLIST_BLOB_SIZE;
	files.used = 0;

	while ((dent = readdir(dp)) != NULL) {
#ifdef HAVE_PCRE_H
		unsigned short exclude_match = 0;
#endif

		if (dent->d_name[0] == '.') {
			if (hide_dotfiles)
				continue;
			if (dent->d_name[1] == '\0')
				continue;
			if (dent->d_name[1] == '.' && dent->d_name[2] == '\0')
				continue;
		}

		if (p->conf.hide_readme_file && !buffer_string_is_empty(p->conf.show_readme)) {
			if (strcmp(dent->d_name, p->conf.show_readme->ptr) == 0)
				continue;
		}
		if (p->conf.hide_header_file && !buffer_string_is_empty(p->conf.show_header)) {
			if (strcmp(dent->d_name, p->conf.show_header->ptr) == 0)
				continue;
		}

		/* compare d_name against excludes array
		 * elements, skipping any that match.
		 */
#ifdef HAVE_PCRE_H
		for(i = 0; i < p->conf.excludes->used; i++) {
			int n;
#define N 10
			int ovec[N * 3];
			pcre *regex = p->conf.excludes->ptr[i]->regex;

			if ((n = pcre_exec(regex, NULL, dent->d_name,
				    strlen(dent->d_name), 0, 0, ovec, 3 * N)) < 0) {
				if (n != PCRE_ERROR_NOMATCH) {
					log_error_write(srv, __FILE__, __LINE__, "sd",
						"execution error while matching:", n);

					/* aborting would require a lot of manual cleanup here.
					 * skip instead (to not leak names that break pcre matching)
					 */
					exclude_match = 1;
					break;
				}
			}
			else {
				exclude_match = 1;
				break;
			}
		}

		if (exclude_match) {
			continue;
		}
#endif

		i = strlen(dent->d_name);

		/* NOTE: the manual says, d_name is never more than NAME_MAX
		 *       so this should actually not be a buffer-overflow-risk
		 */
		if (i > (size_t)name_max) continue;

		memcpy(path_file, dent->d_name, i + 1);
		if (stat(path, &st) != 0)
			continue;

		list = &files;
		if (S_ISDIR(st.st_mode))
			list = &dirs;

		if (list->used == list->size) {
			list->size += DIRLIST_BLOB_SIZE;
			list->ent   = (dirls_entry_t**) realloc(list->ent, sizeof(dirls_entry_t*) * list->size);
			force_assert(list->ent);
		}

		tmp = (dirls_entry_t*) malloc(sizeof(dirls_entry_t) + 1 + i);
		tmp->mtime = st.st_mtime;
		tmp->size  = st.st_size;
		tmp->namelen = i;
		memcpy(DIRLIST_ENT_NAME(tmp), dent->d_name, i + 1);

		list->ent[list->used++] = tmp;
	}
	closedir(dp);

	if (dirs.used) http_dirls_sort(dirs.ent, dirs.used);

	if (files.used) http_dirls_sort(files.ent, files.used);

	out = chunkqueue_append_buffer_open(con->write_queue);
	http_list_directory_header(srv, con, p, out);

	/* directories */
	for (i = 0; i < dirs.used; i++) {
		tmp = dirs.ent[i];

#ifdef HAVE_LOCALTIME_R
		localtime_r(&(tmp->mtime), &tm);
		strftime(datebuf, sizeof(datebuf), "%Y-%b-%d %H:%M:%S", &tm);
#else
		strftime(datebuf, sizeof(datebuf), "%Y-%b-%d %H:%M:%S", localtime(&(tmp->mtime)));
#endif

		buffer_append_string_len(out, CONST_STR_LEN("<tr class=\"d\"><td class=\"n\"><a href=\""));
		buffer_append_string_encoded(out, DIRLIST_ENT_NAME(tmp), tmp->namelen, ENCODING_REL_URI_PART);
		buffer_append_string_len(out, CONST_STR_LEN("/\">"));
		buffer_append_string_encoded(out, DIRLIST_ENT_NAME(tmp), tmp->namelen, ENCODING_MINIMAL_XML);
		buffer_append_string_len(out, CONST_STR_LEN("</a>/</td><td class=\"m\">"));
		buffer_append_string_len(out, datebuf, sizeof(datebuf) - 1);
		buffer_append_string_len(out, CONST_STR_LEN("</td><td class=\"s\">- &nbsp;</td><td class=\"t\">Directory</td></tr>\n"));

		free(tmp);
	}

	/* files */
	for (i = 0; i < files.used; i++) {
		tmp = files.ent[i];

		content_type = NULL;
#if defined(HAVE_XATTR)
		if (con->conf.use_xattr) {
			memcpy(path_file, DIRLIST_ENT_NAME(tmp), tmp->namelen + 1);
			attrlen = sizeof(attrval) - 1;
			if (attr_get(path, srv->srvconf.xattr_name->ptr, attrval, &attrlen, 0) == 0) {
				attrval[attrlen] = '\0';
				content_type = attrval;
			}
		}
#elif defined(HAVE_EXTATTR)
		if (con->conf.use_xattr) {
			memcpy(path_file, DIRLIST_ENT_NAME(tmp), tmp->namelen + 1);
			if(-1 != (attrlen = extattr_get_file(path, EXTATTR_NAMESPACE_USER, srv->srvconf.xattr_name->ptr, attrval, sizeof(attrval)-1))) {
				attrval[attrlen] = '\0';
				content_type = attrval;
			}
		}
#endif

		if (content_type == NULL) {
			const buffer *type = stat_cache_mimetype_by_ext(con, DIRLIST_ENT_NAME(tmp), tmp->namelen);
			content_type = NULL != type ? type->ptr : "application/octet-stream";
		}

#ifdef HAVE_LOCALTIME_R
		localtime_r(&(tmp->mtime), &tm);
		strftime(datebuf, sizeof(datebuf), "%Y-%b-%d %H:%M:%S", &tm);
#else
		strftime(datebuf, sizeof(datebuf), "%Y-%b-%d %H:%M:%S", localtime(&(tmp->mtime)));
#endif
		http_list_directory_sizefmt(sizebuf, sizeof(sizebuf), tmp->size);

		buffer_append_string_len(out, CONST_STR_LEN("<tr><td class=\"n\"><a href=\""));
		buffer_append_string_encoded(out, DIRLIST_ENT_NAME(tmp), tmp->namelen, ENCODING_REL_URI_PART);
		buffer_append_string_len(out, CONST_STR_LEN("\">"));
		buffer_append_string_encoded(out, DIRLIST_ENT_NAME(tmp), tmp->namelen, ENCODING_MINIMAL_XML);
		buffer_append_string_len(out, CONST_STR_LEN("</a></td><td class=\"m\">"));
		buffer_append_string_len(out, datebuf, sizeof(datebuf) - 1);
		buffer_append_string_len(out, CONST_STR_LEN("</td><td class=\"s\">"));
		buffer_append_string(out, sizebuf);
		buffer_append_string_len(out, CONST_STR_LEN("</td><td class=\"t\">"));
		buffer_append_string(out, content_type);
		buffer_append_string_len(out, CONST_STR_LEN("</td></tr>\n"));

		free(tmp);
	}

	free(files.ent);
	free(dirs.ent);
	free(path);

	http_list_directory_footer(srv, con, p, out);

	/* Insert possible charset to Content-Type */
	if (buffer_string_is_empty(p->conf.encoding)) {
		http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_STR_LEN("text/html"));
	} else {
		buffer_copy_string_len(p->content_charset, CONST_STR_LEN("text/html; charset="));
		buffer_append_string_buffer(p->content_charset, p->conf.encoding);
		http_header_response_set(con, HTTP_HEADER_CONTENT_TYPE, CONST_STR_LEN("Content-Type"), CONST_BUF_LEN(p->content_charset));
	}

	chunkqueue_append_buffer_commit(con->write_queue);
	con->file_finished = 1;

	return 0;
}



URIHANDLER_FUNC(mod_dirlisting_subrequest) {
	plugin_data *p = p_d;
	stat_cache_entry *sce = NULL;

	UNUSED(srv);

	/* we only handle GET and HEAD */
	switch(con->request.http_method) {
	case HTTP_METHOD_GET:
	case HTTP_METHOD_HEAD:
		break;
	default:
		return HANDLER_GO_ON;
	}

	if (con->mode != DIRECT) return HANDLER_GO_ON;

	if (buffer_is_empty(con->physical.path)) return HANDLER_GO_ON;
	if (buffer_is_empty(con->uri.path)) return HANDLER_GO_ON;
	if (con->uri.path->ptr[buffer_string_length(con->uri.path) - 1] != '/') return HANDLER_GO_ON;

	mod_dirlisting_patch_connection(srv, con, p);

	if (!p->conf.dir_listing) return HANDLER_GO_ON;

	if (con->conf.log_request_handling) {
		log_error_write(srv, __FILE__, __LINE__,  "s",  "-- handling the request as Dir-Listing");
		log_error_write(srv, __FILE__, __LINE__,  "sb", "URI          :", con->uri.path);
	}

	if (HANDLER_ERROR == stat_cache_get_entry(srv, con, con->physical.path, &sce)) {
		log_error_write(srv, __FILE__, __LINE__,  "SB", "stat_cache_get_entry failed: ", con->physical.path);
		SEGFAULT();
	}

	if (!S_ISDIR(sce->st.st_mode)) return HANDLER_GO_ON;

	if (http_list_directory(srv, con, p, con->physical.path)) {
		/* dirlisting failed */
		con->http_status = 403;
	}

	buffer_reset(con->physical.path);

	/* not found */
	return HANDLER_FINISHED;
}

/* this function is called at dlopen() time and inits the callbacks */

int mod_dirlisting_plugin_init(plugin *p);
int mod_dirlisting_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("dirlisting");

	p->init        = mod_dirlisting_init;
	p->handle_subrequest_start  = mod_dirlisting_subrequest;
	p->set_defaults  = mod_dirlisting_set_defaults;
	p->cleanup     = mod_dirlisting_free;

	p->data        = NULL;

	return 0;
}
