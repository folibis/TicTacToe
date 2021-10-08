#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"


typedef void (*connected_callback)(int connID, char *address, int port);
typedef void (*read_callback)(int connID, char *data, int len);
typedef void (*close_callback)(int connID);

extern result_t init_connection(connected_callback onconnect, read_callback onread, close_callback onclose);
extern result_t start_connection(char *address, int port);
extern result_t close_connection(int connID);
extern result_t close_connections();
extern result_t wait_connection();
extern result_t stop_connection();
extern result_t write_connection(int connID, char *data, int len);

#endif // CONNECTION_H
