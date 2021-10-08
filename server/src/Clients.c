#include "Protocol.h"
#include "Connection.h"
#include "Clients.h"

#define CLIENT_COUNT 2

static void swap(int i1, int i2);
static void assign_shapes();

static Client m_clients[CLIENT_COUNT];
static int m_client_count = 0;
static int current_client = (-1);


result_t init_clients()
{
    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        m_clients[i].connID = (-1);
        m_clients[i].shape = (-1);
    }

    return Ok;
}

result_t add_client(int connID)
{
    if(m_client_count < CLIENT_COUNT)
    {
        m_clients[m_client_count].connID = connID;
        m_client_count ++;

        if(m_client_count == (CLIENT_COUNT - 1))
        {
            assign_shapes();
            current_client = 0;
        }

        return Ok;
    }

    return Failed;
}

result_t remove_client(int connID)
{
    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        if(m_clients[i].connID == connID)
        {
            m_clients[i].connID = (-1);
            m_clients[i].shape = (-1);
            current_client = (-1);
            m_client_count --;

            if(i == (CLIENT_COUNT - 1))
            {
                swap(0, i);
            }
            send_waiting();
            return Ok;
        }
    }

    return Failed;
}

int get_client_count()
{
    return m_client_count;
}

result_t is_ready()
{
    return ((m_client_count == CLIENT_COUNT) ? Ok : Failed);
}

void swap(int i1, int i2)
{
    if(i1 != i2)
    {
        Client t = m_clients[i1];
        m_clients[i1] = m_clients[i2];
        m_clients[i2] = t;
    }
}

void assign_shapes()
{
    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        m_clients[i].shape = i;
    }
}

result_t send_ready()
{
    message_t message;
    message_start_t message_start;
    char buffer[MessageSize];

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        message_start.shape = m_clients[i].shape;
        from_start(&message_start, &message);
        message2array(&message, buffer, MessageSize);
        write_connection(m_clients[i].connID, buffer, MessageSize);
    }

    return Ok;
}

result_t send_your_turn()
{
    message_t message;
    message_your_turn_t message_your_turn;
    message_your_turn.shape = current_client;
    char buffer[MessageSize];
    from_your_turn(&message_your_turn, &message);
    message2array(&message, buffer, MessageSize);

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        if(write_connection(m_clients[i].connID, buffer, MessageSize) != Ok)
        {
            return Failed;
        }
    }

    return Ok;
}

result_t send_waiting()
{
    message_t message;
    message_waiting_t message_waiting;
    char buffer[MessageSize];

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        if(m_clients[i].connID > 0)
        {
            from_waiting(&message_waiting, &message);
            message2array(&message, buffer, MessageSize);
            write_connection(m_clients[i].connID, buffer, MessageSize);
        }
    }

    return Ok;
}

result_t send_turn(int id, int row, int col)
{
    message_t message;
    message_turn_t message_turn;
    char buffer[MessageSize];

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        if(m_clients[i].connID >= 0)
        {
            message_turn.shape = id;
            message_turn.row = row;
            message_turn.col = col;
            from_turn(&message_turn, &message);
            message2array(&message, buffer, MessageSize);
            write_connection(m_clients[i].connID, buffer, MessageSize);
        }
    }

    return Ok;
}

result_t next()
{
    current_client ++;
    if(current_client >= CLIENT_COUNT)
    {
        current_client = 0;
    }
    return send_your_turn();
}

result_t send_win(int id, int reason)
{
    message_t message;
    message_game_finish_t message_finish;
    message_finish.winner_id = id;
    message_finish.reason = reason;
    char buffer[MessageSize];
    from_finish(&message_finish, &message);
    message2array(&message, buffer, MessageSize);

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        write_connection(m_clients[i].connID, buffer, MessageSize);
    }

    return Ok;
}

result_t send_lose(int reason)
{
    message_t message;
    message_game_finish_t message_finish;
    message_finish.reason = reason;

    char buffer[MessageSize];

    for(int i = 0;i < CLIENT_COUNT;i ++)
    {
        if(i == current_client)
        {
            message_finish.winner_id = 0x00FF;
        }
        else
        {
            message_finish.winner_id = i;
        }
        from_finish(&message_finish, &message);
        message2array(&message, buffer, MessageSize);
        write_connection(m_clients[i].connID, buffer, MessageSize);
    }

    return Ok;
}

int get_current_client()
{
    return current_client;
}

result_t client_send_incorrect(int id)
{
    message_t message;
    message_incorrect_t message_incorrect;
    char buffer[MessageSize];
    from_incorrect(&message_incorrect, &message);
    message2array(&message, buffer, MessageSize);
    return write_connection(m_clients[id].connID, buffer, MessageSize);
}
