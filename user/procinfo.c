#include "kernel/types.h"
#include "user/user.h"
#include "procinfo_s.h"

int
main(int argc, char *argv[])
{
  struct procinfo info;
  int pid;

  if(argc > 1){
    pid = atoi(argv[1]);
  } else {
    pid = getpid();
  }

  if(procinfo(pid, &info) == 0){
    printf("Process name: %s\n", info.name);
    printf("PID: %d\n", info.pid);
    printf("PPID: %d\n", info.ppid);
    printf("State: %d\n", info.state);
    printf("Memory: %ld bytes\n", info.sz);
  } else {
    printf("procinfo syscall failed\n");
  }

  exit(0);
}