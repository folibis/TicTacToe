#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "Window.h"
#include "Client.h"

#define CELL_COUNT 3
#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_LEFT 68
#define KEY_RIGHT 67
#define KEY_BKSPACE 127
#define KEY_ENTER 10
#define KEY_SPACE 32
#define TITLE_ROW 1
#define SUBTITLE_ROW 2
#define STATUS_ROW 17

static void reset();
static void hide_cursor();
static void show_cursor();
static void clrscr();
static void put_text(unsigned int row, unsigned int col, char *text);
static void draw_frame();
static void set_color(Colors color);
static void clear_color();
static void getch_start();
static void getch_restore();
static void move(Moving dir);
static void make_turn();

static char window[13][20] = {
    {"###################"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"###################"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"###################"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"#     #     #     #"},
    {"###################"},
};

static char shapes[2][2] = {"X","O"};
static char colors[7][15] = { "\033[0;32m","\033[0;31m", "\033[0;33m", "\033[0;34m", "\033[0;35m", "\033[0;36m", "\033[0m" };
static struct termios oldt;
static int running = 0;
static int cur_row = -1;
static int cur_col = -1;


result_t draw_screen()
{
    setbuf(stdout, 0);
    hide_cursor();
    clrscr();
    draw_frame();
    put_text(1, 1, "Press SPACE to connect to server, BACKSPACE to exit");
    run();
    return Ok;
}

result_t close_screen()
{
    show_cursor();
    reset();
    getch_restore();
    clrscr();
    return Ok;
}

result_t clear_field()
{
    for(int r = 0;r < CELL_COUNT;r ++)
    {
        for(int c = 0;c < CELL_COUNT;c ++)
        {
            put_shape(r, c, -1);
            select_cell(r, c, 0);
        }
    }

    cur_row = -1;
    cur_col = -1;

    return Ok;
}

result_t put_shape(unsigned int y, unsigned int x, int shape)
{
    int row = y * 4 + 5;
    int col = x * 6 + 4;
    set_color(Green);
    if(shape == (-1))
    {
        put_text(row, col, " ");
    }
    else
    {
        put_text(row, col, shapes[shape]);
    }
    clear_color();
    return Ok;
}

void draw_frame()
{
    set_color(Blue);
    for(int i = 0;i < 13;i ++)
    {
        put_text(i + 3, 1, window[i]);
    }
    clear_color();
}

void reset()
{
    printf("\033[0m");
}

void hide_cursor()
{
    printf("\033[?25l");
}

void show_cursor()
{
    printf("\033[?25h");
}

void clrscr()
{
    reset();
    printf("\033[1;1H\033[2J");
}

void put_text(unsigned int row, unsigned int col, char *text)
{
    printf("\033[%d;%dH", row, col);
    printf("%s", text);
}

void set_color(Colors color)
{
    printf("%s", colors[color]);
}

void clear_color()
{
    printf("%s", colors[Clear]);
}

result_t select_cell(unsigned int y, unsigned int x, int set)
{
    int row = x * 4 + 4;
    int col = y * 6 + 2;
    char s[2] = " ";

    if(set)
    {
        s[0] = '#';
        set_color(Yellow);
    }
    else
    {
        clear_color();
    }

    for(int i = 0;i < 5;i ++)
    {
        printf("\033[%d;%dH", row, col + i);
        printf("%s", s);
        printf("\033[%d;%dH", row + 2, col + i);
        printf("%s", s);
    }

    printf("\033[%d;%dH", row + 1, col);
    printf("%s", s);
    printf("\033[%d;%dH", row + 1, col + 4);
    printf("%s", s);
    clear_color();
    return Ok;
}

void getch_start()
{
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void getch_restore()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void run()
{
    running = 1;
    int ch = 0;
    getch_start();
    while(running)
    {
        ch = getchar();
        if(ch > 0)
        {
            switch(ch)
            {
                case KEY_UP:
                    move(Up);
                    break;
                case KEY_DOWN:
                    move(Down);
                    break;
                case KEY_RIGHT:
                    move(Right);
                    break;
                case KEY_LEFT:
                    move(Left);
                    break;
                case KEY_BKSPACE:
                    running = 0;
                    break;
                case KEY_SPACE:
                    if(GameStarted)
                    {
                        make_turn();
                    }
                    else
                    {
                        start_game();
                    }
                    break;
            }
        }
    }

    close_screen();
}

void move(Moving dir)
{
    if(!GameStarted)
    {
        return;
    }

    if(cur_col < 0 || cur_row < 0)
    {
        cur_col = 0;
        cur_row = 0;
        select_cell(cur_col, cur_row, 1);
        return;
    }

    select_cell(cur_col, cur_row, 0);

    switch(dir)
    {
        case Left:
            if(cur_col > 0)
            {
                cur_col --;
            }
            break;
        case Right:
            if(cur_col < 2)
            {
                cur_col ++;
            }
            break;
        case Up:
            if(cur_row > 0)
            {
                cur_row --;
            }
            break;
        case Down:
            if(cur_row < 2)
            {
                cur_row ++;
            }
            break;
        defaut: break;
    }

    if(cur_col >= 0 && cur_row >= 0)
    {
        select_cell(cur_col, cur_row, 1);
    }
}

void status(char *text)
{
    clear_color();
    put_text(STATUS_ROW, 1, "                                                                   ");
    set_color(Red);
    put_text(STATUS_ROW, 1, text);
    clear_color();
}

void make_turn()
{
    if(is_my_turn() == Failed)
    {
        status("not your turn");
        return;
    }

    if(!is_allowed(cur_row, cur_col))
    {
        status("not allowed");
        return;
    }

    if(turn(cur_row, cur_col) != Ok)
    {
        status("failed to send to server");
    }

}

void title(char *text)
{
    clear_color();
    put_text(TITLE_ROW, 1, "                                                                 ");
    set_color(Yellow);
    put_text(TITLE_ROW, 1, text);
    clear_color();
}

void subtitle(char *text)
{
    clear_color();
    put_text(SUBTITLE_ROW, 1, "                                                                 ");
    set_color(Cyan);
    put_text(SUBTITLE_ROW, 1, text);
    clear_color();
}

char *get_shape(int shape)
{
    return shapes[shape];
}
