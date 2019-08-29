#include <unistd.h>
#include <stdio.h>

int main()
{
  int pid;

  pid = getpid();
  nprintf("%d\n", pid);
  reboot(RB_REBOOT);

  return 0;
}
