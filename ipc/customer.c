/*
 * consumer_tobacco.c
 * 
 * Copyright 2020 ALTLI <altli@ALTLIDesktop>
 * 
 * Data: 2020/6/1
 * 
 * Function: 顾客程序
 * 
 */
 
#include "ipc.h"

int main(int argc,char *argv[])
{
	int i;
	Msg_buf msg_arg;
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
	
	// 随机数，用于收费时随机选择一个理发师
	int a;
    srand((unsigned)time(NULL));
    a = rand()%3; // 产生0~2范围内的随机数
	
	// 此时说明有空余的理发椅，没有顾客等待，请求理发
	if (buff_ptr[count_Chair] > 0 && buff_ptr[count_Sofa] == 4)
	{
		msg_arg.mtype = CHAIRQUEST;
		buff_ptr[count_Sofa]--;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发中...\n", msg_arg.mid);
		msgrcv(quest_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发完成\n", msg_arg.mid);
		// 向理发店付钱
		msg_arg.mtype = MONEYQUEST;
		msg_arg.mid = msg_arg.mid*10 + a; // 将顾客选择的理发师放在mid的最后一位
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msg_arg.mid = getpid();
		msgrcv(respond_id2, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客离开理发店\n", msg_arg.mid);
	}
	// 此时说明沙发中还有位置，请求去沙发等待
	else if (buff_ptr[count_Chair] == 0 && buff_ptr[count_Sofa] > 0)
	{
		msg_arg.mtype = SOFAQUEST;
		buff_ptr[count_Room]--;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客在沙发等待中...\n", msg_arg.mid);
		// 请求理发
		msg_arg.mtype = CHAIRQUEST;
		buff_ptr[count_Sofa]--;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发中...\n", msg_arg.mid);
		msgrcv(quest_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发完成\n", msg_arg.mid);
		// 向理发店付钱
		msg_arg.mtype = MONEYQUEST;
		msg_arg.mid = msg_arg.mid*10 + a; // 将顾客选择的理发师放在mid的最后一位
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msg_arg.mid = getpid();
		msgrcv(respond_id2, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客离开理发店\n", msg_arg.mid);
	}
	// 此时说明等候室中还有位置，请求去等候室等待
	else if (buff_ptr[count_Sofa] == 0 && buff_ptr[count_Room] > 0)
	{
		msg_arg.mtype = ROOMQUEST;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客在等候室等待中...\n", msg_arg.mid);
		// 请求去沙发等待
		msg_arg.mtype = SOFAQUEST;
		buff_ptr[count_Room]--;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客在沙发等待中...\n", msg_arg.mid);
		// 请求理发
		msg_arg.mtype = CHAIRQUEST;
		buff_ptr[count_Sofa]--;
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msgrcv(respond_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发中...\n", msg_arg.mid);
		msgrcv(quest_id, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客理发完成\n", msg_arg.mid);
		// 向理发店付钱
		msg_arg.mtype = MONEYQUEST;
		msg_arg.mid = msg_arg.mid*10 + a; // 将顾客选择的理发师放在mid的最后一位
		msgsnd(quest_id, &msg_arg, sizeof(msg_arg), 0);
		msg_arg.mid = getpid();
		msgrcv(respond_id2, &msg_arg, sizeof(msg_arg), msg_arg.mid, 0);
		printf("%d 顾客离开理发店\n", msg_arg.mid);
	}
	// 此时说明人数超过20，不进入理发店（如果已经进了理发店，那么就不会判断是否人数超过20）
	else
	{
		printf("%d 顾客去其他理发店理发\n", msg_arg.mid);
	}
	
	return EXIT_SUCCESS;
}
