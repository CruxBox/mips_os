#ifndef _PID_H_
#define _PID_H_

struct pid
{
    pid_t pid;
    pid_t t_pid;
    pid_t ppid;
    struct thread *thread;
};

void pid_bootstrap();

int pid_alloc(struct thread *);

int pid_dealloc(struct thread *);

#endif
