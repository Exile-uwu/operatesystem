#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int k = 10;

typedef void (*sighandler_t)(int);
void sigcat() {
  //+1
  setpriority(PRIO_PROCESS, getpid(), getpriority(PRIO_PROCESS, 0) + 1);
  printf("优先数+1\n");
}
void sigzat() {
  //-1
  setpriority(PRIO_PROCESS, getpid(), getpriority(PRIO_PROCESS, 0) - 1);
  k--;
  printf("优先数-1\n");
}
void donothing() {}
int main(int argc, char *argv[]) {
  printf("***按 Ctrl + C 为父进程优先数加一***\n***按 Ctrl + Z "
         "为子进程优先数减一***\n");
  int i, j, status;
  int pid;                 // 存放进程号
  struct sched_param p[2]; // 设置调度策略时使用的数据结构
  p[0].sched_priority = 10;
  p[1].sched_priority = 10;
  pid = fork();
  if (pid < 0) {
    printf("failed!");
  } else if (pid > 0) {
    signal(SIGINT, (sighandler_t)sigcat);
    signal(SIGTSTP, (sighandler_t)donothing);

    sched_setscheduler(getpid(), SCHED_OTHER, &p[0]);
    setpriority(PRIO_PROCESS, getpid(), 10);
    sleep(1);
    for (i = 0; i < 10; i++) {
      printf("父进程号 = %d ;优先数 = %d ;调度策略 = %d\n", getpid(),
             getpriority(PRIO_PROCESS, 0), sched_getscheduler(getpid()));
      pause();
    }
  }
  // 各子进程循环报告其优先数和调度策略
  else {
    signal(SIGINT, (sighandler_t)donothing);
    signal(SIGTSTP, (sighandler_t)sigzat);
    sched_setscheduler(getpid(), SCHED_OTHER, &p[1]);
    setpriority(PRIO_PROCESS, getpid(), 10);
    sleep(1);

    for (i = 0; i < 10; i++) {
      printf("子进程号 = %d ;优先数 = %d ;调度策略 = %d\n", getpid(),
             k, sched_getscheduler(getpid()));
      pause();
    }
    exit(EXIT_SUCCESS);
  }
  return EXIT_SUCCESS;
}