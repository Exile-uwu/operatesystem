/*
 * consumer_tobacco.c
 * 
 * Copyright 2020 ALTLI <altli@ALTLIDesktop>
 * 
 * Data: 2020/6/1
 * 
 * Function: 理发师程序
 * 
 */

#include "ipc.h"

int main(int argc,char *argv[])
{
	int i;
	int rate1, rate2; // rate1是理发速度，rate2是收费速度
	Msg_buf msg_arg;
	// 可在在命令行第一参数指定一个进程睡眠秒数，以调解进程执行速度
	if(argv[1] != NULL) 
		rate1 = atoi(argv[1]);
	else 
		rate1 = 3;
	if (argv[2] != NULL)
		rate2 = atoi(argv[2]);
	else
		rate2 = 1;

	// 附加一个要读内容的共享内存
	buff_key = 1001;
	buff_num = 7;
	shm_flg = IPC_CREAT | 0644;
	buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
	// 联系一个请求消息队列
	quest_flg = IPC_CREAT | 0644;
	quest_key = 2001;
	quest_id = set_msq(quest_key, quest_flg);
	// 联系一个响应消息队列
	respond_flg = IPC_CREAT | 0644;
	respond_key = 2002;
	respond_id = set_msq(respond_key, respond_flg);
	// 下面的消息队列专门用于收费
	// 建立一条请求消息队列
	quest_key2 = 3001;
	quest_flg2 = IPC_CREAT| 0644;
	quest_id2 = set_msq(quest_key2, quest_flg2);
	// 建立一条响应消息队列
	respond_key2 = 4002;
	respond_flg2 = IPC_CREAT|0644;
	respond_id2 = set_msq(respond_key2,respond_flg2);
	
	msg_arg.mid = getpid();
	// 上班前给ALTLI理发店发消息，表示我开始上班
	msg_arg.mtype = WORKQUEST;
	msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
	msgrcv(respond_id, &msg_arg, sizeof(msg_arg), getpid(), 0);
	int flag = msg_arg.mid;
	msg_arg.mid = getpid();
	printf("%d 理发师上班\n", msg_arg.mid);
	printf("%d 理发师睡觉中...\n", getpid());
	
	// 循环：理发、收费、睡觉
	while (1)
	{
		if (msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, IPC_NOWAIT) >= 0)
		{
			printf("%d 理发师为 %d 顾客理发中...\n", getpid(), msg_arg.mid);
			sleep(rate1);
			printf("%d 顾客理发完成\n", msg_arg.mid);
			// 理发师通知顾客理发完成，让顾客找一个理发师收费
			msg_arg.mtype = msg_arg.mid;
			msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
			sleep(1);
			if (msgrcv(quest_id2, &msg_arg, sizeof(msg_arg), getpid(), IPC_NOWAIT) >= 0)
			{	
				// 顾客令该理发师收费
				while (1)
				{
					if (buff_ptr[count_Cash])
					{
						buff_ptr[count_Cash] = 0;
						printf("%d 理发师收取 %d 顾客费用中...\n", getpid(), msg_arg.mid);
						sleep(rate2); 
						buff_ptr[count_Cash] = 1;
						msg_arg.mtype = msg_arg.mid;
						printf("收费完毕\n");
						msgsnd(respond_id2, &msg_arg, sizeof(msg_arg), 0); // 向顾客发送收费完成信号
						break;
					}
				}
			}
			// 理发师通知理发店老板理发完成，请求给下一个顾客理发
			msg_arg.mtype = FINISHED;
			msg_arg.mid = getpid();
			msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
			if (buff_ptr[count_Sofa] == 4)
				printf("%d 理发师睡觉中...\n", getpid());
		}
		if (msgrcv(quest_id2, &msg_arg, sizeof(msg_arg), getpid(), IPC_NOWAIT) >= 0)
		{
			// 顾客令该理发师收费
			while (1)
			{
				if (buff_ptr[count_Cash])
				{
					buff_ptr[count_Chair]--;
					buff_ptr[count_Cash] = 0;
					printf("%d 理发师收取 %d 顾客费用中...\n", getpid(), msg_arg.mid);
					sleep(rate2); 
					buff_ptr[count_Cash] = 1;
					msg_arg.mtype = msg_arg.mid;
					printf("收费完毕\n");
					msgsnd(respond_id2, &msg_arg, sizeof(msg_arg), 0); // 向顾客发送收费完成信号
					msg_arg.mtype = FINISHED;
					msg_arg.mid = getpid();
					msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0); // 向理发店发送收费完成信号
					break;
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
