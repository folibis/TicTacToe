#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

extern int GameStarted;

extern result_t init_client();
extern result_t clear_client();
extern result_t run_client(char *address, int port);
extern result_t stop_client();
extern void start_game();
extern result_t is_my_turn();
extern result_t is_allowed(int row, int col);
extern result_t turn(int row, int col);

#endif // CLIENT_H
