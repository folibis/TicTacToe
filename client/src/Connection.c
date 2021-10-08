#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "Connection.h"
#include "Window.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1234
#define POLL_TIMEOUT 1000
#define BUFFER_SIZE 1014

static result_t run_red_thread();
static void* read_thread();

static int m_socket = (-1);
static int m_initialized = 0;
static int m_running = 0;
static struct pollfd m_poll;
static pthread_t m_thread;
static char m_read_buffer[BUFFER_SIZE];
static callback m_read_callback = 0;


result_t init_connection(callback read_callback)
{
    m_read_callback = read_callback;
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket == (-1))
    {
        status("socket init error");
        return Failed;
    }

    m_initialized = 1;
    return Ok;
}

result_t start_connection(char *address, int port)
{
    struct hostent *hostinfo;
    struct sockaddr_in dest_addr;

    if(!m_initialized)
    {
        status("uninitialized connection");
        return Failed;
    }

    if(!address)
    {
        address = DEFAULT_ADDRESS;
    }
    if(!port)
    {
        port = DEFAULT_PORT;
    }

    hostinfo = gethostbyname(address);

    if(hostinfo == 0)
    {
        status("failed to get server address");
        return Failed;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);
    memset(&(dest_addr.sin_zero), 0, 8);

    if(connect(m_socket, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
    {
        status("failed to connect to server");
        return Failed;
    }

    m_poll.fd = m_socket;
    m_poll.events = POLLIN;

    if(run_red_thread() != Ok)
    {
        return Failed;
    }

    return Ok;
}

result_t close_connection()
{
    m_running = 0;
    close(m_socket);
    m_socket = (-1);
    m_initialized = 0;
    pthread_join(m_thread, 0);
    return Ok;
}

result_t write_connection(char *data, int len)
{
    if(!m_initialized)
    {
        return Failed;
    }

    size_t sentBytes = send(m_socket, data, len, MSG_NOSIGNAL);
    if(len != sentBytes)
    {
        status("failed to send to server");
        return Failed;
    }

    return Ok;
}

result_t run_red_thread()
{
    if(m_initialized)
    {
        m_poll.fd = m_socket;
        m_poll.events = POLLIN;

        m_running = 1;
        if(pthread_create(&m_thread, 0, &read_thread, 0) != 0)
        {
            status("failed to start read thread");
            m_running = 0;
            return Failed;
        }
    }

    return Ok;
}

void* read_thread()
{
    while(m_running)
    {
        int stat = poll(&m_poll, 1, POLL_TIMEOUT);
        if(stat > 0 && (m_poll.revents & POLLIN))
        {
            int pos = 0;
            int readMore = 0;
            do
            {
                ssize_t readBytes = recv(m_poll.fd, m_read_buffer + pos, BUFFER_SIZE, MSG_DONTWAIT);
                if (readBytes < 0)
                {
                    if (errno == EWOULDBLOCK)
                    {
                        readMore = 0;
                    }
                    else
                    {
                        status("the server connection error");
                        close_connection();
                    }
                }
                else if(readBytes > 0)
                {
                    readMore = 1;
                    pos += readBytes;
                }
                else // the peer connection probably has closed
                {
                    status("the server has disconnected");
                    close_connection();
                }
            }
            while(readMore);

            if(pos > 0 && m_read_callback)
            {
                m_read_callback(m_read_buffer, pos);
            }
        }
    }

    return 0;
}
