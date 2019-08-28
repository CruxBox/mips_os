#ifndef _KERN_SEEK_H_
#define _KERN_SEEK_H_

/*
 * Codes for lseek(), which are shared in libc between <fcntl.h> and
 * <unistd.h> and thus get their own file.
 *
 * These are pretty important. Back in the day (like 20+ years ago)
 * people would often just write the values 0, 1, and 2, but that's
 * really not recommended.
 */

#define SEEK_SET      0      /* Seek relative to beginning of file */
#define SEEK_CUR      1      /* Seek relative to current position in file */
#define SEEK_END      2      /* Seek relative to end of file */


#endif /* _KERN_SEEK_H_ */
