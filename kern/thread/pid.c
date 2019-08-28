#include <types.h>
#include <spinlock.h>
#include <thread.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#define MAX_NO_PID 256

struct pid* pids[MAX_NO_PID];
struct lock* pid_lock;

void pid_bootstrap()
{

    pid_lock = lock_create("pid_lock");

    if (pid_lock == NULL)
    {
        panic("pid lock not created\n");
    }

    for (uint16_t i = 1; i < MAX_NO_PID; i++)
    {
        pids[i] = NULL;
    }
}
int pid_alloc(struct thread *th)
{
    lock_acquire(&pid_lock);
    uint16_t i;

    for (i = 1; i < MAX_NO_PID; i++)
    {
        if (pids[i] != NULL)
        {
            pids[i] = 1;
            th->t_pid = i;

            lock_release(&pid_lock);

            return i;
        }
    }
    //Shouldn't reach here.
    lock_release(&pid_lock);
    panic('Cannot create more pids\n');
}

void pid_dealloc(struct thread *th)
{
    uint16_t i;
    lock_acquire(&pid_lock);

    i = th->t_pid;

    if (i < 0 || i >= MAX_NO_PID)
        panic("Invalid range of pid\n");

    pids[i] = 0;

    th->t_pid = -1;

    lock_release(&pid_lock);
}