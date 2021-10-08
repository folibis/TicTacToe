#ifndef CONNECTIONZMQ_H
#define CONNECTIONZMQ_H

#include <zmq.h>
#include "common.h"


typedef void (*read_callback_zmq)(char *data, int len);

extern result_t init_connection_zmq(read_callback_zmq onread);
extern result_t start_connection_zmq(char *address, int port);
extern result_t stop_connection_zmq();
extern result_t write_connection_zmq(char *data, int len);
extern result_t write_connection_zmq_short(char *data);

#endif // CONNECTIONZMQ_H
