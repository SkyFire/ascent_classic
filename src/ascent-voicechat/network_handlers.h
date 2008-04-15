#ifndef __NETWORK_HANDLERS_H
#define __NETWORK_HANDLERS_H

int voicechat_ascent_listen_socket_read_handler(network_socket *s, int act);
int voicechat_ascent_socket_read_handler(network_socket *s, int act);
int voicechat_client_socket_read_handler(network_socket *s, int act);

int voicechat_init_clientsocket();
int voicechat_init_serversocket();
int get_server_count();

#endif
