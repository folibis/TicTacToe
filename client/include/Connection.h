#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"


typedef void (* callback)(char *data, int len);

extern result_t init_connection(callback read_callback);
extern result_t start_connection(char *address, int port);
extern result_t close_connection();
extern result_t write_connection(char *data, int len);

#endif // CONNECTION_H
