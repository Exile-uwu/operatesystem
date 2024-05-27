/*
 * consumer_tobacco.c
 * 
 * Copyright 2020 ALTLI <altli@ALTLIDesktop>
 * 
 * Data: 2020/6/1
 * 
 * Function: 理发店程序，是一个控制程序
 * 
 */

#include "ipc.h"

int main()
{
	Msg_buf msg_arg;
	struct msqid_ds msg_inf;
	// 建立一个共享内存
	buff_key = 1001;
	buff_num = 7;
	shm_flg = IPC_CREAT | 0644;
	buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg);
	buff_ptr[count_Chair] = 3; // 理发椅空闲的数量
	buff_ptr[count_Sofa] = 4; // 沙发空闲的数量
	buff_ptr[count_Room] = 13; // 等候室空闲的数量
	buff_ptr[count_Cash] = 1; // 现金登记册是否可用（1可用，0不可用）
	// 建立一条请求消息队列
	quest_key = 2001;
	quest_flg = IPC_CREAT| 0644;
	quest_id = set_msq(quest_key, quest_flg);
	// 建立一条响应消息队列
	respond_key = 2002;
	respond_flg = IPC_CREAT|0644;
	respond_id = set_msq(respond_key,respond_flg);
	// 下面的消息队列专门用于收费
	// 建立一条请求消息队列
	quest_key2 = 3001;
	quest_flg2 = IPC_CREAT| 0644;
	quest_id2 = set_msq(quest_key2, quest_flg2);
	// 建立一条响应消息队列
	respond_key2 = 4002;
	respond_flg2 = IPC_CREAT|0644;
	respond_id2 = set_msq(respond_key2,respond_flg2);
	
	int hairCutters_state[3]; // 3位理发师的状态，0为睡觉，1为理发或收费中
	// 等那3位理发师都到了ALTLI理发店，就开始营业（接收3位理发师上班请求）
	for (int i = 0; i < 3; i++)
		if (msgrcv(quest_id, &msg_arg, sizeof(msg_arg), WORKQUEST, 0) >= 0)
		{
			id[i] = msg_arg.mid;
			hairCutters_state[i] = 0;
			printf("%d 理发师到岗\n", msg_arg.mid);
			// 发给理发师，在数组中的索引，用于收费部分的处理
			msg_arg.mtype = msg_arg.mid;
			msg_arg.mid = i;
			msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0); 
		}
	
	// 理发店准备迎接顾客
	printf("ALTLI理发店开始营业！\n\n欢迎来到ALTLI理发店！\n");
	
	while (1)
	{
		// 当count_Room大于0时，说明等候室有多余空闲的位置，查询街上是否有顾客想来理发
		if (buff_ptr[count_Room] > 0)
		{
			quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
			if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), ROOMQUEST, quest_flg) >= 0)
			{
				// 街上有顾客，允许ta来等候室
				buff_ptr[count_Room]--;
				msg_arg.mtype = msg_arg.mid;
				msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
				printf("%d 顾客请求进入等候室\n", msg_arg.mid);
			}
		}
		
		// 当count_Sofa大于0时，说明沙发有多余空闲的位置，查询等候室是否有顾客等待
		if (buff_ptr[count_Sofa] > 0)
		{
			quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
			if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), SOFAQUEST, quest_flg) >= 0)
			{
				// 等候室中有顾客，允许ta坐沙发
				buff_ptr[count_Sofa]--;
				buff_ptr[count_Room]++;
				msg_arg.mtype = msg_arg.mid;
				printf("%d 顾客请求坐沙发\n", msg_arg.mid);
				msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
			}
		}
		
		// 当count_Chair大于0时，说明理发椅有多余空闲的位置，查询沙发上是否有客人等待
		if (buff_ptr[count_Chair] > 0)
		{
			quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
			if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), CHAIRQUEST, quest_flg) >= 0)
			{
				// 沙发上有顾客请求理发，允许ta理发
				buff_ptr[count_Chair]--;
				buff_ptr[count_Sofa]++;
				for (int i = 0; i < 3; i++) // 找一个空闲的理发师理发
					if (!hairCutters_state[i])
					{
						msg_arg.mtype = id[i];
						hairCutters_state[i] = 1;
						break;
					}
				msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0); // 给理发师发理发消息
				msg_arg.mtype = msg_arg.mid;
				msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0); // 给顾客发理发消息
				printf("%d 顾客请求理发\n", msg_arg.mid);
			}
		}
		
		// 接收到一个理发或收费完成的消息
		quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
		if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), FINISHED, quest_flg) >= 0)
		{
			// 理发店老板收到理发师发送的理发或收费完成信号
			buff_ptr[count_Chair]++;
			for (int i = 0; i < 3; i++)
				if (msg_arg.mid == id[i])
				{
					hairCutters_state[i] = 0;
					break;	
				}
		}
		
		// 接收到一个顾客付钱的消息
		quest_flg = IPC_NOWAIT; // 以非阻塞方式接收请求消息
		if(msgrcv(quest_id, &msg_arg, sizeof(msg_arg), MONEYQUEST, quest_flg) >= 0)
		{
			// 理发店让顾客指定的理发师收钱
			msg_arg.mtype = id[msg_arg.mid%10];
			msg_arg.mid = msg_arg.mid/10;
			hairCutters_state[msg_arg.mid%10] = 1;
			msgsnd(quest_id2, &msg_arg, sizeof(msg_arg), 0); // 给理发师发收费消息
		}
	}
	
	return EXIT_SUCCESS;
}
