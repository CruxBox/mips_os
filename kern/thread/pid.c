#include <types.h>
#include <spinlock.h>
#include <thread.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#include <pid.h>

#define MAX_NO_PID 256

struct pid *pids[MAX_NO_PID];
struct lock *pid_lock;

void pid_create(pid_t p_pid, int index)
{
    KASSERT(lock_do_i_hold(pid_lock));

    struct pid *temp;

    temp->pid = index;
    temp->p_pid = p_pid;

    pids[index] = temp;
}

void pid_bootstrap()
{

    pid_lock = lock_create("pid_lock");

    if (pid_lock == NULL)
    {
        panic("pid lock not created\n");
    }

    for (uint16_t i = 0; i < MAX_NO_PID; i++)
    {
        pids[i] = NULL;
    }

    pid_create(INVALID_ID, BOOTPROC_ID);
}

int pid_alloc(pid_t p_pid)
{
    lock_acquire(&pid_lock);
    uint16_t i;

    for (i = 2; i < MAX_NO_PID; i++)
    {
        if (pids[i] == NULL)
        {
            pid_create(p_pid, i);

            lock_release(&pid_lock);

            return i;
        }
    }
    //Shouldn't reach here.
    lock_release(&pid_lock);
    panic('Cannot create more pids\n');
}

void pid_dealloc(pid_t pid)
{
    uint16_t i;
    lock_acquire(&pid_lock);

    if (pid < 0 || i >= MAX_NO_PID)
        panic("Invalid range of pid\n");

    pids[pid] = NULL;

    lock_release(&pid_lock);
}