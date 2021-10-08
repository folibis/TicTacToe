#ifndef CLIENTS_H
#define CLIENTS_H

#include "common.h"


typedef struct Client
{
    int connID;
    int shape;
} Client;

extern result_t init_clients();
extern result_t add_client(int connID);
extern result_t remove_client(int connID);
extern int get_client_count();
extern result_t is_ready();
extern result_t send_ready();
extern result_t send_your_turn();
extern result_t send_waiting();
extern result_t send_turn(int id, int row, int col);
extern result_t next();
extern result_t send_win(int id, int reason);
extern result_t send_lose(int reason);
extern int get_current_client();
extern result_t client_send_incorrect(int id);

#endif // CLIENTS_H
