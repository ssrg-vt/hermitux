#ifndef _PLUGIN_H_
#define _PLUGIN_H_
#include "first.h"

#include "base_decls.h"
#include "buffer.h"
#include "array.h"
#include "configfile.h"

#define SERVER_FUNC(x) \
		static handler_t x(server *srv, void *p_d)

#define CONNECTION_FUNC(x) \
		static handler_t x(server *srv, connection *con, void *p_d)

#define INIT_FUNC(x) \
		__attribute_cold__ \
		static void *x(void)

#define FREE_FUNC          __attribute_cold__ SERVER_FUNC
#define SETDEFAULTS_FUNC   __attribute_cold__ SERVER_FUNC
#define SIGHUP_FUNC        __attribute_cold__ SERVER_FUNC
#define TRIGGER_FUNC       SERVER_FUNC

#define SUBREQUEST_FUNC    CONNECTION_FUNC
#define PHYSICALPATH_FUNC  CONNECTION_FUNC
#define REQUESTDONE_FUNC   CONNECTION_FUNC
#define URIHANDLER_FUNC    CONNECTION_FUNC

#define PLUGIN_DATA        size_t id

typedef struct {
	size_t version;

	buffer *name; /* name of the plugin */

	void *(* init)                       ();
	handler_t (* priv_defaults)          (server *srv, void *p_d);
	handler_t (* set_defaults)           (server *srv, void *p_d);
	handler_t (* worker_init)            (server *srv, void *p_d); /* at server startup (each worker after fork()) */
	handler_t (* cleanup)                (server *srv, void *p_d);
	                                                                                   /* is called ... */
	handler_t (* handle_trigger)         (server *srv, void *p_d);                     /* once a second */
	handler_t (* handle_sighup)          (server *srv, void *p_d);                     /* at a sighup */
	handler_t (* handle_waitpid)         (server *srv, void *p_d, pid_t pid, int status); /* upon a child process exit */

	handler_t (* handle_uri_raw)         (server *srv, connection *con, void *p_d);    /* after uri_raw is set */
	handler_t (* handle_uri_clean)       (server *srv, connection *con, void *p_d);    /* after uri is set */
	handler_t (* handle_docroot)         (server *srv, connection *con, void *p_d);    /* getting the document-root */
	handler_t (* handle_physical)        (server *srv, connection *con, void *p_d);    /* mapping url to physical path */
	handler_t (* handle_request_env)     (server *srv, connection *con, void *p_d);    /* (deferred env populate) */
	handler_t (* handle_request_done)    (server *srv, connection *con, void *p_d);    /* at the end of a request */
	handler_t (* handle_connection_accept) (server *srv, connection *con, void *p_d);  /* after accept() socket */
	handler_t (* handle_connection_shut_wr)(server *srv, connection *con, void *p_d);  /* done writing to socket */
	handler_t (* handle_connection_close)  (server *srv, connection *con, void *p_d);  /* before close() of socket */



	handler_t (* handle_subrequest_start)(server *srv, connection *con, void *p_d);

	                                                                                   /* when a handler for the request
											    * has to be found
											    */
	handler_t (* handle_subrequest)      (server *srv, connection *con, void *p_d);    /* */
	handler_t (* handle_response_start)  (server *srv, connection *con, void *p_d);    /* before response headers are written */
	handler_t (* connection_reset)       (server *srv, connection *con, void *p_d);    /* after request done or request abort */
	void *data;

	/* dlopen handle */
	void *lib;
} plugin;

__attribute_cold__
int plugins_load(server *srv);

__attribute_cold__
void plugins_free(server *srv);

handler_t plugins_call_handle_uri_raw(server *srv, connection *con);
handler_t plugins_call_handle_uri_clean(server *srv, connection *con);
handler_t plugins_call_handle_subrequest_start(server *srv, connection *con);
handler_t plugins_call_handle_subrequest(server *srv, connection *con);
handler_t plugins_call_handle_response_start(server *srv, connection *con);
handler_t plugins_call_handle_request_env(server *srv, connection *con);
handler_t plugins_call_handle_request_done(server *srv, connection *con);
handler_t plugins_call_handle_docroot(server *srv, connection *con);
handler_t plugins_call_handle_physical(server *srv, connection *con);
handler_t plugins_call_handle_connection_accept(server *srv, connection *con);
handler_t plugins_call_handle_connection_shut_wr(server *srv, connection *con);
handler_t plugins_call_handle_connection_close(server *srv, connection *con);
handler_t plugins_call_connection_reset(server *srv, connection *con);

handler_t plugins_call_handle_trigger(server *srv);
handler_t plugins_call_handle_waitpid(server *srv, pid_t pid, int status);

__attribute_cold__
handler_t plugins_call_handle_sighup(server *srv);

__attribute_cold__
handler_t plugins_call_init(server *srv);

__attribute_cold__
handler_t plugins_call_set_defaults(server *srv);

__attribute_cold__
handler_t plugins_call_worker_init(server *srv);

__attribute_cold__
handler_t plugins_call_cleanup(server *srv);

#endif
