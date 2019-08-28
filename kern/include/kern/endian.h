#ifndef _KERN_ENDIAN_H_
#define _KERN_ENDIAN_H_

/*
 * Machine-independent and exported endianness definitions.
 *
 * Note: get these via <endian.h> in the kernel and <arpa/inet.h> in
 * userland.
 *
 * This is the historic BSD way of defining endianness.
 */

#define _LITTLE_ENDIAN 1234
#define _BIG_ENDIAN    4321
#define _PDP_ENDIAN    3412

/* This defines _BYTE_ORDER to one of the above. */
#include <kern/machine/endian.h>


#endif /* _KERN_ENDIAN_H_ */
