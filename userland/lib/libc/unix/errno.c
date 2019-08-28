#include <errno.h>

/*
 * Source file that declares the space for the global variable errno.
 *
 * We also declare the space for __argv, which is used by the err*
 * functions, and __environ, which is used by getenv(). Since these
 * are set by crt0, they are always referenced in every program;
 * putting them here prevents gratuitously linking all the err* and
 * warn* functions (and thus printf) into every program.
 */

char **__argv;
char **__environ;

int errno;
