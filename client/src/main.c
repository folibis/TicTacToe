#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "Client.h"

typedef enum ParseAction
{
    None = 0,
    Address,
    Port,
    Help,
} ParseAction;

void handle_sigint(int sig)
{
    stop_client();
}

void PrintUsage(char *app)
{
    printf("TicTacToe client \n");
    printf("Usage: \n");
    printf("%s [-a address] [-p port] [-h] \n", app);
    printf("where: \n");
    printf("\t-a address - set the server address\n");
    printf("\t-p port - set the server port\n");
    printf("\t-h - print this help\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handle_sigint);

    char *address = 0;
    int port = 0;

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
                    case None:
                    case Help:
                    defaut: break;
                }
                action = None;
            }
        }
    }

    init_client();
    run_client(address, port);

    return 0;
}
