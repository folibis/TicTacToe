#include <string.h>
#include "Protocol.h"

const int MessageSize = sizeof(message_t);


result_t array2message(char *data, int len, message_t *message)
{
    if(len >= MessageSize)
    {
        memcpy(message, data, MessageSize);
        return Ok;
    }

    return Failed;
}

result_t message2array(message_t *message, char *data, int len)
{
    if(len >= MessageSize)
    {
        memcpy(data, message, MessageSize);
        return Ok;
    }

    return Failed;
}

int message_size()
{
    return MessageSize;
}

result_t to_start(message_t *message, message_start_t *start_message)
{
    start_message->shape = (char)message->payload1;
    return Ok;
}

result_t from_start(message_start_t *start_message, message_t *message)
{
    message->payload1 = start_message->shape;
    message->type = MessageGameStart;
    return Ok;
}

result_t from_turn(message_turn_t *turn, message_t *message)
{
    message->type = MessageTurn;
    message->payload1 = turn->shape;
    message->payload2 = (turn->row << 8) & 0x0FF00 | turn->col & 0x00FF;
    return Ok;
}

result_t to_turn(message_t *message, message_turn_t *turn)
{
    turn->shape = message->payload1;
    turn->col = message->payload2 & 0x00FF;
    turn->row = (message->payload2 >> 8) & 0x00FF;
    return Ok;
}

result_t to_your_turn(message_t *message, message_your_turn_t *turn)
{
    turn->shape = message->payload1;
    return Ok;
}

result_t from_your_turn(message_your_turn_t *turn, message_t *message)
{
    message->payload1 = turn->shape;
    message->type = MessageYourTurn;
    return Ok;
}

result_t to_waiting(message_t *message, message_waiting_t *waiting)
{
    return Ok;
}

result_t from_waiting(message_waiting_t *waiting, message_t *message)
{
    message->type = MessageWait;
    return Ok;
}

result_t to_finish(message_t *message, message_game_finish_t *finish)
{
    finish->winner_id = message->payload1;
    finish->reason = message->payload2;
    return Ok;
}

result_t from_finish(message_game_finish_t *finish, message_t *message)
{
    message->type = MessageFinish;
    message->payload1 = finish->winner_id;
    message->payload2 = finish->reason;
    return Ok;
}

result_t to_incorrect(message_t *message, message_incorrect_t *incorrect)
{
    return Ok;
}

result_t from_incorrect(message_incorrect_t *incorrect, message_t *message)
{
    message->type = MessageIncorrectTurn;
    return Ok;
}

result_t to_busy(message_t *message, message_busy_t *busy)
{
    busy->reason = message->payload1;
    return Ok;
}

result_t from_busy(message_busy_t *busy, message_t *message)
{
    message->type = MessageBusy;
    message->payload1 = busy->reason;
    return Ok;
}
