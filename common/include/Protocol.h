#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"


typedef enum MessageType
{
    MessageUndefined,
    MessageTurn,
    MessageGameStart,
    MessageWait,
    MessageYourTurn,
    MessageFinish,
    MessageIncorrectTurn,
    MessageBusy,
} MessageType;

typedef enum ErrorReason
{
    ReasonUndefined,
    ReasonWin,
    ReasonTimeout,
    ReasonServerFail,
    ReasonOverflow,
} ErrorReason;

typedef struct message_t
{
    int type;
    int payload1;
    int payload2;
} message_t;

typedef struct message_start_t
{
    int shape;
} message_start_t;

typedef struct message_turn_t
{
    int shape;
    int row;
    int col;
} message_turn_t;

typedef struct message_your_turn_t
{
    int shape;
} message_your_turn_t;

typedef struct message_waiting_t
{
} message_waiting_t;

typedef struct message_game_finish_t
{
    int winner_id;
    int reason;
} message_game_finish_t;

typedef struct message_incorrect_t
{
} message_incorrect_t;

typedef struct message_busy_t
{
    int reason;
} message_busy_t;

extern const int MessageSize;

extern int message_size();
extern result_t array2message(char *data, int len, message_t *message);
extern result_t message2array(message_t *message, char *data, int len);
extern result_t to_start(message_t *message, message_start_t *start_message);
extern result_t from_start(message_start_t *start_message, message_t *message);
extern result_t to_turn(message_t *message, message_turn_t *turn);
extern result_t from_turn(message_turn_t *turn, message_t *message);
extern result_t to_your_turn(message_t *message, message_your_turn_t *turn);
extern result_t from_your_turn(message_your_turn_t *turn, message_t *message);
extern result_t to_waiting(message_t *message, message_waiting_t *waiting);
extern result_t from_waiting(message_waiting_t *waiting, message_t *message);
extern result_t to_finish(message_t *message, message_game_finish_t *finish);
extern result_t from_finish(message_game_finish_t *finish, message_t *message);
extern result_t to_incorrect(message_t *message, message_incorrect_t *incorrect);
extern result_t from_incorrect(message_incorrect_t *incorrect, message_t *message);
extern result_t to_busy(message_t *message, message_busy_t *incorrect);
extern result_t from_busy(message_busy_t *incorrect, message_t *message);

#endif // PROTOCOL_H
