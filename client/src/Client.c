#include <stdio.h>
#include "Window.h"
#include "Client.h"
#include "Connection.h"
#include "Protocol.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1234

static void on_read(char *data, int len);
static result_t process_message(message_t *message);

int GameStarted = 0;
static char *m_address = DEFAULT_ADDRESS;
static int m_port = DEFAULT_PORT;
static int m_my_turn = 0;
static int m_field[3][3];
static int my_shape = (-1);


result_t init_client()
{
    clear_client();
    init_connection(&on_read);
    draw_screen();
    return Ok;
}

result_t clear_client()
{
    for(int i = 0;i < 3;i ++)
    {
        for(int j = 0;j < 3;j ++)
        {
            m_field[i][j] = (-1);
        }
    }

    return Ok;
}

result_t run_client(char *address, int port)
{
    if(!address)
    {
        m_address = DEFAULT_ADDRESS;
    }
    if(!port)
    {
        m_port = DEFAULT_PORT;
    }

    return Ok;
}

result_t stop_client()
{
    close_screen();
    return close_connection();
}

void start_game()
{
    if(start_connection(m_address, m_port) != Ok)
    {
        status("failed to connect to server");
        return;
    }
    status("connecting to server...");
}


void on_read(char *data, int len)
{
    int pos = 0;
    int msize = message_size();
    message_t message;

    while((len - pos) >= msize)
    {
        if(array2message(data + pos, msize, &message) == Ok)
        {
            process_message(&message);
        }

        pos += msize;
    }
}

result_t process_message(message_t *message)
{
    switch(message->type)
    {
        case MessageWait:
            {
                status("waiting for opponent...");
                clear_client();
                clear_field();
            }
            break;
        case MessageGameStart:
            {
                message_start_t start;
                to_start(message, &start);
                my_shape = start.shape;
                title("Press arrows to move, Space to make a turn, Backspace to exit");
                char str[20];
                sprintf(str, "Your shape: %s", get_shape(start.shape));
                subtitle(str);
                status("");
                GameStarted = 1;
            }
            break;
        case MessageYourTurn:
            {
                message_your_turn_t your_turn;
                to_your_turn(message, &your_turn);
                if(your_turn.shape == my_shape)
                {
                    m_my_turn = 1;
                    status("This is your turn");
                }
                else
                {
                    m_my_turn = 0;
                    status("This is your opponent's turn");
                }
            }
            break;
        case MessageTurn:
            {
                message_turn_t turn;
                to_turn(message, &turn);
                m_field[turn.row][turn.col] = turn.shape;
                put_shape(turn.row, turn.col, turn.shape);
            }
            break;
        case MessageFinish:
            {
                message_game_finish_t message_finish;
                to_finish(message, &message_finish);
                if(message_finish.winner_id == (-1))
                {
                    subtitle("Game over, it's draw!");
                }
                else
                {
                    if(my_shape == message_finish.winner_id)
                    {
                        subtitle("Game over, you win!");
                    }
                    else
                    {
                        subtitle("Game over, you lose!");
                    }
                }

                title("Press Backspace to exit");
                switch(message_finish.reason)
                {
                    case ReasonWin:
                        status("See ya later");
                        break;
                    case ReasonTimeout:
                        status("Time out");
                        break;
                }

                GameStarted = 0;
            }
            break;
        case MessageIncorrectTurn:
            status("Incorrect turn!");
            break;
        case MessageBusy:
            status("The server is full, try later");
            break;
    }

    return Ok;
}

result_t is_my_turn()
{
    return m_my_turn == 1 ? Ok : Failed;
}

result_t is_allowed(int row, int col)
{
    if(m_field[row][col] == (-1))
    {
        return Ok;
    }

    return Failed;
}

result_t turn(int row, int col)
{
    message_turn_t turn;
    message_t message;

    turn.shape = my_shape;
    turn.row = row;
    turn.col = col;

    from_turn(&turn, &message);
    char buffer[MessageSize];

    if(message2array(&message, buffer, MessageSize) == Ok)
    {
        m_my_turn = 0;
        return write_connection(buffer, MessageSize);
    }
    status("waiting for response...");

    return Failed;
}
