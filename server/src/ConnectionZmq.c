#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ConnectionZmq.h"

#define BUFFER_SIZE 1024
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1235

static void* thread_routine(void *arg);

static int m_running = 0;
static pthread_t m_read_thread;
static void *context = 0;
static void *requester = 0;
static read_callback_zmq m_on_read = 0;


result_t init_connection_zmq(read_callback_zmq onread)
{
    m_on_read = onread;
    return Ok;
}

result_t start_connection_zmq(char *address, int port)
{
    if(!address)
    {
        address = DEFAULT_ADDRESS;
    }
    if(!port)
    {
        port = DEFAULT_PORT;
    }

    char buff[100];
    sprintf(buff, "tcp://%s:%d", address, port);
    context = zmq_ctx_new();
    requester = zmq_socket(context, ZMQ_REQ);
    int rc = zmq_connect(requester, buff);
    if(rc != 0)
    {
        printf("failed to connect to zmq server: %s\n", strerror(errno));
        return Failed;
    }

    m_running = 1;
    if(pthread_create(&m_read_thread, 0, &thread_routine, 0) != 0)
    {
        printf("failed to run read thread: %s\n", strerror(errno));
        m_running = 0;
        return Failed;
    }

    return Ok;
}

result_t stop_connection_zmq()
{
    m_running = 0;
    pthread_join(m_read_thread, 0);

    zmq_close (requester);
    zmq_ctx_destroy(context);

    return Ok;
}

void* thread_routine(void *arg)
{
    char buffer[BUFFER_SIZE];
    while(m_running)
    {
        int bytes = zmq_recv(requester, buffer, BUFFER_SIZE, ZMQ_DONTWAIT);
        if(bytes > 0)
        {
            if(m_on_read)
            {
                m_on_read(buffer, bytes);
            }
        }
        else if(bytes < 0)
        {
            //if(errno != EAGAIN)
            //{
            //    printf("zmq read failed: %s", strerror(errno));
            //    m_running = 0;
            //}
        }
    }

    return 0;
}

result_t write_connection_zmq(char *data, int len)
{
    if(zmq_send(requester, data, len, 0) != 0)
    {
        printf("failed to send to zmq server: %s\n", strerror(errno));
        return Failed;
    }
    return Ok;
}

result_t write_connection_zmq_short(char *data)
{
    return write_connection_zmq(data, strlen(data));
}
