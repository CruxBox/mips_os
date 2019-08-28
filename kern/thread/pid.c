#include <types.h>
#include <spinlock.h>
#include <thread.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#define MAX_NO_PID 256

static uint16_t *pids[MAX_NO_PID];
struct lock pid_array;

int pid_setup()
{

    for (uint16_t i = 0; i < MAX_NO_PID; i++)
    {
        pids[i] = 0;
    }

    pid_array = lock_create("pid_lock");
}
int pid_alloc(struct thread *th)
{
    lock_acquire(&pid_array);
    uint16_t i;

    for (i = 0; i < MAX_NO_PID; i++)
    {
        if (pids[i] != NULL)
        {
            pids[i] = 1;
            th->t_pid = i;
            break;
        }
    }

    if (i == MAX_NO_PID)
    {
        lock_release(&pid_array);
        panic('Cannot create more pids\n');
    }

    lock_release(&pid_array);
}

int pid_dealloc(struct thread *th)
{
    uint16_t i;
    lock_acquire(&pid_array);

    i = th->t_pid;
    pids[i] = 0;

    th->t_pid = -1;

    lock_release(&pid_array);
}

