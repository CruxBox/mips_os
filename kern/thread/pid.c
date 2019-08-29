#include <types.h>
#include <spinlock.h>
#include <thread.h>
#include <current.h>
#include <proc.h>
#include <synch.h>
#include <pid.h>

#define MAX_NO_PID 256

static struct pid *pids[MAX_NO_PID];
static struct lock *pid_lock;

struct pid *pid_create(pid_t p_pid, int index)
{
    KASSERT(!(index < 1 || index >= MAX_NO_PID));

    struct pid *temp = kmalloc(sizeof(struct pid));

    KASSERT(temp != NULL);

    temp->pid = index;
    temp->p_pid = p_pid;

    return temp;
}

void pid_bootstrap()
{

    pid_lock = lock_create("pid_lock");

    if (pid_lock == NULL)
    {
        panic("pid lock not created\n");
    }
    /* No need to call the lock. It's bootstrap process so no process will be interfering anyway */

    for (uint16_t i = 0; i < MAX_NO_PID; i++)
    {
        pids[i] = NULL;
    }

    pids[BOOTPROC_ID] = pid_create(INVALID_ID, BOOTPROC_ID);
}

int pid_alloc(pid_t p_pid)
{
    lock_acquire(pid_lock);
    uint16_t i;

    for (i = 2; i < MAX_NO_PID; i++)
    {
        if (pids[i] == NULL)
        {
            pids[i] = pid_create(p_pid, i);

            lock_release(pid_lock);

            return i;
        }
    }

    //Shouldn't reach here.
    lock_release(pid_lock);
    return INVALID_ID;
}

void pid_dealloc(pid_t pid)
{
    lock_acquire(pid_lock);

    if (pid < 0 || pid >= MAX_NO_PID)
        panic("Invalid range of pid\n");

    KASSERT(pid != INVALID_ID);

    struct pid *temp = pids[pid];

    kfree(temp);

    pids[pid] = NULL;
    lock_release(pid_lock);
}