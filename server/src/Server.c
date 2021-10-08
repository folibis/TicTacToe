#include <string.h>
#include <stdlib.h>
#include "Connection.h"
#include "ConnectionZmq.h"
#include "Protocol.h"
#include "Clients.h"
#include "Server.h"
#include "List.h"
#include <Timer.h>

#define DEFAULT_ADDRESS "*"
#define DEFAULT_PORT 1234
#define TURN_TIMEOUT 10000

static void on_connected(int connID, char *address, int port);
static void on_read(int connID, char *data, int len);
static void on_close(int connID);
static void process_message(int connID, message_t *message);
static void on_read_zmq(char *data, int len);
static char *get_value(char *tokens, char *key);
static void process_init_game(char *ok);
static void process_turn(char *id_, char *row_, char *col_);
static void process_finish(int id);
static void on_timeout();

static int timer_id;


result_t init_server()
{
    if(init_connection(&on_connected, &on_read, &on_close) != Ok)
    {
        return Failed;
    }

    if(init_connection_zmq(&on_read_zmq) != Ok)
    {
        return Failed;
    }

    return Ok;
}

result_t run_server(char *address, int port, char *address_zmq, int port_zmq)
{
    if(!address)
    {
        address = DEFAULT_ADDRESS;
    }
    if(!port)
    {
        port = DEFAULT_PORT;
    }


    if(start_connection(address, port) != Ok)
    {
        return Failed;
    }

    if(start_connection_zmq(address_zmq, port_zmq) != Ok)
    {
        return Failed;
    }

    timer_id = add_timer(&on_timeout, TURN_TIMEOUT, 0, 0);
    wait_connection();

    return Ok;
}

result_t stop_server()
{
    stop_connection_zmq();
    close_all_timers();
    return stop_connection();
}

void on_connected(int connID, char *address, int port)
{
    if(is_ready() == Ok)
    {
        message_t message;
        message_busy_t message_busy;
        message_busy.reason = ReasonOverflow;
        char buffer[MessageSize];
        from_busy(&message_busy, &message);
        message2array(&message, buffer, MessageSize);
        write_connection(connID, buffer, MessageSize);
    }
    else
    {
        if(add_client(connID) == Ok)
        {
            if(is_ready() == Ok)
            {
                write_connection_zmq_short("command: init_game");
            }
            send_waiting();
        }
    }
}

void on_read(int connID, char *data, int len)
{
    message_t message;
    int pos = 0;

    while((pos + MessageSize) <= len)
    {
        array2message(data + pos, MessageSize, &message);
        pos += MessageSize;
        process_message(connID, &message);
    }
}

void process_message(int connID, message_t *message)
{
    switch(message->type)
    {
        case MessageTurn:
            {
                message_turn_t message_turn;
                to_turn(message, &message_turn);
                if(message_turn.shape == get_current_client())
                {
                    char str[100];
                    sprintf(str, "command: turn, id: %d, row: %d, col:%d", message_turn.shape, message_turn.row, message_turn.col);
                    write_connection_zmq_short(str);
                }
                else
                {
                    client_send_incorrect(message_turn.shape);
                }
            }
            break;
    }
}

void on_close(int connID)
{
    remove_client(connID);
    stop_timer(timer_id);
}

void on_read_zmq(char *data, int len)
{
    DictNode*dict = create_dict(data, ",", ":");

    char *command = get_dict_value(dict, "command");
    if(strcmp(command, "init_game") == 0)
    {
        char *response = get_dict_value(dict, "response");
        process_init_game(response);
    }
    else if(strcmp(command, "turn") == 0)
    {
        char *error = get_dict_value(dict, "error");
        if(error != NULL)
        {
            printf("wrong turn parameters: %s\n", error);
        }
        else
        {
            char *id = get_dict_value(dict, "id");
            char *col = get_dict_value(dict, "col");
            char *row = get_dict_value(dict, "row");

            process_turn(id, row, col);

            char *win = get_dict_value(dict, "win");
            if(win != NULL)
            {
                int win_id = atoi(win);
                process_finish(win_id);
            }
        }
    }

    remove_dict(dict);
}

char *get_value(char *data, char *key)
{
    char *tokens = strtok(data, ",");

    while(tokens != NULL)
    {
        if(strstr(tokens, key))
        {
            char *pos = strstr(tokens, ":");
            if(pos)
            {
                pos++;
                while(*pos == ' ')
                    pos ++;
                return pos;
            }
        }
        tokens = strtok(NULL, ",");
    }
    return NULL;
}

void process_init_game(char *ok)
{
    if(ok != NULL && strcmp(ok, "ok") == 0)
    {
        send_ready();
        send_your_turn();
        reset_timer(timer_id);
        start_timer(timer_id);
    }
}

void process_turn(char *id_, char *row_, char *col_)
{
    int id = atoi(id_);
    int row = atoi(row_);
    int col = atoi(col_);

    send_turn(id, row, col);
    next();
    reset_timer(timer_id);
}

void process_finish(int id)
{
    send_win(id, ReasonWin);
    stop_timer(timer_id);
}

void on_timeout()
{
    send_lose(ReasonTimeout);
    stop_timer(timer_id);
}
