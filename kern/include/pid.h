#ifndef _PID_H_
#define _PID_H_

int pid_setup();

int pid_alloc(struct proc*);

int pid_dealloc();

#endif
