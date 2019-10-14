#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_
#include "first.h"

#include "base.h"

__attribute_cold__
void connections_free(server *srv);

connection * connection_accept(server *srv, server_socket *srv_sock);
connection * connection_accepted(server *srv, server_socket *srv_socket, sock_addr *cnt_addr, int cnt);

int connection_set_state(server *srv, connection *con, connection_state_t state);
const char * connection_get_state(connection_state_t state);
const char * connection_get_short_state(connection_state_t state);
int connection_state_machine(server *srv, connection *con);
handler_t connection_handle_read_post_state(server *srv, connection *con);
handler_t connection_handle_read_post_error(server *srv, connection *con, int http_status);
int connection_write_chunkqueue(server *srv, connection *con, chunkqueue *c, off_t max_bytes);
void connection_response_reset(server *srv, connection *con);

#endif
