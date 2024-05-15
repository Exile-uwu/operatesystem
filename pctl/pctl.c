/*

父子协作进程
24.4.22

实现功能:父进程控制子进程,每三秒打印文件列表.

*/

#include "pctl.h"

int main() {


  // 子进程号
  int pid;
  // 子进程状态
  int status;
  // 初始化信号
  signal(SIGUSR1, f);

  // 创建新进程
  pid = fork();

  // pid<0:进程创建失败;
  if (pid < 0) {
    printf("创建进程失败!\n");
    exit(EXIT_FAILURE);
  }

  // pid=0:目前进行的是子进程;
  if (pid == 0) {
    printf("子进程: %d 运行正常...\n", getpid());

    while (1) {
      // 进程暂停，等待父进程发送信号;
      pause();
      // 执行命令ls;
      system("/bin/ls");
    }
  }

  // 父进程执行代码段
  else {
    printf("父进程: %d 运行正常...\n", getpid()); // 报告父进程进程号
    while (1) {
      sleep(3);
      //向子进程pid发送SIGUSR1信号,激活进程.
      kill(pid, SIGUSR1);
    }
    // 等待子进程结束
    waitpid(pid, &status, 0);
    printf("子进程结束了! 状态为: %d\n", status);
  }
  return EXIT_SUCCESS;
}
