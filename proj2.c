#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>
#include <conio.h>
#include  <time.h>

#include  "app_cfg.h"

#define         OS_TIME_DELAY
#define         LOOP_CNT         250

#define         TASK_STK_SIZE    256      /* Size of each task's stacks (# of OS_STK)      */
#define         MSG_QUEUE_SIZE   100

OS_STK          TaskStartStk[TASK_STK_SIZE];   /* TaskStart()�Լ��� ���ÿ���                */
OS_STK          Task1Stk[TASK_STK_SIZE];       /* Task #1    task stack                     */
OS_STK          Task2Stk[TASK_STK_SIZE];       /* Task #2    task stack                     */
OS_STK          Task3Stk[TASK_STK_SIZE];       /* Task #3    task stack                     */

#define     TITLE_DELAY()   OSTimeDly(OS_TICKS_PER_SEC)
#define     LOOP_DELAY()    OSTimeDly(1)

void TaskStart(void* pdat);
void Task1(void* pdat);
void Task2(void* pdat);
void Task3(void* pdat);

OS_EVENT* Mbox1;
OS_EVENT* Mbox2;
OS_EVENT* Mbox3;
OS_EVENT* MsgQueue;


void* MsgQueueTbl[20];

int main(int argc, char* argv[]) {
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif

    //   PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);
    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();
    OSInit();                                                 /* uC/OS-II�� �ʱ�ȭ�Ѵ�    */

    /* ���� �½�ũ ���� */
    OSTaskCreate(TaskStart, (void*)0, (void*)&TaskStartStk[TASK_STK_SIZE - 1], 5);

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(APP_CFG_STARTUP_TASK_PRIO,
        (INT8U*)"Startup Task",
        &os_err);
#endif

    OSStart();                                                  /* uC/OS-II �⵿            */

    return 0;
}

void TaskStart(void* pdat) {

#define MAX_MAILBOXES 100
    INT8U err;
    char keybord;
    int rand_num;
    char* mbox;
    char* token = NULL;
    char* message = NULL;
    char* str;
    int cnt = 0;
    OS_EVENT** Mboxes[] = { &Mbox1, &Mbox2, &Mbox3 }; // ������ mailbox ���ȭ
    message = rand() % 26 + 65; // ���� �޽��� ����

    srand((unsigned)time(NULL));

    MsgQueue = OSQCreate(&MsgQueueTbl[0], MSG_QUEUE_SIZE);
    Mbox1= OSMboxCreate((void*)0);
    Mbox2 = OSMboxCreate((void*)0);
    Mbox3 = OSMboxCreate((void*)0);

    err = 0;
    err += OSTaskCreate(Task1, (void*)0, &Task1Stk[TASK_STK_SIZE - 1], 11);
    err += OSTaskCreate(Task2, (void*)0, &Task2Stk[TASK_STK_SIZE - 1], 12);
    err += OSTaskCreate(Task3, (void*)0, &Task3Stk[TASK_STK_SIZE - 1], 13);

    printf("=======================================\n");
    printf("\tSytem Start\n");
    printf("=======================================\n");

    OSMboxPost(*Mboxes[rand() % 3], (void*)message); 

    while (1) {
        str = (char*)OSQPend(MsgQueue, 50, &err);
        if (err != OS_ERR_TIMEOUT) {
            cnt++;
            printf("%s\n", str);
            token = strtok_s(str, ",", &message);
            token = strtok_s(token, ":", &message);
            if (*context == 'Z') {
                printf("Received message from Task : %d\n", cnt); //�޽��� ����
                printf("Continue?(y/n)"); //������� ����
                if (_getch() == 'y') { //getch�� ����Ȯ��
                    printf("\n");
                    continue;
                }
                else {
                    OSQDel(MsgQueue, OS_DEL_ALWAYS, &err);
                    OSMboxDel(Mbox1, OS_DEL_ALWAYS, &err);
                    OSMboxDel(Mbox2, OS_DEL_ALWAYS, &err);
                    OSMboxDel(Mbox3, OS_DEL_ALWAYS, &err);

                    OSTaskDel(10);
                    OSTaskDel(11);
                    OSTaskDel(12);
                    OSTaskDel(13);
                    OSTaskDel(OS_PRIO_SELF);
                    break;
                }
            }
            TITLE_DELAY();
        }
        else printf("Queue Time Out!!\n");
    }
    printf("=======================================\n");
    printf("\tExit\n");
    printf("=======================================\n");
}


void Task1(void* pdat) { 
    INT8U err;
    INT16U cnt = 0;
    int taskid = 1;
    char* charptr;
    char str[256];
    srand((unsigned)time(NULL));

    for (;;) {
        
        charptr = (char*)OSMboxPend(Mbox1, 1, &err);
        if (err != OS_ERR_TIMEOUT) {
            sprintf_s(str, sizeof(str), "task%d:%c, %d timeout", taskid, charptr, cnt);
            OSQPost(MsgQueue, (void*)str, sizeof(str), OS_POST_OPT_NONE, &err);
            OSMboxPost(Mbox2, (void*)(rand() % 26 + 65)); //Mbox2�� �Ѱ���
            cnt = 0;
        }
        else {
            cnt += 1;
        }
        TITLE_DELAY();
    }
}

void Task2(void* pdat) { 
    INT8U err;
    INT16U cnt = 0;
    int taskid = 2;
    char* charptr;
    char str[256];
    srand((unsigned)time(NULL));

    for (;;) {
        
        charptr = (char*)OSMboxPend(Mbox2, 1, &err);
        if (err != OS_ERR_TIMEOUT) {
            sprintf_s(str, sizeof(str), "task%d:%c, %d timeout", taskid, charptr, cnt);
            OSQPost(MsgQueue, (void*)str, sizeof(str), OS_POST_OPT_NONE, &err);
            OSMboxPost(Mbox3, (void*)(rand() % 26 + 65)); //Mbox3���� �Ѱ���
            cnt = 0;
        }
        else {
            cnt += 1;
        }
        TITLE_DELAY();
    }
}

void Task3(void* pdat) { 
    INT8U err;
    INT16U cnt = 0;
    int taskid = 3;
    char* charptr;
    char str[256];
    srand((unsigned)time(NULL));

    for (;;) {
        
        charptr = (char*)OSMboxPend(Mbox3, 1, &err);
        if (err != OS_ERR_TIMEOUT) {
            sprintf_s(str, sizeof(str), "task%d:%c, %d timeout", taskid, charptr, cnt);
            OSQPost(MsgQueue, (void*)str, sizeof(str), OS_POST_OPT_NONE, &err); //������ �Ѱ��ְ� ������ ����ó��
            OSMboxPost(Mbox1, (void*)(rand() % 26 + 65)); //Mbox1�� �Ѱ���
            cnt = 0;
        }
        else {
            cnt += 1;
        }
        TITLE_DELAY();
    }
}