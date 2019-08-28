#ifndef _KERN_REBOOT_H_
#define _KERN_REBOOT_H_

/*
 * Constants for libc's <sys/reboot.h> and the reboot() system call.
 * (Not all that important.)
 */


/* Codes for reboot */
#define RB_REBOOT     0      /* Reboot system */
#define RB_HALT       1      /* Halt system and do not reboot */
#define RB_POWEROFF   2      /* Halt system and power off */


#endif /* _KERN_REBOOT_H_ */
