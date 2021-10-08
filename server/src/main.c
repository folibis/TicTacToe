#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "Server.h"


typedef enum ParseAction
{
    None = 0,
    Address,
    Port,
    AddressZmq,
    PortZmq,
    Help,
} ParseAction;

void handle_sigint(int sig)
{
    stop_server();
}

void PrintUsage(char *app)
{
    printf("TicTacToe server \n");
    printf("Usage: \n");
    printf("%s [-a address] [-p port] [-h] [-az address] [-pz port] \n", app);
    printf("where: \n");
    printf("\t-a address - bind the server to this address\n");
    printf("\t-p port - run the server port on this port\n");
    printf("\t-az address - connect to the zmq server on this address\n");
    printf("\t-px port - connect to the zmq server on this port\n");
    printf("\t-h - print this help\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handle_sigint);

    char *address = 0;
    int port = 0;
    char *address_zmq = 0;
    int port_zmq = 0;

    if(argc > 1)
    {
        ParseAction action;
        for(int i = 1;i < argc;i ++)
        {
            if(strcmp("-a", argv[i]) == 0)
            {
                action = Address;
            }
            else if(strcmp("-p", argv[i]) == 0)
            {
                action = Port;
            }
            else if(strcmp("-h", argv[i]) == 0)
            {
                PrintUsage(argv[0]);
            }
            else if(strcmp("-az", argv[i]) == 0)
            {
                action = AddressZmq;
            }
            else if(strcmp("-pz", argv[i]) == 0)
            {
                action = PortZmq;
            }
            else
            {
                switch(action)
                {
                    case Address:
                        address = argv[i];
                        break;
                    case Port:
                        port = atoi(argv[i]);
                        break;
                    case AddressZmq:
                        address_zmq = argv[i];
                        break;
                    case PortZmq:
                        port_zmq = atoi(argv[i]);
                        break;
                    case None:
                    case Help:
                    defaut: break;
                }
                action = None;
            }
        }
    }

    if(init_server() == Ok)
    {
        return run_server(address, port, address_zmq, port_zmq) == Ok ? 0 : 1;
    }

    return 1;
}
