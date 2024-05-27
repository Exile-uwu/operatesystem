/*
* Filename
: ipc.h
* copyright
: (C) 2006 by zhonghonglie
* Function
: 声明 IPC 机制的函数原型和全局变量
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#define BUFSZ 256
#define CHAIRQUEST 1 // 顾客 理发椅请求标识
#define SOFAQUEST 2 // 顾客 沙发请求标识
#define ROOMQUEST 3 // 顾客 等候室请求标识
#define FINISHED 4 // 理发师对理发店 理发完成标识
#define FINISHED_2 5 // 理发师对顾客 理发完成标识 
#define HAIRQUEST 6 // 理发师 请求理发标识
#define WORKQUEST 7 // 理发师到理发店上班标识
#define MONEYQUEST 8 // 收费标识
// 共享内存的索引，方便阅读
#define count_Chair 0 
#define count_Sofa 1
#define count_Room 2
#define count_Cash 3

/*信号灯控制用的共同体*/
typedef union semuns {
	int val;
} Sem_uns;

/* 消息结构体*/
typedef struct msgbuf {
	long mtype;
	int mid;
} Msg_buf;

static int id[3]; // 3位理发师的id（进程号）

static key_t buff_key;
static int buff_num;
static char *buff_ptr;
static int shm_flg;
static int quest_flg;
static key_t quest_key;
static int quest_id;
static int respond_flg;
static key_t respond_key;
static int respond_id;
static int quest_flg2;
static key_t quest_key2;
static int quest_id2;
static int respond_flg2;
static key_t respond_key2;
static int respond_id2;

int get_ipc_id(char *proc_file,key_t key);
char *set_shm(key_t shm_key,int shm_num,int shm_flag);
int set_msq(key_t msq_key,int msq_flag);
int set_sem(key_t sem_key,int sem_val,int sem_flag);
int down(int sem_id);
int up(int sem_id);
