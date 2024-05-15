/*
 * Filename : ppipe.c
 * copyright : (C) 24 by Exile
 * Function : 利用管道实现在父子进程间传递整数
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

const int pidnum = 2; // 子进程数量

int f(int i, int x) {
  if (i == 0) {
    if (x == 1) {
      return 1;
    } else {
      return f(i, x - 1) * x;
    }
  }
  if (i == 1) {
    if (x == 1 || x == 2) {
      return 1;
    } else {
      return f(i, x - 1) + f(i, x - 2);
    }
  }
  return -1;
}

int main() {
  int pid[pidnum];         // 进程号
  int pipe0[pidnum][2][2]; // 管道
  int x[] = {-1, -1};      // 参数值
  int res = 0;             // 结果值
  int status[pidnum];

  // 初始化参数值
  if (x[0] == -1 && x[1] == -1)
    for (int i = 0; i <= 1; i++) {
      printf("请输入参数%d的值:", i);
      scanf("%d", &x[i]);
    }

  // 使用 pipe()系统调用建立两个无名管道。建立不成功程序退出，执行终止
  for (int i = 0; i < pidnum; i++)
    for (int j = 0; j <= 1; j++) {

      if (pipe(pipe0[i][j]) < 0) {
        printf("管道建立失败.");
        exit(EXIT_FAILURE);
      }
    }
  // 建立子进程.
  for (int i = 0; i < pidnum; i++)
    if ((pid[i] = fork()) < 0) {
      printf("子进程建立失败.");
      exit(EXIT_FAILURE);
    }

    // 子进程0或1
    else if (pid[i] == 0) {

      // 管道0的0端读,管道1的1端写.
      // 关闭不用的管道.
      close(pipe0[i][0][1]);
      close(pipe0[i][1][0]);

      read(pipe0[i][0][0], &x[i], sizeof(int));
      printf("子进程 %d 读取参数 %d 为: %d\n", getpid(), i, x[i]);
      // 执行不同的函数
      res = f(i, x[i]);
      printf("子进程 %d 执行完成, 得到参数 %d 结果为 %d\n", getpid(), i, res);
      write(pipe0[i][1][1], &res, sizeof(int));

      // 关闭所有管道
      close(pipe0[i][1][1]);
      close(pipe0[i][0][0]);

      // 子进程执行结束
      exit(EXIT_SUCCESS);
    }

  // 父进程
  // 管道 0 的 0 端写,管道 1 的 1 端读.
  // 关闭不用的管道.
  for (int i = 0; i < pidnum; i++) {
    close(pipe0[i][0][0]);
    close(pipe0[i][1][1]);
  }

  for (int i = 0; i < pidnum; i++) {
    write(pipe0[i][0][1], &x[i], sizeof(int));
    read(pipe0[i][1][0], &x[i], sizeof(int));
    waitpid(pid[i], &status[i], 0);

    res += x[i];
  }

  printf("父进程 %d 得到f(x,y)为: %d\n", getpid(), res);

  // 读写完成后,关闭管道
  for (int i = 0; i < pidnum; i++) {
    close(pipe0[i][0][1]);
    close(pipe0[i][1][0]);
  }

  // 父进程执行结束
  return EXIT_SUCCESS;
}