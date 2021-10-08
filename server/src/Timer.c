#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "Timer.h"

#define SLEEP_DELAY 500000 // 500 ms


typedef struct Timer
{
    int id;
    int active;
    timeout_callback calback;
    int loop;
    unsigned int delay;
    unsigned int counter;
    struct Timer *next;
} Timer;

static result_t run_thread();
static void* thread_routine();
static int has_timers();
static void send_signal();
static void decrement_timers();

static pthread_t m_thread;
static pthread_mutex_t m_listMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t m_condition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t m_conditionMutex = PTHREAD_MUTEX_INITIALIZER;
static int m_running = 0;
static Timer *ptr = NULL;
static int g_counter = 1;


int add_timer(timeout_callback callback, unsigned int delay, int loop, int active)
{
    pthread_mutex_lock(&m_listMutex);
    Timer *new_timer = malloc(sizeof(Timer));
    new_timer->id = g_counter++;
    new_timer->active = active;
    new_timer->calback = callback;
    new_timer->delay = delay * 1000;
    new_timer->counter = new_timer->delay;
    new_timer->loop = loop;
    new_timer->next = NULL;

    if(ptr == NULL)
    {
        ptr = new_timer;
        run_thread();
    }
    else
    {
        Timer *curr = ptr;
        while(curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_timer;
    }

    pthread_mutex_unlock(&m_listMutex);
    send_signal();

    return new_timer->id;
}

result_t reset_timer(int id)
{
    pthread_mutex_lock(&m_listMutex);
    Timer *curr = ptr;
    while(curr != NULL)
    {
        if(curr->id == id)
        {
            curr->counter = curr->delay;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&m_listMutex);

    return Ok;
}

result_t start_timer(int id)
{
    pthread_mutex_lock(&m_listMutex);
    Timer *curr = ptr;
    while(curr != NULL)
    {
        if(curr->id == id)
        {
            curr->active = 1;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&m_listMutex);
    send_signal();
    return Ok;
}

result_t stop_timer(int id)
{
    pthread_mutex_lock(&m_listMutex);
    Timer *curr = ptr;
    while(curr != NULL)
    {
        if(curr->id == id)
        {
            curr->active = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&m_listMutex);

    return Ok;
}

result_t close_all_timers()
{
    m_running = 0;
    pthread_mutex_lock(&m_listMutex);
    Timer *curr = ptr;
    while(curr != NULL)
    {
        Timer *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    pthread_mutex_unlock(&m_listMutex);
    return Ok;
}

static result_t run_thread()
{
    m_running = 1;
    if(pthread_create(&m_thread, 0, &thread_routine, 0) != 0)
    {
        printf("failed to run timer thread: %s\n", strerror(errno));
        return Failed;
        m_running = 0;
    }

    return Ok;
}

static void* thread_routine()
{
    while(m_running)
    {
        pthread_mutex_lock(&m_conditionMutex);
        while(!has_timers() && m_running)
            pthread_cond_wait(& m_condition, & m_conditionMutex);
        pthread_mutex_unlock(&m_conditionMutex);

        while(has_timers() && m_running)
        {
            usleep(SLEEP_DELAY);
            decrement_timers();
        }
    }

    return NULL;
}

void decrement_timers()
{
    timeout_callback clbk = NULL;

    pthread_mutex_lock(&m_listMutex);

    Timer *curr = ptr;
    while(curr != NULL)
    {
        if(curr->active)
        {
            if(curr->counter >= 0 && curr->counter < SLEEP_DELAY)
            {
                curr->counter = 0;
                clbk = curr->calback;
                if(curr->loop)
                {
                    curr->counter = curr->delay;
                }
                else
                {
                    curr->active = 0;
                }
            }
            else
            {
                curr->counter -= SLEEP_DELAY;
            }
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&m_listMutex);

    if(clbk != NULL)
    {
        clbk();
    }
}

int has_timers()
{
    if(ptr == NULL)
    {
        return 0;
    }

    int retval = 0;

    pthread_mutex_lock(&m_listMutex);
    Timer *curr = ptr;
    while(curr != NULL)
    {
        if(curr->active)
        {
            retval = 1;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&m_listMutex);

    return retval;
}

void send_signal()
{
    pthread_mutex_lock(&m_conditionMutex);
    pthread_cond_signal(&m_condition);
    pthread_mutex_unlock(&m_conditionMutex);
}
