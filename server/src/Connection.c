#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include "Connection.h"

#define QUEUE_SIZE 10
#define MAX_CLIENTS 10
#define POLL_TIMEOUT 1000
#define READ_BUFFER_SIZE 1024
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1234

static int m_socket = (-1);
static int m_initialized = 0;
static pthread_t m_read_thread;
static struct pollfd m_fds[MAX_CLIENTS + 1];
static pthread_mutex_t m_writeMutex = PTHREAD_MUTEX_INITIALIZER;
static int m_running = 0;
static connected_callback m_onconnect = 0;
static read_callback m_onread = 0;
static close_callback m_onclose = 0;
static char buffer_read[READ_BUFFER_SIZE];

static void* read_thread();


result_t init_connection(connected_callback onconnect, read_callback onread, close_callback onclose)
{
    m_onconnect = onconnect;
    m_onread = onread;
    m_onclose = onclose;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket == (-1))
    {
        printf("failed to init socket");
        return Failed;
    }

    int opt = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == (-1))
    {
        printf("failed to change socket options");
        return Failed;
    }

    return Ok;
}

result_t start_connection(char *address, int port)
{
    if(!address)
    {
        address = DEFAULT_ADDRESS;
    }
    if(!port)
    {
        port = DEFAULT_PORT;
    }

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    if(address == 0 || strlen(address) == 0 || strcmp(address, "*") == 0)
    {
        server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        server_sockaddr.sin_addr.s_addr = inet_addr(address);
    }

    if(bind(m_socket, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr)) == (-1))
    {
        printf("failed to bind socket: %s\n", strerror(errno));
        return Failed;
    }

    if(listen(m_socket, QUEUE_SIZE) == -1)
    {
        printf("failed to listen on socket: %s\n", strerror(errno));
        return Failed;
    }

    memset(m_fds, 0, sizeof(m_fds));
    for (int i = 0; i < (MAX_CLIENTS + 1); i++)
    {
        m_fds[i].fd = (-1);
    }

    // index 0 is the main socket
    m_fds[0].fd = m_socket;
    m_fds[0].events = POLLIN;
    m_fds[0].revents = 0;

    m_running = 1;
    if(pthread_create(&m_read_thread, 0, &read_thread, 0) != 0)
    {
        printf("failed to run read thread: %s\n", strerror(errno));
        return Failed;
        m_running = 0;
    }

    return Ok;
}

result_t close_connection(int connID)
{
    if(connID < 0 || connID > MAX_CLIENTS)
    {
        return Failed;
    }

    if(m_fds[connID].fd != (-1))
    {
        close(m_fds[connID].fd);
        m_fds[connID].fd = (-1);
        m_fds[connID].events = 0;
        m_fds[connID].revents = 0;

        return Ok;
    }

    return Failed;
}

result_t wait_connection()
{
    if(m_running)
    {
        pthread_join(m_read_thread, 0);
    }

    return Ok;
}

result_t stop_connection()
{
    if(m_running)
    {
        m_running = 0;
    }

    if(m_initialized)
    {
        close_connections();
        m_initialized = 0;
    }

    return Ok;
}

result_t close_connections()
{
    for (int i = 0; i <= MAX_CLIENTS; i++)
    {
        close_connection(i);
    }

    return Ok;
}

result_t write_connection(int connID, char *data, int len)
{
    if(connID < 0 || connID > MAX_CLIENTS)
    {
        printf("incorrect connection ID\n");
        return Failed;
    }

    int fd = m_fds[connID].fd;
    if(fd != (-1))
    {
        ssize_t sent = send(fd, data, len, MSG_NOSIGNAL);
        if(sent == (-1))
        {
            close_connection(fd);
            printf("failed to write\n");
            return Failed;
        }

        return Ok;
    }

    return Failed;
}

static void* read_thread()
{
    int retval;

    while(m_running)
    {
        retval = poll(m_fds, MAX_CLIENTS + 1, POLL_TIMEOUT);
        if(retval > 0)
        {
            for (int i = 0; i <= MAX_CLIENTS; i++)
            {
                if(m_fds[i].revents == 0)
                {
                    continue;
                }

                if(m_fds[i].revents == POLLIN)
                {
                    if (i == 0) // new client connected
                    {
                        int new_socket = accept(m_socket, NULL, NULL);
                        if (new_socket < 0)
                        {
                            close(new_socket);
                            continue;
                        }

                        fcntl(new_socket, F_SETFL, O_NONBLOCK);

                        for(int j = 1;j <= MAX_CLIENTS; j ++) // looks for free space for the new socket
                        {
                            if(m_fds[j].fd == (-1))
                            {
                                m_fds[j].fd = new_socket;
                                m_fds[j].events = POLLIN;
                                struct sockaddr_in client_sockaddr = {};
                                socklen_t len = sizeof(client_sockaddr);

                                getpeername(new_socket, (struct sockaddr *)&client_sockaddr, &len );
                                if(m_onconnect)
                                {
                                    m_onconnect(j, inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port));
                                }

                                break;
                            }
                        }
                    }
                    else // existing socket data received
                    {
                        int readMore = 0;
                        int isError = 0;
                        int pos = 0;
                        do
                        {
                            retval = recv(m_fds[i].fd, buffer_read + pos, READ_BUFFER_SIZE, MSG_DONTWAIT);
                            if (retval < 0)
                            {
                                if (errno == EWOULDBLOCK)
                                {
                                    readMore = 0;
                                }
                                else
                                {
                                    isError = 1;
                                    close_connection(i);
                                    if(m_onclose)
                                    {
                                        m_onclose(i);
                                    }
                                    readMore = 0;
                                }
                            }
                            else if(retval > 0)
                            {
                                pos += retval;
                            }
                            else // the peer connection probably has closed
                            {
                                readMore = 0;
                                isError = 1;
                                close_connection(i);
                                if(m_onclose)
                                {
                                    m_onclose(i);
                                }
                            }
                        }
                        while(readMore);

                        if(!isError)
                        {
                            if(m_onread)
                            {
                                m_onread(i, buffer_read, pos);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
