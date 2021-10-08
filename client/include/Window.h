#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"


typedef enum Colors
{
    Green,
    Red,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    Clear
} Colors;

typedef enum Moving
{
    Left,
    Right,
    Up,
    Down
} Moving;

extern result_t draw_screen();
extern result_t close_screen();
extern result_t clear_field();
extern void run();
extern result_t put_shape(unsigned int y, unsigned int x, int shape);
extern result_t select_cell(unsigned int y, unsigned int x, int set);
extern result_t unselect_cell(unsigned int y, unsigned int x);
extern void status(char *text);
extern void title(char *text);
extern void subtitle(char *text);
extern char* get_shape(int shape);

#endif // WINDOW_H
