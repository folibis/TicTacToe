#ifndef SERVER_H
#define SERVER_H

#include "common.h"


extern result_t init_server();
extern result_t run_server(char *address, int port, char *address_zmq, int port_zmq);
extern result_t stop_server();

#endif // SERVER_H
